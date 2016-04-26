#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>

#include "constants.h"
#include "uthash.h"
#include "allocation_strategy.h"
#include "uthash.h"
#include "pqueue.h"
#include "file_md.h"
#include "sshdd.h"
#include "sshdd_handle.h"
#include "allocation_strategy_helper.h"

void * allocation_strategy(void *sshdd_handle) {
	// Get the sshdd handle
	sshdd_t *sshdd = (sshdd_t *) sshdd_handle;

	//Create the priority queues for HDD, SSD
	pqueue_t *max_pq = NULL; //HDD
	pqueue_t *min_pq = NULL; //SSD

	// create a map of fileid to as_node_t
	as_node_t *ht_as_node = NULL;

	//Initialize max priority queue (for hdd files)
	max_pq = pqueue_init(MAX_FILES, cmp_pri_max, get_pri, set_pri, get_pos,
			set_pos);
	if (!max_pq) {
		printf("Error creating pqueue\n");
		return NULL;
	}
	//Initialize min priority queue (for ssd files)
	min_pq = pqueue_init(MAX_FILES, cmp_pri_min, get_pri, set_pri, get_pos,
			set_pos);
	if (!min_pq) {
		printf("Error creating pqueue\n");
		return NULL;
	}

	//Add files to the two queues by iterating the hash table
	file_md_t *head = sshdd->ht_file_md_head;
	file_md_t *file_md_ptr;
	int hdd_file_ctr = 0;
	int ssd_file_ctr = 0;
	for (file_md_ptr = head; file_md_ptr != NULL;
			file_md_ptr = file_md_ptr->hh.next) {
		//Construct the as_node
		as_node_t *ns = malloc(sizeof(as_node_t));
		if (!ns)
			return NULL;

		//Initialize
		ns->file_md_ptr = file_md_ptr;
		ns->pos = 0;
		ns->priority = get_md_priority(file_md_ptr);

		//Insert
		if (HDD == file_md_ptr->loc) {
			pqueue_insert(max_pq, ns);
			hdd_file_ctr++;
		} else if (SSD == file_md_ptr->loc) {
			pqueue_insert(min_pq, ns);
			ssd_file_ctr++;
		} else {
			printf("Invalid file_md_ptr->loc[%d]", file_md_ptr->loc);
			fflush(stdout);
			return NULL;
		}

		// add file_md -> as_node_t mapping
		HASH_ADD_STR(ht_as_node, file_md_ptr->fileid, ns);
	}

	printf("Files added to queues : max_pq[%d], min_pq[%d]\n", hdd_file_ctr,
			ssd_file_ctr);

	// buffer to hold message
	char buffer[MSG_SIZE];
	int bytes_read = 0;

	// remove all older message
	while (1) {
		int bytes_read = mq_receive(sshdd->mq_reader, buffer, MSG_SIZE, NULL);
		if (bytes_read <= 0)
			break;
	}

	// some metrics
	int num_file_moves = 0;
	int num_file_swaps = 0;
	int num_msgs_recv = 0;

	// Periodically move the files from HDD to SSD
	// based on SSD space and priority queues
	while (sshdd->the_end == 0) {

		// Heapify the priority queues after reading messages
		while (1) {
			// receive pending messages
			bytes_read = mq_receive(sshdd->mq_reader, buffer, MSG_SIZE, NULL);
			if (bytes_read <= 0)
				break;

			// parse the message to find file_md and as_node
			pri_update_msg *msg = (pri_update_msg *)buffer;
			file_md_t *file_md = msg->file_md_ptr;
			char *fileid = file_md->fileid;
			as_node_t *as_node = NULL;

			HASH_FIND_STR(ht_as_node, fileid, as_node);

			if (file_md->loc == SSD) {
				pqueue_change_priority(min_pq, get_md_priority(file_md), as_node);
			} else {
				pqueue_change_priority(max_pq, get_md_priority(file_md), as_node);
			}

			num_msgs_recv++;
		}

		//Peek the top node from HDD
		as_node_t *max_pq_node;
		max_pq_node = pqueue_peek(max_pq);
		file_md_t *hdd_file_md_ptr = max_pq_node->file_md_ptr;

		// wait for priority to be high before swapping
		if (get_pri(max_pq_node) < 1) {
			usleep(SLEEP_DUR);
			continue;
		}

		// TODO : lock the HDD file before using its metadata

		//Check if the file is open
		if (hdd_file_md_ptr->ref_count > 0) {
			continue; // File is open
		}

		//Get the file size
		char fpath[FNAME_SIZE] = {0};
		snprintf(fpath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
				hdd_file_md_ptr->fileid);
		int file_size_hdd = get_file_size(fpath);

		// Check if SSD has enough space
		// TODO : optimize assuming read only FS
		int curr_size_ssd = get_folder_size(sshdd->ssd_folder);
		// fill SSD part of the algorithm
		if (sshdd->ssd_max_size > curr_size_ssd + file_size_hdd) {
			//Pop the max node
			max_pq_node = pqueue_pop(max_pq);
			hdd_file_md_ptr = max_pq_node->file_md_ptr;

			//Move the file from HDD to SSD
			char srcPath[FNAME_SIZE] = {0};
			char destPath[FNAME_SIZE] = {0};

			snprintf(srcPath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
					hdd_file_md_ptr->fileid);
			snprintf(destPath, FNAME_SIZE, "%s/%s", sshdd->ssd_folder,
					hdd_file_md_ptr->fileid);

			// string to hold move command
			char command[2 * FNAME_SIZE + 8];
			sprintf(command, "mv %s %s", srcPath, destPath);
			if (system(command) == -1) {
				printf("ERROR moving %s HDD->SSD\n", srcPath); // something went wrong
				continue; //continue to next iteration if this fails
			}

			//Update the file metadata structure inside the nodes
			hdd_file_md_ptr->loc = SSD; //Change location from HDD to SSD//Change location from HDD to SSD

			//Put the node into min_pq
			pqueue_insert(min_pq, max_pq_node);

			num_file_moves++;

		} else { // swap between SSD and HDD part of the algorithm
			// There is not enough space in SSD
			// See if some file from SSD can be swapped
			// based on THRESHOLD value
			as_node_t *min_pq_node;
			min_pq_node = pqueue_peek(min_pq);
			file_md_t *ssd_file_md_ptr = min_pq_node->file_md_ptr;

			// TODO : lock the SSD file before using its metadata

			// Check if the file is open
			if (ssd_file_md_ptr->ref_count > 0) {
				continue; // File is open
			}

			//Get the file size
			snprintf(fpath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
					hdd_file_md_ptr->fileid);

			//Pop the two nodes
			max_pq_node = pqueue_pop(max_pq);
			min_pq_node = pqueue_pop(min_pq);

			//Move the file from SSD to HDD
			char srcPath[FNAME_SIZE] = { 0 };
			char destPath[FNAME_SIZE] = { 0 };

			snprintf(srcPath, FNAME_SIZE, "%s/%s", sshdd->ssd_folder,
					ssd_file_md_ptr->fileid);
			snprintf(destPath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
					ssd_file_md_ptr->fileid);

			// string to hold move command
			char command[2 * FNAME_SIZE + 8];
			sprintf(command, "mv %s %s", srcPath, destPath);
			if (system(command) == -1) {
				printf("ERROR moving %s SSD->HDD\n", srcPath);
				continue;
			}

			//Move the file from HDD to SSD
			snprintf(srcPath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
					hdd_file_md_ptr->fileid);
			snprintf(destPath, FNAME_SIZE, "%s/%s", sshdd->ssd_folder,
					hdd_file_md_ptr->fileid);

			// string to hold move command
			sprintf(command, "mv %s %s", srcPath, destPath);
			if (system(command) == -1) {
				printf("ERROR moving %s HDD->SSD\n", srcPath);
				//TODO: Roll back the last move
				continue;
			}

			//Update the file metadata structure inside the nodes
			hdd_file_md_ptr->loc = SSD; //Change location from HDD to SSD
			ssd_file_md_ptr->loc = HDD; //Change location from SSD to HDD

			//Put the node into min_pq
			pqueue_insert(max_pq, min_pq_node);
			pqueue_insert(min_pq, max_pq_node);

			num_file_swaps++;

			usleep(SLEEP_DUR);
		}
	}

	// free the queues
	while (pqueue_size(max_pq) > 0) {
		free(pqueue_pop(max_pq));
	}
	pqueue_free(max_pq);

	while (pqueue_size(min_pq) > 0) {
		free(pqueue_pop(min_pq));
	}
	pqueue_free(min_pq);

	// print metrics
	printf("Free SSD->HDD moves = %d\n", num_file_moves);
	printf("Swaps between SSD & HDD = %d\n", num_file_swaps);
	printf("Msgs reveiced = %d\n", num_msgs_recv);

	return NULL;
}
