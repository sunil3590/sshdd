/*
 * sshdd_helper.c
 *
 *  Created on: 03-Apr-2016
 *      Author: sunil3590
 */

#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

#include "uthash.h"
#include "file_md.h"
#include "pqueue.h"
#include "sshdd.h"

int build_metadata_for_folder(const char *folder, file_loc loc,
		file_md_t *file_md, file_md_t **ht_head) {

	// Read all the files in folder
	DIR *dir = NULL;
	struct dirent *in_file;
	if (NULL == (dir = opendir(folder))) {
		fprintf(stderr, "Failed to open folder : %s for building metadata\n",
				folder);
		return 0;
	}

	int i = 0;
	file_md_t *ht_file_md = *ht_head;
	while ((in_file = readdir(dir))) {
		//Ignore '.' and '..'
		if (!strcmp(in_file->d_name, "."))
			continue;
		if (!strcmp(in_file->d_name, ".."))
			continue;
		if (!strcmp(in_file->d_name, ".gitignore"))
			continue;

		file_md_t *cur_file_md = &file_md[i];
		// TODO Assuming size less than 16 bytes
		strcpy(cur_file_md->fileid, in_file->d_name);
		cur_file_md->loc = loc;
		cur_file_md->mfu_ctr = 0;
		cur_file_md->lru_ctr = 0;
		cur_file_md->is_open = 0;

		// add fileid -> file_md mapping
		HASH_ADD_STR(ht_file_md, fileid, cur_file_md);

		i++;
	}

	*ht_head = ht_file_md;

	return i;
}

int get_folder_size(char *folder) {
	// Read all the files in folder
	DIR *dir = NULL;
	struct dirent *in_file;
	if (NULL == (dir = opendir(folder))) {
		fprintf(stderr, "Failed to open folder : %s for building metadata\n",
				folder);
		return 0;
	}

	int size = 0;

	while ((in_file = readdir(dir))) {
		//Ignore '.' and '..'
		if (!strcmp(in_file->d_name, "."))
			continue;
		if (!strcmp(in_file->d_name, ".."))
			continue;
		if (!strcmp(in_file->d_name, ".gitignore"))
			continue;
		char fname[256] = {0};
		strcat(fname, folder);
		strcat(fname, in_file->d_name);
		struct stat buf;
		fstat(fileno(fopen(fname, "r")), &buf);
		size += buf.st_size;
	}

	printf("%d", size);
	return size;
}
