#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "uthash.h"
#include "file_md.h"
#include "pqueue.h"
#include "sshdd.h"
#include "allocation_strategy.h"
#include "sshdd_helper.h"
#include "sshdd_helper.h"

void* sshdd_init(const int optimize, const char *ssd_folder,
		const char *hdd_folder) {
	sshdd_t* sshdd = malloc(sizeof(struct sshdd_t));

	// initialize sshdd
	sshdd->currently_open = 0;
	sshdd->optimize = optimize;
	strcpy(sshdd->ssd_folder, ssd_folder);
	strcpy(sshdd->hdd_folder, hdd_folder);
	sshdd->ht_file_md_head = NULL;
	sshdd->ssd_pq = NULL; // TODO : initialize priority queue
	sshdd->hdd_pq = NULL; // TODO : initialize priority queue

	// index all files in HDD and SSD folders
	int len = build_metadata_for_folder(ssd_folder, SSD, &sshdd->file_md[0],
			sshdd->ssd_pq, &(sshdd->ht_file_md_head));
	len += build_metadata_for_folder(hdd_folder, HDD, &sshdd->file_md[len],
			sshdd->hdd_pq, &(sshdd->ht_file_md_head));
	fprintf(stderr, "Indexed %d files in total\n", len);

	// set up sshdd allocation thread if requested
	if (sshdd->optimize != 0) {
		if (0 != pthread_create(&(sshdd->as_thread), NULL,
						&allocation_strategy, (void *) sshdd)) {
			fprintf(stderr, "Cannot create allocation strategy thread\n");
		}
	}

	return sshdd;
}

FILE* sshdd_fopen(void *handle, const char *fileid, const char *mode) {
	// TODO : cannot open a file which is currently getting moved

	sshdd_t* sshdd = handle;

	sshdd->currently_open++;

	// Get file_md object from hash table
	file_md_t *ht_file_md = sshdd->ht_file_md_head;
	file_md_t *file_md = NULL;
	HASH_FIND_STR(ht_file_md, fileid, file_md);

	// mark file as open
	// TODO : should we keep a count of concurrent opens?
	file_md->is_open = 1;

#if 0
	// debug print
	if (file_md) {
		fprintf(stderr, "Got the file_obj: %s, %s, %d, %d, %d\n", fileid,
				file_md->fileid, file_md->loc, file_md->mfu_ctr,
				file_md->lru_ctr);
	}
#endif

	// check where the file is located
	char *folder = NULL;
	if (file_md->loc == SSD) {
		folder = sshdd->ssd_folder;
	} else {
		folder = sshdd->hdd_folder;
	}

	//Update the statistics, if the optimize is enabled
	if (sshdd->optimize != 0) {
		//Increment the MFU counter
		file_md->mfu_ctr++;
		//Update the LRU counter
		file_md->lru_ctr++; // TODO : add timestamp
	}

	// map file id to actual file path
	char filename[512]; // TODO : hardcode
	sprintf(filename, "%s%s", folder, fileid);

	// TODO : put FILE* in SSHDD_FILE struct, fileid should also be a part of SSHDD_FILE
	return fopen(filename, mode);
}

int sshdd_fclose(void *handle, FILE *stream) {
	// TODO
	// cannot move a file which is open, so bookmark when a file is closed

	sshdd_t* sshdd = handle;

	sshdd->currently_open--;

	return fclose(stream);
}

size_t sshdd_fread(void *handle, void *ptr, size_t size, size_t nmemb,
		FILE *stream) {
	return fread(ptr, size, nmemb, stream);
}

size_t sshdd_fwrite(void *handle, const void *ptr, size_t size, size_t nmemb,
		FILE *stream) {
	return fwrite(ptr, size, nmemb, stream);
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

	free(sshdd);

	return 0;
}
