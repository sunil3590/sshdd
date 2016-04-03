#ifndef SSHDD_H_
#define SSHDD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// TODO : global
extern int timestamp;
pthread_mutex_t ht_lock; //lock for all the file_md objects

typedef struct sshdd {
	// TODO : add any state information here
	// statistics variables
	// book keeping variables
	// file ID to actual path mapping

	int optimize; // should the algorithm be run?
	int currently_open;
	char ssd_folder[256];
	char hdd_folder[256];
} sshdd;
sshdd* sshdd_handle = NULL;

void* sshdd_init(const int optimize, const char *ssd_folder, const char *hdd_folder);

FILE* sshdd_fopen(void *handle, const char *fileid, const char *mode);

size_t sshdd_fread(void *handle, void *ptr, size_t size, size_t nmemb, FILE *stream);

size_t sshdd_fwrite(void *handle, const void *ptr, size_t size, size_t nmemb, FILE *stream);

int sshdd_fclose(void *handle, FILE *stream);

int sshdd_terminate(void* handle);

// TODO : there are more file related functions as part of stdio
// analyze which functions need to be wrapped and which need not be
// http://www.tutorialspoint.com/c_standard_library/stdio_h.htm

#endif /* SSHDD_H_ */
