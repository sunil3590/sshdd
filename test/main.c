/*
 * main.c
 *
 *  Created on: 03-Mar-2016
 *      Author: sunil3590
 */

#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "uthash.h"
#include "pqueue.h"
#include "file_md.h"
#include "sshdd.h"
#include "generate_files.h"
#include "profile.h"
#include "main.h"

void usage() {
	printf("usage: sshdd <generate / benchmark / profile / simple> <ssd_folder> <hdd_folder> <1 / 0>\n");
	printf("\tgenerate : generates the data files for further testing\n");
	printf("\tbenchmark : establish benchmark using default file functions\n");
	printf("\tprofile : profile sshdd file functions\n");
	printf("\tsimple : simple use case\n");
	printf("\tssd_folder : folder where SSD files are placed\n");
	printf("\thdd_folder : folder where HDD files are placed\n");
	printf("\t1 : real time simulation\n");
	printf("\t0 : as fast as possible\n");

	return;
}

// function to demonstrate simple use case of sshdd
void run_simple(char *ssd_folder, char *hdd_folder) {
	// initialize sshdd
	sshdd_conf_t conf;
	conf.optimize = 1;
	conf.ssd_folder = ssd_folder;
	conf.hdd_folder = hdd_folder;
	conf.ssd_max_size = 50 * 1024 * 1024;
	conf.hdd_max_size = 50 * 1024 * 1024;

	void *sshdd = NULL;
	sshdd = sshdd_init(&conf);
	if (sshdd == NULL) {
		printf("Failed to init sshdd.\n");
		return;
	}

	void *f = sshdd_fopen(sshdd, "1", "r");
	if (f == NULL) {
		printf("Failed to open file.\n");
		return;
	}

	if (sshdd_fclose(sshdd, f) != 0) {
		printf("Failed to open file.\n");
		return;
	}

	sshdd_terminate(sshdd);

	printf("Successfully tested simple use case.\n");
}

int main(int argc, char** argv) {

	if (argc != 4 && argc != 5) {
		usage();
		return -1;
	}

	if (strcmp(argv[1], "generate") == 0) {
		generate_files(argv[2], argv[3]);
	} else if (strcmp(argv[1], "simple") == 0) {
		run_simple(argv[2], argv[3]);
	} else if (argc == 5){
		int real_time = 0;
		if (strcmp(argv[4], "1") == 0)
			real_time = 1;
		if (strcmp(argv[1], "benchmark") == 0) {
			profile(DEFAULT_FUNCTIONS, argv[2], argv[3], real_time);
		} else if (strcmp(argv[1], "profile") == 0) {
			profile(SSHDD_FUNCTIONS, argv[2], argv[3], real_time);
		} else {
			usage();
			return -1;
		}
	} else {
		usage();
		return -1;
	}

	return 0;
}
