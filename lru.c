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

QNode* newQNode( int pageNumber )
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

/* Create new node, set the frame number, and add to front of queue
 * If the queue is empty, then set both front and rear;
 * otherwise, only set front
 */
void enqueue(int frameRef){
	printf("enque runs\n");
	//create new node with frame reference number
	QNode* new = newQNode(frameRef);
	
	//If this is the first frame to be referenced i.e. the queue is empty
	if (mainQ->isEmpty){

		mainQ->front = mainQ->rear = new;
		mainQ->isEmpty = 0;
		// printf("it was empty, it is now %d\n", mainQ->isEmpty);

	//Otherwise set new front
	}else{
		// printf("not empty runs\n");
		new->next = mainQ->front;
		mainQ->front->prev=new;
		mainQ->front=new;
	}

	contains[frameRef]=1;
	// printf("tail after enq is %d\n", mainQ->rear->frameNumber);

	return;
	printf("enque ends\n");
}

/* Removes the tail of the queue, free the memory, and return the
 * associated frameNumber
 */
int dequeue(){
	//if only one node, then change the front
	if(mainQ->front==mainQ->rear)
		mainQ->front=NULL;

	//set new node to hold the last element
	QNode* temp = mainQ->rear;
	int toRet=temp->frameNumber;

	//remove the last element from queue
	mainQ->rear = mainQ->rear->prev;
	mainQ->rear->next = NULL;
	
	free(temp);

	//Set contains to no
	contains[toRet]=0;
	
	return toRet;
}

QNode* find (int frameRef){
	QNode* temp = mainQ->front;
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
	printf("evict begins\n");

	//Ensure there is a frame to evict
	assert(mainQ->isEmpty==0);

	int frameToEvict = dequeue();
	printf("evict ends, evicted %d\n", frameToEvict);
	return frameToEvict;
}

void printq(){

	QNode* temp1 = mainQ->front;

	printf("The current queue is %d ", temp1->frameNumber);

	while(temp1->next!=NULL){
		temp1=temp1->next;
		printf("and %d ", temp1->frameNumber);
	}
	printf("\n");

	return;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int frameRef = p->frame >> PAGE_SHIFT;

	printf("ref runs, frame referenced is %d\n", frameRef);

	//If the frameNumber has never been referenced before
	if (!contains[frameRef]){

		//Create new node, and add to front of queue
		enqueue(frameRef);


	//Otherwise if the node is not at the front, then move it to the front
	}else if(mainQ->front->frameNumber!=frameRef){
		//Find the corresponding node
		QNode* temp = find(frameRef);

		//unlink from current position
		temp->prev->next = temp->next;
		if(temp->next)
			temp->next->prev=temp->prev;

		//If temp was the rear node, then change the rear as temp will be new front
		if (mainQ->rear == temp){
			mainQ->rear = temp->prev;
			mainQ->rear->next = NULL;
		}

		//Put the temp node at the front of the queue and perform book keeping
		temp->next = mainQ->front;
		temp->prev = NULL;
		mainQ->front->prev = temp;
		mainQ->front=temp;
	}
	printq();
	// printf("q head = %d and q tail = %d\n", mainQ->front->frameNumber, mainQ->rear->frameNumber);
	printf("ref ends\n\n");
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
