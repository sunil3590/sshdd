#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "allocation_strategy.h"
#include "sshdd.h"
#include "maxHeap.h"
#include "minHeap.h"

//Init function to start allocation strategy daemon
void * as_daemon_init(void *arg){
	//Get the sshdd handle
	sshdd * sshdd_handle = (sshdd *)arg;

	//Create the hash table structure to map file_name -> file_md structures
	file_md *ht_file_md = NULL;
	//Create the maxHeap structure to rank files in HDD
	maxHeap max_heap = initMaxHeap();
	//Create the minHeap structure to rank files in SSD
	minHeap min_heap = initMinHeap();

	//Iterate files in HDD
	//

}
