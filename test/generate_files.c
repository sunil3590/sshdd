/*
 * generate_files.c
 *
 *  Created on: 01-Mar-2016
 *      Author: sunil3590
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"
#include "definitions.h"
#include "endian.h"
#include "request.h"

int generate_files() {

	// process log
	struct request BER, LER, *R;

	// metrics
	int file_count = 0;
	int append_count = 0;
	int log_count = 0;
	unsigned int bytes_written = 0;

	fprintf(stderr, "Reading Access Log\n");
	FILE *log_file = fopen(LOG_FILE, "rb");

	/* read the initial request */
	if ((fread(&BER, sizeof(struct request), 1, log_file)) != 1) {
		fprintf(stderr, "Error: Failed to read initial request!\n");
		return -1;
	}

	while (!feof(log_file)) {
		/* status indicator */
		log_count++;
		if (log_count % 100000 == 0)
			fprintf(stderr, "Logs analyzed : %d\n", log_count);

		LittleEndianRequest(&BER, &LER);
		R = &LER;

		/* determine http method and status */
		int method = (int) R->method;
		int status = ((int) R->status) & 0x3f;
		int size = R->size;

		if (status == SC_200 && method == GET
				&& size != NO_SIZE && size > 0) {

			// file names
			char ssd_fname[128];
			char hdd_fname[128];
			char* fname = NULL;
			sprintf(ssd_fname, "%s%d", SSD_FOLDER, R->objectID);
			sprintf(hdd_fname, "%s%d", HDD_FOLDER, R->objectID);

			// check if file already exists and get the size of existing file
			struct stat ssd_st;
			struct stat hdd_st;
			int old_size = 0;
			int ssd_val = stat(ssd_fname, &ssd_st);
			int hdd_val = stat(hdd_fname, &hdd_st);
			if (ssd_val == 0) {
				old_size = ssd_st.st_size;
				fname = ssd_fname;
			} else if (hdd_val == 0) {
				old_size = hdd_st.st_size;
				fname = hdd_fname;
			} else {
				// file does not exist
				file_count++;
				if (file_count % 100 == 0)
					fprintf(stderr, "File count : %d\n", file_count);

				old_size = 0;
				if (bytes_written < SSD_SIZE) {
					fname = ssd_fname;
				} else {
					fname = hdd_fname;
				}
			}

			int extra_size = size - old_size;
			// stick to the older file size if it is larger
			if (extra_size < 0) {
				extra_size = 0;
			}

			// if any more data to be written
			if (extra_size > 0) {

				FILE* f = fopen(fname, "a");
				if (f == NULL) {
					fprintf(stderr, "File open error : %s\n", fname);
					continue;
				}

				// write data
				char* data = malloc(extra_size);
				char pattern[10];
				sprintf(pattern, "%d", R->objectID % 10);
				memset(data, pattern[0], extra_size);
				int written = fwrite(data, 1, extra_size, f);
				if (extra_size != written) {
					fprintf(stderr, "File write error : %s\n", fname);
				}
				free(data);

				// flush and close file
				fflush(f);
				fsync(fileno(f));
				fclose(f);

				// metrics
				append_count++;
				bytes_written += extra_size;
				if (append_count % 100 == 0)
					fprintf(stderr, "File appends : %d\n", append_count);
			}
		}

		/* read the next request */
		fread(&BER, sizeof(struct request), 1, log_file);
	}

	fclose(log_file);

	/* final count */
	fprintf(stderr, "******************************\n");
	fprintf(stderr, "Total logs : %d\n", log_count);
	fprintf(stderr, "Total files : %d\n", file_count);
	fprintf(stderr, "Total appends : %d\n", append_count);
	fprintf(stderr, "Total bytes : %u\n", bytes_written);
	fprintf(stderr, "******************************\n");

	return 0;
}
