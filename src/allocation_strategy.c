#include <unistd.h>
#include <stdio.h>

#include "constants.h"
#include "allocation_strategy.h"
#include "uthash.h"
#include "pqueue.h"
#include "file_md.h"
#include "sshdd.h"
#include "allocation_strategy_helper.h"

void * allocation_strategy(void *sshdd_handle) {
	// Get the sshdd handle
	sshdd_t *sshdd = (sshdd_t *) sshdd_handle;

	//Create the priority queues for HDD, SSD
	pqueue_t *max_pq = NULL; //HDD
	pqueue_t *min_pq = NULL; //SSD

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

	int hdd_file_ctr = 0, ssd_file_ctr = 0;
	for (file_md_ptr = head; file_md_ptr != NULL;
			file_md_ptr = file_md_ptr->hh.next) {
		printf("file id %s: loc:%d\n", file_md_ptr->fileid, file_md_ptr->loc);

		//Construct the as_node
		as_node_t *ns = malloc(sizeof(as_node_t));
		if (!ns)
			return NULL;

		//Initialize
		ns->file_md_ptr = file_md_ptr;
		ns->pos = 0;

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
	}
	printf("Files added to queues, max_pq[%d], min_pq[%d]", hdd_file_ctr,
			ssd_file_ctr);

	//Print the priority queues
	pqueue_print(max_pq, stdout, printq_elem);
	pqueue_print(min_pq, stdout, printq_elem);

	// Periodically move the files from HDD to SSD
	// based on SSD space and priority queues
	while (sshdd->the_end == 0) {
		//Peek the top node from HDD
		as_node_t *max_pq_node;
		max_pq_node = pqueue_peek(max_pq);
		file_md_t *hdd_file_md_ptr = max_pq_node->file_md_ptr;

		// TODO : lock hdd peeked file

		//Check if the file is open
		if (hdd_file_md_ptr->ref_count) {
			continue; //File is open
		}

		//Get the file size
		int file_size_hdd = get_file_size(hdd_file_md_ptr->fileid);

		// Check if SSD has enough space
		int curr_size_ssd = get_folder_size(sshdd->ssd_folder);
		// fill SSD part of the algorithm
		if (sshdd->ssd_max_size > curr_size_ssd + file_size_hdd) {
			//Pop the max node
			max_pq_node = pqueue_pop(max_pq);
			hdd_file_md_ptr = max_pq_node->file_md_ptr;

			//Move the file from HDD to SSD
			char srcPath[FNAME_SIZE] = { 0 };
			char destPath[FNAME_SIZE] = { 0 };

			snprintf(srcPath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
					hdd_file_md_ptr->fileid);
			snprintf(destPath, FNAME_SIZE, "%s/%s", sshdd->ssd_folder,
					hdd_file_md_ptr->fileid);

			if (rename(srcPath, destPath)) {
				printf("ERROR moving %s HDD->SSD\n", srcPath); // something went wrong
				//TODO: cleanup (remove locks)
				continue;//continue to next iteration if this fails
			} else { // the rename succeeded
				printf("Successful moving %s HDD->SSD\n", srcPath);
			}

			//Update the file metadata structure inside the nodes
			hdd_file_md_ptr->loc = SSD; //Change location from HDD to SSD//Change location from HDD to SSD

			//Put the node into min_pq
			pqueue_insert(min_pq, max_pq_node);

			// TODO : Unlock hdd peeked file
		} else { // swap between SSD and HDD part of the algorithm
			// There is not enough space in SSD
			// See if some file from SSD can be swapped
			// based on THRESHOLD value
			as_node_t *min_pq_node;
			min_pq_node = pqueue_peek(min_pq);
			file_md_t *ssd_file_md_ptr = min_pq_node->file_md_ptr;

			// TODO : lock ssd peeked file

			//Check if the file is open
			if (ssd_file_md_ptr->ref_count) {
				continue; //File is open
			}

			// TODO : optimize assuming read only FS
			int curr_size_hdd = get_folder_size(sshdd->hdd_folder);
			//Get the file size
			int file_size_ssd = get_file_size(ssd_file_md_ptr->fileid);

			int new_sz_hdd = curr_size_hdd + file_size_ssd;
			int new_sz_ssd = curr_size_ssd - file_size_ssd + file_size_hdd;

			//Check if HDD has enough space to hold both files
			//And SSD has space to hold the new file
			if ((new_sz_hdd < sshdd->hdd_max_size)
					&& (new_sz_ssd < sshdd->ssd_max_size)) {
				printf("Files can be interchanged within hdd, ssd caps\n");
				printf("Pop the top nodes from both hdd, ssd pri queues\n");
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

				if (rename(srcPath, destPath)) {
					printf("ERROR moving %s SSD->HDD\n", srcPath);// something went wrong
					//TODO: cleanup (remove locks)
					continue;//continue to next iteration if this fails
				} else { // the rename succeeded
					printf("Successful moving %s SSD->HDD\n", srcPath);
				}

				//Move the file from HDD to SSD
				snprintf(srcPath, FNAME_SIZE, "%s/%s", sshdd->hdd_folder,
						hdd_file_md_ptr->fileid);
				snprintf(destPath, FNAME_SIZE, "%s/%s", sshdd->ssd_folder,
						hdd_file_md_ptr->fileid);

				if (rename(srcPath, destPath)) {
					printf("ERROR moving %s HDD->SSD\n", srcPath); // something went wrong
					//TODO: cleanup (remove locks)
					//TODO: Rollback the last move
					continue;//continue to next iteration if this fails
				} else { // the rename succeeded
					printf("Successful moving %s HDD->SSD\n", srcPath);
				}

				//Update the file metadata structure inside the nodes
				hdd_file_md_ptr->loc = SSD; //Change location from HDD to SSD
				ssd_file_md_ptr->loc = HDD; //Change location from SSD to HDD

				//Put the node into min_pq
				pqueue_insert(max_pq, min_pq_node);
				pqueue_insert(min_pq, max_pq_node);

				//TODO : Unlock the SSD file
			}

			//TODO : Unlock the HDD file

			sleep(1);

			//TODO: Heapify the priority queues
		}
	}

	// free the queues
	while(pqueue_size(max_pq) > 0) {
		free(pqueue_pop(max_pq));
	}
	pqueue_free(max_pq);

	while(pqueue_size(min_pq) > 0) {
		free(pqueue_pop(min_pq));
	}
	pqueue_free(min_pq);

	return NULL;
}
