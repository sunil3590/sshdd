#ifndef ALLOCATION_STRATEGY_H_
#define ALLOCATION_STRATEGY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include "uthash.h"
#include "maxHeap.h"
#include "minHeap.h"

// TODO : global is not a good idea
//Timestamp for LRU counter;
int timestamp = 0;

//File metadata strcuture
typedef struct file_metadata{
	char file_name[32]; //Assuming filename to contain less than 32 characters
    int located; //hdd or ssd
    int mfu_ctr; //MFU counter
    int lru_ctr; //LRU counter
    UT_hash_handle hh; /* makes this structure hashable */
} file_md;

//Declare the hash table structure to map file_name -> file_md structures
file_md *ht_file_md = NULL;
//Declare the maxHeap structure to rank files in HDD
maxHeap max_heap;
//Declare the minHeap structure to rank files in SSD
minHeap min_heap;

//Init function to start allocation strategy daemon
void * as_daemon_init(void *arg);

#endif /* ALLOCATION_STRATEGY_H_ */

//TODO:Write a makefile for the code
