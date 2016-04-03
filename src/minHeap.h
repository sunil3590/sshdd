#ifndef MIN_HEAP_H_
#define MIN_HEAP_H_

#include "sshdd.h"
#include "allocation_strategy.h"

#define LCHILD(x) 2 * x + 1
#define RCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

//Min Heap structure
typedef struct minHeap{
	int size;
	file_md *elem;
}minHeap;

//Function to initialize min heap
minHeap initMinHeap();

#endif /* MIN_HEAP_H_ */

//TODO:Write a makefile for the code
