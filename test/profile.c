/*
 * profile.c
 *
 *  Created on: 01-Mar-2016
 *      Author: sunil3590
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"
#include "main.h"
#include "uthash.h"
#include "pqueue.h"
#include "file_md.h"
#include "sshdd.h"
#include "definitions.h"
#include "endian.h"
#include "request.h"
#include "profile.h"

int profile(profile_what what, char *ssd_folder, char *hdd_folder, int real_time) {

	// initialize sshdd
	sshdd_conf_t conf;
	conf.ssd_folder = ssd_folder;
	conf.hdd_folder = hdd_folder;
	conf.ssd_max_size = SSD_SIZE;
	conf.hdd_max_size = HDD_SIZE;

	void *sshdd = NULL;

	if (what == DEFAULT_FUNCTIONS) {
		conf.optimize = 0;
		sshdd = sshdd_init(&conf);
	} else if (what == SSHDD_FUNCTIONS) {
		conf.optimize = 1;
		sshdd = sshdd_init(&conf);
	} else {
		printf("Error: Invalid argument\n");
		return -1;
	}

	if (sshdd == NULL) {
		printf("Error: Failed to initialize sshdd\n");
		return -1;
	}

	// prepare to run profiler
	struct request BER, LER, *R;
	int log_count = 0;
	int file_access_count = 0;
	unsigned long bytes_read = 0;
	double time_taken = 0;

	printf("Reading Access Log\n");
	FILE *log_file = fopen(LOG_FILE, "rb");

	/* read the initial request */
	if ((fread(&BER, sizeof(struct request), 1, log_file)) != 1) {
		printf("Error: Failed to read initial request!\n");
		return -1;
	}
	LittleEndianRequest(&BER, &LER);
	R = &LER;

	// remember the first time stamp
	time_t run_start = time(NULL);
	int sim_start = R->timestamp;

	// allocate some memory to hold data read from file
	char* data = malloc(10 * 1024 * 1024);
	char* pattern = malloc(10 * 1024 * 1024);

	while (!feof(log_file)) {
		/* status indicator */
		log_count++;
		if (log_count % 100000 == 0)
			printf("Logs analyzed : %d\n", log_count);

		LittleEndianRequest(&BER, &LER);
		R = &LER;

		/* determine http method and status */
		int method = (int) R->method;
		int status = ((int) R->status) & 0x3f;
		int size = R->size;

		if (status == SC_200 && method == GET
				&& size != NO_SIZE && size > 0) {

			// simulate real time access
			if (real_time == 1) {
				time_t run_cur = time(NULL);
				time_t run_time = run_cur - run_start;
				int sim_prog = R->timestamp - sim_start;
				int delay = sim_prog - run_time;
				if (delay > 0) {
					sleep(delay);
				}
			}

			// prepare file name and expected data
			char fileid[128];
			sprintf(fileid, "%d", R->objectID);
			memset(pattern, '0' + (R->objectID % 10), size);

			// open, read and close -  file
			clock_t start = clock() ;
			void* f = sshdd_fopen(sshdd, fileid, "r");
			if (f == NULL) {
				printf("File open error : %s\n", fileid);
				continue;
			}
			int read = sshdd_fread(sshdd, data, 1, size, f);
			sshdd_fclose(sshdd, f);
			clock_t end = clock();
			time_taken += (end - start);

			// check validity of data
			if (size != read) {
				printf("File size error : %s. Expected %d, got %d\n", fileid, size, read);
			}
			if ((memcmp(data, pattern, size) != 0)) {
				printf("File content error : %s\n", fileid);
			}

			// metrics
			bytes_read += size;
			file_access_count++;
			if (file_access_count % 100000 == 0)
				printf("Files accessed : %d\n", file_access_count);
		}

		/* read the next request */
		fread(&BER, sizeof(struct request), 1, log_file);
	}

	free(data);
	free(pattern);
	fclose(log_file);

	/* final count */
	printf("Total logs analyzed : %d\n", log_count);
	printf("Total file access : %d\n", file_access_count);
	printf("Total bytes read : %lu\n", bytes_read);
	printf("Total time taken : %f sec\n", time_taken  / (double)CLOCKS_PER_SEC);

	return 0;
}
