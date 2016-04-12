#ifndef SSHDD_H_
#define SSHDD_H_

typedef struct sshdd_conf_t {
	int optimize;
	char *ssd_folder;
	const char *hdd_folder;
	unsigned int ssd_max_size;
	unsigned int hdd_max_size;
} sshdd_conf_t;

typedef struct sshdd_t {
	int optimize; // should the algorithm be run?
	int currently_open; // count of currentl
	char ssd_folder[256]; // TODO : hardcode
	char hdd_folder[256]; // TODO : hardcode
	file_md_t file_md[4096]; // array of meta data objects TODO : hardcode
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

void* sshdd_init(sshdd_conf_t *conf);

SFILE* sshdd_fopen(void *handle, const char *fileid, const char *mode);

size_t sshdd_fread(void *handle, void *ptr, size_t size, size_t nmemb, SFILE *fs);

size_t sshdd_fwrite(void *handle, const void *ptr, size_t size, size_t nmemb, SFILE *fs);

int sshdd_fclose(void *handle, SFILE *fs);

int sshdd_terminate(void* handle);

#endif /* SSHDD_H_ */
