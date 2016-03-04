/*
 * main.c
 *
 *  Created on: 03-Mar-2016
 *      Author: sunil3590
 */

#include <stdio.h>
#include <string.h>

#include "generate_files.h"
#include "profile.h"

void usage() {
	fprintf(stderr, "usage: sshdd <generate / benchmark / profile>\n");
	fprintf(stderr, "\tgenerate : generates the data files for further testing\n");
	fprintf(stderr, "\tbenchmark : establish benchmark using default file functions\n");
	fprintf(stderr, "\tprofile : profile sshdd file functions\n");

	return;
}

int main(int argc, char** argv) {

	if (argc < 2) {
		usage();
		return -1;
	}

	if (strcmp(argv[1], "generate") == 0) {
		generate_files();
	} else if (strcmp(argv[1], "benchmark") == 0) {
		profile(DEFAULT_FUNCTIONS);
	} else if (strcmp(argv[1], "profile") == 0) {
		profile(SSHDD_FUNCTIONS);
	} else {
		usage();
		return -1;
	}

	return 0;
}
