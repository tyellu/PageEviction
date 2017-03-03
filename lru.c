#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

//Queue node structure
typedef struct QNode_t
{
    struct QNode *next, *prev;
    int frameNumber;  // the page number stored in this QNode
} Qnode;

typedef struct Queue_t
{
    QNode *front, *rear;
    bool isEmpty;
} Queue;

//To record if the main Queue contains the frame
bool *contains;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	//Ensure there is a frame to evict
	assert(!mainQ->isEmpty);

	//Otherwise grab frame number from the front of the queue
	int frameToEvict = mainQ->front->frame;

	//Set the front of the queue to the next node
	mainQ->front = mainQ->front->next;

	//Delete the previous of the new front
	mainQ->front->prev = NULL;

	//If the queue is now empty, set indicator
	if (mainQ->front == Null){
		mainQ->isEmpty = true;
	}

	//The queue now doesn't contain frameToEvict, set indicator
	contains[frameToEvict]=0;

	return frameToEvict;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	//If the frame is contained our queue
	if (contains[p->frame]){
		//Set a 
		Qnode *temp = mainQ->front;

		while(temp->frame != p->frame){
			temp
		}
	}


	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	//Create the main queue
	Queue *mainQ;
	mainQ->front = NULL;
	mainQ->rear = NULL;
	mainQ->isEmpty = true;
}
