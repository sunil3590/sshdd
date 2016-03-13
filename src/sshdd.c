#include "sshdd.h"
#include "allocation_strategy.h"

void* sshdd_init(const int optimize, const char *ssd_folder,
		const char *hdd_folder) {
	sshdd* sshdd = malloc(sizeof(struct sshdd));

	// initialize sshdd
	sshdd->currently_open = 0;
	sshdd->optimize = optimize;
	strcpy(sshdd->ssd_folder, ssd_folder);
	strcpy(sshdd->hdd_folder, hdd_folder);

	//Create the allocation strategy daemon thread if optimize set to 1
	if(sshdd->optimize){
		//create the thread
		pthread_t as_daemon;
		pthread_create(&as_daemon, NULL, &as_daemon_init, (void *)&sshdd);
	}

	return sshdd;
}

FILE* sshdd_fopen(void *handle, const char *fileid, const char *mode) {
	// TODO
	// update book marking each time a file is opened, this will help the algo
	// cannot move a file which is open, so book mark when a file is closed
	// cannot open a file which is currently getting moved

	sshdd* sshdd = handle;

	sshdd->currently_open++;

	// TODO : remove this trial and error method
	// map file id to actual file path
	char filename[512];
	sprintf(filename, "%s%s", sshdd->hdd_folder, fileid);
	if (access(filename, F_OK) != 0) {
		// file exists in ssd
		sprintf(filename, "%s%s", sshdd->ssd_folder, fileid);
	}

	return fopen(filename, mode);
}

int sshdd_fclose(void *handle, FILE *stream) {
	// TODO
	// cannot move a file which is open, so bookmark when a file is closed

	sshdd* sshdd = handle;

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

	sshdd* sshdd = handle;

	// don't terminate if some file is still open
	if (sshdd->currently_open != 0) {
		fprintf(stderr, "Cannot terminate, files still open\n");
	}

	free(sshdd);

	return 0;
}
