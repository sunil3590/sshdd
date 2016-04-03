#include "allocation_strategy.h"
#include "sshdd.h"
#include "maxHeap.h"
#include "minHeap.h"
#include "uthash.h"

//Init function to start allocation strategy daemon
void * as_daemon_init(void *arg){
	//Get the sshdd handle
	sshdd * sshdd_handle = (sshdd *)arg;

	//Create the hash table structure to map file_name -> file_md structures
	ht_file_md = NULL;
	//Create the maxHeap structure to rank files in HDD
	max_heap = initMaxHeap();
	//Create the minHeap structure to rank files in SSD
	min_heap = initMinHeap();

	//Iterate files in HDD
	DIR *FD = NULL;
	struct dirent *in_file;
	if( NULL == (FD = opendir(sshdd_handle->hdd_folder))){
		fprintf(stderr, "Failed to open hdd_folder:%s\n", sshdd_handle->hdd_folder);
		return NULL;
	}
	//Read all the file names in the hdd_folder
	while((in_file = readdir(FD))){
		//Ignore '.' and '..'
		if(!strcmp(in_file->d_name, "."))
			continue;
		if(!strcmp(in_file->d_name, ".."))
			continue;

		//Create file_md object for each file
		file_md *file_obj = (file_md *)malloc(sizeof(file_md));
		strcpy(file_obj->file_name, in_file->d_name);//Assuming in_file->d_name of size less than 16 bytes
		file_obj->located = 0; //hdd locator
		file_obj->mfu_ctr = 0;
		file_obj->lru_ctr = 0;

		// TODO : file_name -> fileid
		//Add the file_md object into the hash map
		HASH_ADD_STR(ht_file_md, file_name, file_obj);
		//Add the file_md object into the max heap for HDD
		//insertNodeMaxHeap(max_heap, file_obj);
	}

	//Iterate files in SSD
	FD = NULL;
	in_file = NULL;
	if( NULL == (FD = opendir(sshdd_handle->ssd_folder))){
		fprintf(stderr, "Failed to open ssd_folder:%s\n", sshdd_handle->ssd_folder);
		return NULL;
	}
	//Read all the file names in the ssd_folder
	while((in_file = readdir(FD))){
		//Ignore '.' and '..'
		if(!strcmp(in_file->d_name, "."))
			continue;
		if(!strcmp(in_file->d_name, ".."))
			continue;

		//Create file_md object for each file
		file_md *file_obj = (file_md *)malloc(sizeof(file_md));
		strcpy(file_obj->file_name, in_file->d_name);
		file_obj->located = 0; //hdd locator
		file_obj->mfu_ctr = 0;
		file_obj->lru_ctr = 0;

		//Add the file_md object into the hash map
		HASH_ADD_STR(ht_file_md, file_name, &file_obj);
		//Add the file_md object into the min heap for SSD
		//insertNodeMinHeap(min_heap, file_obj);
	}


	//Periodically move the files from HDD to SSD
	//based on MFU counter value
	while(1){
		//Lock all the file_md objects before heapify
		pthread_mutex_lock(&ht_lock);
		//Heapify max and min heaps to get the updated values

		//Unlock all the file_md objects
		pthread_mutex_unlock(&ht_lock);
		//Interchange the top object from both max and min heaps
		//based on difference between MFU counter values
		//compared to a fixed threshold criteria


		//Sleep for 1s
		sleep(1);
	}


}
