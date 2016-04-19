#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>

#include "constants.h"
#include "uthash.h"
#include "file_md.h"
#include "pqueue.h"
#include "sshdd.h"
#include "sshdd_handle.h"
#include "allocation_strategy.h"
#include "sshdd_helper.h"
#include "sshdd_helper.h"

void* sshdd_init(sshdd_conf_t *conf) {
	sshdd_t* sshdd = malloc(sizeof(struct sshdd_t));

	// initialize sshdd
	sshdd->currently_open = 0;
	sshdd->optimize = conf->optimize;
	strcpy(sshdd->ssd_folder, conf->ssd_folder);
	strcpy(sshdd->hdd_folder, conf->hdd_folder);
	sshdd->ht_file_md_head = NULL;
	sshdd->ssd_max_size = conf->ssd_max_size;
	sshdd->hdd_max_size = conf->hdd_max_size;
	sshdd->the_end = 0;

	// index all files in HDD and SSD folders
	int len = build_metadata_for_folder(conf->ssd_folder, SSD,
			&sshdd->file_md[0], &(sshdd->ht_file_md_head));
	len += build_metadata_for_folder(conf->hdd_folder, HDD,
			&sshdd->file_md[len], &(sshdd->ht_file_md_head));
	fprintf(stderr, "Indexed %d files in total\n", len);

	if (sshdd->optimize != 0) {
		struct mq_attr attr;
	    // initialize the queue attributes
	    attr.mq_flags = 0;
	    attr.mq_maxmsg = NUM_MSGS;
	    attr.mq_msgsize = MSG_SIZE;
	    attr.mq_curmsgs = 0;
	    mq_unlink(QUEUE_NAME);
		sshdd->mq_writer = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY | O_NONBLOCK, 0644, &attr);
		if (sshdd->mq_writer == -1) {
			fprintf(stderr, "Cannot create write message queue\n");
			return NULL;
		}
		sshdd->mq_reader = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
		if (sshdd->mq_reader == -1) {
			fprintf(stderr, "Cannot create read message queue\n");
			return NULL;
		}
		// set up sshdd allocation thread
		int status = pthread_create(&(sshdd->as_thread), NULL,
				&allocation_strategy, (void *) sshdd);
		if (status != 0) {
			mq_close(sshdd->mq_writer);
			mq_close(sshdd->mq_reader);
			fprintf(stderr, "Cannot create allocation strategy thread\n");
			return NULL;
		}
	}

	return sshdd;
}

SFILE* sshdd_fopen(void *handle, const char *fileid, const char *mode) {
	// TODO : cannot open a file which is currently getting moved, LOCK

	sshdd_t* sshdd = handle;
	sshdd->currently_open++;

	// Get file_md object from hash table
	file_md_t *ht_file_md = sshdd->ht_file_md_head;
	file_md_t *file_md = NULL;
	HASH_FIND_STR(ht_file_md, fileid, file_md);

	// check where the file is located
	char *folder = NULL;
	if (file_md->loc == SSD) {
		folder = sshdd->ssd_folder;
	} else {
		folder = sshdd->hdd_folder;
	}

	//Update the statistics, if the optimize is enabled
	if (sshdd->optimize != 0) {
		// keep a count of how many times a file is open
		file_md->ref_count++;
		//Increment the MFU counter
		file_md->mfu_ctr++;
		//Update the LRU counter
		file_md->lru_ctr = 0; // TODO : add timestamp
		// send a message about the update
		send_msg(file_md, sshdd->mq_writer);
	}

	// map file id to actual file path
	char filename[FNAME_SIZE];
	sprintf(filename, "%s%s", folder, fileid);

	// create a SFILE struct for the file
	SFILE *sf = malloc(sizeof(SFILE));
	sf->f = fopen(filename, mode);
	sf->file_md = file_md;

	return sf;
}

int sshdd_fclose(void *handle, SFILE *fs) {
	sshdd_t* sshdd = handle;
	sshdd->currently_open--;

	//Update the statistics, if the optimize is enabled
	if (sshdd->optimize != 0) {
		fs->file_md->ref_count--;
		fs->file_md->lru_ctr = 0; // TODO
		// send a message about the update
		//send_msg(fs->file_md, sshdd->mq_writer); // TODO
	}

	int ret = fclose(fs->f);

	free(fs);

	return ret;
}

size_t sshdd_fread(void *handle, void *ptr, size_t size, size_t nmemb,
		SFILE *fs) {
	sshdd_t* sshdd = handle;

	//Update the statistics, if the optimize is enabled
	if (sshdd->optimize != 0) {
		fs->file_md->mfu_ctr++;
		fs->file_md->lru_ctr = 0; // TODO
		// send a message about the update
		//send_msg(fs->file_md, sshdd->mq_writer); // TODO
	}

	return fread(ptr, size, nmemb, fs->f);
}

size_t sshdd_fwrite(void *handle, const void *ptr, size_t size, size_t nmemb,
		SFILE *fs) {
	sshdd_t* sshdd = handle;

	//Update the statistics, if the optimize is enabled
	if (sshdd->optimize != 0) {
		fs->file_md->mfu_ctr++;
		fs->file_md->lru_ctr = 0; // TODO
		// send a message about the update
		//send_msg(fs->file_md, sshdd->mq_writer); // TODO
	}

	return fwrite(ptr, size, nmemb, fs->f);
}

int sshdd_terminate(void* handle) {
	// TODO : perform basic checking before terminating,
	// also, print statistics before terminating
	// close everything tht was opened in init function
	// end the allocation_strategy thread

	sshdd_t* sshdd = handle;

	// don't terminate if some file is still open
	if (sshdd->currently_open != 0) {
		fprintf(stderr, "Cannot terminate, files still open\n");
	}

	// wait for the allocation strategy thread to stop
	sshdd->the_end = 1;
	pthread_join(sshdd->as_thread, NULL);

	// close the message queue
	mq_close(sshdd->mq_reader);
	mq_close(sshdd->mq_writer);

	free(sshdd);

	return 0;
}
