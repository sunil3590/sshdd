#ifndef AS_H_
#define AS_H_

//File metadata strcuture
typedef struct file_metadata{
	char file_name[16]; //filename
    int  located; //hdd or ssd
    int mfu_ctr; //MFU counter
    int lru_ctr; //LRU counter
}file_md;

//Init function to start allocation strategy daemon
void * as_daemon_init(void *arg);


#endif /* AS_H_ */

//TODO:Write a makefile for the code
