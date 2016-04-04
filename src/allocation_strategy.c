#include <unistd.h>
#include <stdio.h>

#include "allocation_strategy.h"
#include "uthash.h"
#include "pqueue.h"
#include "file_md.h"
#include "sshdd.h"

void * allocation_strategy(void *sshdd_handle) {
	// Get the sshdd handle
	sshdd_t *sshdd = (sshdd_t *) sshdd_handle;

	// Periodically move the files from HDD to SSD
	// based on priority queues
	while (1) {
		// Sleep for 1s
		sleep(1);
	}
}
