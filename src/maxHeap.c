#include <maxHeap.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


//Function to initialize max priority queue
maxHeap initMaxHeap(){
	maxHeap pq;
	pq.size = 0;
	return pq;
}

//Function to insert new file into the max pq
void insertNodeMaxHeap(maxHeap *pq, file_md elem) {
    // allocating space
    if(pq->size) {
        pq->elem = realloc(pq->elem, (pq->size + 1) * sizeof(file_md)) ;
    } else {
        pq->elem = malloc(sizeof(file_md)) ;
    }

    // Positioning the node at the right position in the max heap
    int i = (pq->size)++ ;
    int mfu_ctr = elem.mfu_ctr; //Get the key of the new element
    while(i && (mfu_ctr > pq->elem[PARENT(i)].mfu_ctr)) {
        pq->elem[i] = pq->elem[PARENT(i)] ;
        i = PARENT(i) ;
    }
    pq->elem[i] = elem;
}

//Swap function
void swap(file_md *n1, file_md *n2) {
	file_md temp = *n1 ;
    *n1 = *n2 ;
    *n2 = temp ;
}

//Heapify function
void heapifyMaxHeap(maxHeap *hp, int i) {
    int largest = (LCHILD(i) < hp->size &&
    		hp->elem[LCHILD(i)].mfu_ctr > hp->elem[i].mfu_ctr) ? LCHILD(i) : i ;

    if(RCHILD(i) < hp->size && hp->elem[RCHILD(i)].mfu_ctr > hp->elem[largest].mfu_ctr) {
        largest = RCHILD(i) ;
    }
    if(largest != i) {
        swap(&(hp->elem[i]), &(hp->elem[largest])) ;
        heapifyMaxHeap(hp, largest) ;
    }
}

//Delete function
void deleteNode(maxHeap *hp) {
    if(hp->size) {
        printf("Deleting node %s\n\n", hp->elem[0].file_name) ;
        hp->elem[0] = hp->elem[--(hp->size)] ;
        hp->elem = realloc(hp->elem, hp->size * sizeof(file_md)) ;
        heapifyMaxHeap(hp, 0) ;
    } else {
        printf("\nMax Heap is empty!\n") ;
        free(hp->elem) ;
    }
}


//Build Max heap
void buildMaxHeap(maxHeap *hp, file_md *arr, int size) {
    int i ;

    // Insertion into the heap without violating the shape property
    for(i = 0; i < size; i++) {
        if(hp->size) {
            hp->elem = realloc(hp->elem, (hp->size + 1) * sizeof(file_md *)) ;
        } else {
            hp->elem = malloc(sizeof(file_md *)) ;
        }
        //node nd ;
        //nd.data = arr[i] ;
        hp->elem[(hp->size)++] = arr[i] ;
    }

    // Making sure that heap property is also satisfied
    for(i = (hp->size - 1) / 2; i >= 0; i--) {
        heapifyMaxHeap(hp, i) ;
    }
}

