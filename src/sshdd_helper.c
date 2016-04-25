/*
 * sshdd_helper.c
 *
 *  Created on: 03-Apr-2016
 *      Author: sunil3590
 */

#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <mqueue.h>
#include <errno.h>

#include "constants.h"
#include "uthash.h"
#include "file_md.h"
#include "pqueue.h"
#include "sshdd.h"
#include "sshdd_handle.h"

int build_metadata_for_folder(const char *folder, file_loc loc,
		file_md_t *file_md, file_md_t **ht_head) {

	// Read all the files in folder
	DIR *dir = NULL;
	struct dirent *in_file;
	if (NULL == (dir = opendir(folder))) {
		printf("Failed to open folder : %s for building metadata\n",
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
		snprintf(cur_file_md->fileid, FNAME_SMALL_SIZE, "%s", in_file->d_name);
		cur_file_md->loc = loc;
		cur_file_md->mfu_ctr = 0;
		// cur_file_md->lru_ctr = 0;
		cur_file_md->ref_count = 0;

		// add fileid -> file_md mapping
		HASH_ADD_STR(ht_file_md, fileid, cur_file_md);

		i++;
	}

	*ht_head = ht_file_md;

	return i;
}

int send_msg(void *file_md_ptr, mqd_t mq) {
	pri_update_msg msg;
	msg.file_md_ptr = file_md_ptr;
	int status = mq_send(mq, (char *)&msg, MSG_SIZE, 0);
	// ignore the error caused due to no space in queue
	if (status != 0 && errno != 11) {
		printf("Error in sending message %d:%s\n", errno, strerror(errno));
	}
	return status;
}
