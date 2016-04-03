#ifndef MAX_HEAP_H_
#define MAX_HEAP_H_

#include "sshdd.h"
#include "allocation_strategy.h"

#define LCHILD(x) 2 * x + 1
#define RCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

//Max Heap structure
typedef struct maxHeap{
	int size;
	file_md *elem; //stores array of file_md pointers
}maxHeap;

//Function to initialize max heap
maxHeap initMaxHeap();

#endif /* MAX_HEAP_H_ */

//TODO:Write a makefile for the code
