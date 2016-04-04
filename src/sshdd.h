#ifndef SSHDD_H_
#define SSHDD_H_

typedef struct sshdd_t {
	int optimize; // should the algorithm be run?
	int currently_open; // count of currentl
	char ssd_folder[256]; // TODO : hardcode
	char hdd_folder[256]; // TODO : hardcode
	file_md_t file_md[4096]; // array of meta data objects TODO : hardcode
	file_md_t *ht_file_md_head; // map of fileid -> file_md
	pqueue_t *ssd_pq; // priority queue of SSD metadata objects
	pqueue_t *hdd_pq; // priority queue of HDD metadata objects
	pthread_t as_thread; // allocation strategy thread
} sshdd_t;

void* sshdd_init(const int optimize, const char *ssd_folder, const char *hdd_folder);

FILE* sshdd_fopen(void *handle, const char *fileid, const char *mode);

size_t sshdd_fread(void *handle, void *ptr, size_t size, size_t nmemb, FILE *stream);

size_t sshdd_fwrite(void *handle, const void *ptr, size_t size, size_t nmemb, FILE *stream);

int sshdd_fclose(void *handle, FILE *stream);

int sshdd_terminate(void* handle);

#endif /* SSHDD_H_ */
