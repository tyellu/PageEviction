#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

//Queue node structure
typedef struct QNode
{
    struct QNode *prev, *next;
    int frameNumber;  // the page number stored in this QNode
} QNode;

typedef struct Queue
{
    struct QNode *front, *rear;
    bool isEmpty;
} Queue;

Queue *mainQ;
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
	int frameToEvict = mainQ->front->frameNumber;

	//Set the front of the queue to the next node
	mainQ->front = mainQ->front->next;

	//If the queue is now empty
	if (mainQ->front == NULL){
		//Set indicator
		mainQ->isEmpty = 1;
		//Set rear to null as well
		mainQ->rear = NULL;
	//Other wise it's not empty
	}else{
		//Delete the previous of the new front
		mainQ->front->prev = NULL;
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
		//Set a temprary node to find the frame node
		QNode *temp = mainQ->front;

		//Search for the frame node
		while(temp->frameNumber != p->frame){
			temp = temp->next;
		}
		//If there are more than 1 node in the queue, proceed, otherwise do nothing
		if (temp != mainQ->front){
			//If the found node is the first one
			if (temp == mainQ->front){

					//Remove the head and point head to next node, bookkeeping
					mainQ->front = temp->next;
					mainQ->front->prev = NULL;

					//Move the node to the end of the queue and bookpeeking
					temp->prev = mainQ->rear;
					temp->next = NULL;
					mainQ->rear->next = temp;
					mainQ->rear = temp;

			//If the found node is rear, we dont do anything, otherwise:
			} else if (!(temp == mainQ->rear)){
				//Unlink the node
				temp->prev->next=temp->next;
				temp->next->prev = temp->prev;

				//Add the node to the end of the queue and perform book keeping
				temp->prev = mainQ->rear;
				temp->next = NULL;
				mainQ->rear->next = temp;
				mainQ->rear = temp;
			}
		}

	//If the frame isn't in our queue, that means it was never accessed before
	} else{
		//create new node
		QNode *new = (QNode*)malloc(sizeof(QNode*));
		new->frameNumber = p->frame;

		//link it to the end of the queue
		new->prev = mainQ->rear;
		mainQ->rear = new;
		
		//Add it to our contained queue
		contains[p->frame] = 1;
	}
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	//allocate the main queue and perform bookkeeping
	mainQ = (Queue*)malloc(sizeof(QNode)*memsize);

	mainQ->front = NULL;
	mainQ->rear = NULL;
	mainQ->isEmpty = 1;

	//Allocate the contains array and wipe everything to 0
	contains = malloc(sizeof(bool)*memsize);
	memset(contains, 0, sizeof(bool) * memsize);
}
