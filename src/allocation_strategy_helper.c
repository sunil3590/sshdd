#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include "uthash.h"
#include "pqueue.h"
#include "file_md.h"
#include "sshdd.h"
#include "allocation_strategy_helper.h"

int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr)
{
	return (next < curr);
}

int cmp_pri_max(pqueue_pri_t next, pqueue_pri_t curr)
{
	return (next < curr);
}

int cmp_pri_min(pqueue_pri_t next, pqueue_pri_t curr)
{
	return (next > curr);
}

pqueue_pri_t get_pri(void *a)
{
	file_md_t *file_md_ptr = ((as_node_t *)a)->file_md_ptr;
	return file_md_ptr->mfu_ctr;
}


void set_pri(void *a, pqueue_pri_t pri)
{
	file_md_t *file_md_ptr = ((as_node_t *)a)->file_md_ptr;
	file_md_ptr->mfu_ctr = pri;
}

//TODO: Remove the pos functions
size_t get_pos(void *a)
{
	return ((as_node_t *) a)->pos;
}


void set_pos(void *a, size_t pos)
{
	((as_node_t *) a)->pos = pos;
}


void printq_elem(FILE *out, void *elem){
	file_md_t *file_md_ptr = ((as_node_t *)elem)->file_md_ptr;
	printf("pri=%d [loc=%d]\n", file_md_ptr->mfu_ctr, file_md_ptr->loc);
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
		size += get_file_size(fname);
	}

	return size;
}

int get_file_size(char *fname) {
	FILE *f = fopen(fname, "r");
	struct stat buf;
	fstat(fileno(f), &buf);
	fclose(f);
	return buf.st_size;
}
