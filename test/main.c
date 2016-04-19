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
	fprintf(stderr, "usage: sshdd <generate / benchmark / profile / simple> <1 / 0>\n");
	fprintf(stderr, "\tgenerate : generates the data files for further testing\n");
	fprintf(stderr, "\tbenchmark : establish benchmark using default file functions\n");
	fprintf(stderr, "\tprofile : profile sshdd file functions\n");
	fprintf(stderr, "\tsimple : simple use case\n");
	fprintf(stderr, "\t1 : real time simulation\n");
	fprintf(stderr, "\t0 : as fast as possible\n");

	return;
}

// function to demonstrate simple use case of sshdd
void run_simple() {
	// initialize sshdd
	sshdd_conf_t conf;
	conf.optimize = 1;
	conf.ssd_folder = SSD_FOLDER;
	conf.hdd_folder = HDD_FOLDER;
	// TODO : test case where
	// 1. SSD is full
	// 2. SSD is partially filled
	conf.ssd_max_size = 50 * 1024 * 1024;
	conf.hdd_max_size = 50 * 1024 * 1024;

	void *sshdd = NULL;
	sshdd = sshdd_init(&conf);
	if (sshdd == NULL) {
		fprintf(stderr, "Failed to init sshdd.\n");
		return;
	}

	void *f = sshdd_fopen(sshdd, "1", "r");
	if (f == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return;
	}

	if (sshdd_fclose(sshdd, f) != 0) {
		fprintf(stderr, "Failed to open file.\n");
		return;
	}

	sshdd_terminate(sshdd);

	fprintf(stderr, "Successfully tested simple use case.\n");
}

int main(int argc, char** argv) {

	if (argc < 2) {
		usage();
		return -1;
	}

	if (strcmp(argv[1], "generate") == 0) {
		generate_files();
	} else if (strcmp(argv[1], "simple") == 0) {
		run_simple();
	} else {
		int real_time = 0;
		if (strcmp(argv[2], "1") == 0)
			real_time = 1;
		if (strcmp(argv[1], "benchmark") == 0) {
			profile(DEFAULT_FUNCTIONS, real_time);
		} else if (strcmp(argv[1], "profile") == 0) {
			profile(SSHDD_FUNCTIONS, real_time);
		} else {
			usage();
			return -1;
		}
	}

	return 0;
}
