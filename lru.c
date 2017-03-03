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

Queue* createQueue(){
	Queue* queue = (Queue *)malloc( sizeof( Queue ) );
	queue->front=queue->rear=NULL;
	queue->isEmpty=1;
	return queue;
}

QNode* newQNode( unsigned pageNumber )
{
    // Allocate memory and assign 'pageNumber'
    QNode* temp = (QNode *)malloc( sizeof( QNode ) );
    temp->frameNumber = pageNumber;
 
    // Initialize prev and next as NULL
    temp->prev = temp->next = NULL;
 
    return temp;
}

Queue* mainQ;

//To record if the main Queue contains the frame
bool *contains;

void enqueue(Queue* queue, int frameRef){

	//create new node with frame reference number
	QNode* new = newQNode(frameRef);
	new->next = queue->front;

	//If this is the first frame to be referenced i.e. the queue is empty
	if (queue->isEmpty){

		queue->front = queue->rear = new;
		queue->isEmpty = 0;

	//Otherwise set new front
	}else{
		queue->front->prev=new;
		queue->front=new;
	}

	contains[frameRef]=1;
}

int dequeue(Queue* queue){
	if(queue->front==queue->rear)
		queue->front=NULL;

	QNode* temp = queue->rear;
	queue->rear = queue->rear->prev;

	int toRet=temp->frameNumber;
	free(temp);
	return toRet;

}

QNode* find (Queue* queue, int frameRef){
	QNode* temp = queue->front;
	while (temp->frameNumber!=frameRef){
		temp = temp->next;
	}
	return temp;
}



/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	//Ensure there is a frame to evict
	assert(mainQ!=NULL);
	assert(!mainQ->isEmpty);

	int frameToEvict = dequeue(mainQ);

	return frameToEvict;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	printf("ref runs\n");
	int frameRef = p->frame >> PAGE_SHIFT;

	if (!contains[frameRef]){
		enqueue(mainQ, frameRef);
	}else if(mainQ->front->frameNumber!=frameRef){
		QNode* temp = find(mainQ, frameRef);

		//remove from rear position
		temp->prev->next = temp->next;
		if(temp->next)
			temp->prev->next=temp->next;

		//If temp was the rear node
		if (mainQ->rear == temp){
			mainQ->rear = temp->prev;
			mainQ->rear->next = NULL;
		}

		temp->next = mainQ->front;
		temp->prev = NULL;
		mainQ->front->prev = temp;
		mainQ->front=temp;
	}

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	//allocate the main queue and perform bookkeeping
	mainQ = createQueue();

	//Allocate the contains array and wipe everything to 0
	contains = malloc(sizeof(bool)*memsize);
	memset(contains, 0, sizeof(bool) * memsize);
}
