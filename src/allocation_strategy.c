#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "uthash.h"
#include "sshdd.h"

//Init function to start allocation strategy daemon
void * as_daemon_init(void *arg){
	//Get the sshdd handle
	sshdd * sshdd_handle = (sshdd *)arg;

	//Create the hash table structure to map file_name -> file_md structures


}
