/*
 * sshdd_handle.h
 *
 *  Created on: Apr 18, 2016
 *      Author: sunil3590
 */

#ifndef SSHDD_HANDLE_H_
#define SSHDD_HANDLE_H_

typedef struct sshdd_t {
	int optimize; // should the algorithm be run?
	int currently_open; // count of currentl
	char ssd_folder[FNAME_SIZE];
	char hdd_folder[FNAME_SIZE];
	file_md_t file_md[MAX_FILES]; // array of meta data objects
	file_md_t *ht_file_md_head; // map of fileid -> file_md
	pthread_t as_thread; // allocation strategy thread
	unsigned int ssd_max_size;
	unsigned int hdd_max_size;
	int the_end;
} sshdd_t;

typedef struct SFILE {
	FILE *f;
	file_md_t *file_md;
} SFILE;

#endif /* SSHDD_HANDLE_H_ */
