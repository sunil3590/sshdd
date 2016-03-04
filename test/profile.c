/*
 * profile.c
 *
 *  Created on: 01-Mar-2016
 *      Author: sunil3590
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "sshdd.h"
#include "definitions.h"
#include "endian.h"
#include "request.h"
#include "profile.h"

int profile(profile_what what) {

	// initialize sshdd
	sshdd* sshdd = NULL;

	if (what == DEFAULT_FUNCTIONS) {
		sshdd = sshdd_init(0, SSD_FOLDER, HDD_FOLDER);
	} else if (what == SSHDD_FUNCTIONS) {
		sshdd = sshdd_init(1, SSD_FOLDER, HDD_FOLDER);
	} else {
		fprintf(stderr, "Error: Invalid argument\n");
		return -1;
	}

	if (sshdd == NULL) {
		fprintf(stderr, "Error: Failed to initialize sshdd\n");
		return -1;
	}

	// prepare to run profiler
	struct request BER, LER, *R;
	int count = 0;
	unsigned int bytes_read = 0;
	double time_taken = 0;

	fprintf(stderr, "Reading Access Log\n");
	FILE *log_file = fopen(LOG_FILE, "rb");

	/* read the initial request */
	if ((fread(&BER, sizeof(struct request), 1, log_file)) != 1) {
		fprintf(stderr, "Error: Failed to read initial request!\n");
		return -1;
	}

	while (!feof(log_file)) {
		/* status indicator */
		count++;
		if (count % 100000 == 0)
			fprintf(stderr, "Logs analyzed : %d\n", count);

		LittleEndianRequest(&BER, &LER);
		R = &LER;

		/* determine http method and status */
		int method = (int) R->method;
		int status = ((int) R->status) & 0x3f;
		int size = R->size;

		if (status == SC_200 && method == GET
				&& size != NO_SIZE && size > 0) {

			// open file
			char fileid[128];
			sprintf(fileid, "%d", R->objectID);
			FILE* f = sshdd_fopen(sshdd, fileid, "r");
			if (f == NULL) {
				fprintf(stderr, "File open error : %s\n", fileid);
				continue;
			}

			// read data
			char* data = malloc(size);
			char* pattern = malloc(size);
			memset(pattern, '0' + (R->objectID % 10), size);

			clock_t start = clock() ;
			int read = sshdd_fread(sshdd, data, 1, size, f);
			clock_t end = clock() ;
			time_taken += (end - start);

			if (size != read) {
				fprintf(stderr, "File size error : %s. Expected %d, got %d\n", fileid, size, read);
			}
			if ((memcmp(data, pattern, size) != 0)) {
				fprintf(stderr, "File content error : %s\n", fileid);
			}

			// close file
			sshdd_fclose(sshdd, f);

			// metrics
			count++;
			bytes_read += size;
		}

		/* read the next request */
		fread(&BER, sizeof(struct request), 1, log_file);
	}

	fclose(log_file);

	/* final count */
	fprintf(stderr, "Total time taken : %f sec\n", time_taken  / (double)CLOCKS_PER_SEC);
	fprintf(stderr, "Total file access : %d\n", count);
	fprintf(stderr, "Total bytes read : %u\n", bytes_read);

	return 0;
}
