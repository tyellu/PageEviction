#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#define MAXLINE 256
extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile;

addr_t *instrn;
addr_t *memory;
int instrn_size;
int instrn_num;


/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int max_distance = 0;
	int frame_index = 0;
	int i;
	//loop throught the memory to get the frames in memory
	for(i=0; i < memsize; i++){
		addr_t curr_frame_addr = memory[i];
		int temp_distance = 0;
		int found = 0;
		int j;
		//loop through the tracefile to find next occurrence
		//of the current frame
		for(j = instrn_num; j < (instrn_size); j++){
			temp_distance++;
			if(instrn[j] == curr_frame_addr){
				if(max_distance < temp_distance){
					frame_index = i;
					max_distance = temp_distance;
					found = 1;
					break;
				}else{
					found = 1;
					break;
				}
			}
		}

		if(found == 0){
			frame_index = i;
			break;
		}
	}
	return frame_index;
}


/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	//update the memory to the current state and increment the instructon num
	int curr_frame = p->frame >> PAGE_SHIFT; 

	memory[curr_frame] = instrn[instrn_num];

	instrn_num = (instrn_num + 1);

	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	FILE *tfp;
	addr_t vaddr;
	char type;
	char buf[MAXLINE];
	
	//open the tracefile and check successfull access
	if(tracefile != NULL) {
		if((tfp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile in opt_init:");
			exit(1);
		}
	}

	instrn_size = 0;
	//goes throught the tracefile to find the length
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			instrn_size++;
		}
	}

	rewind(tfp);
	//create an array to store the tracefile
	instrn = malloc(sizeof(addr_t)*instrn_size);
	int i = 0;

	//read each &vaddr into an array
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			instrn[i] = vaddr;
			i++;
		}
	}

	// curr_instrn = instrn;
	instrn_num = 0;
	//create an array to store the memory
	memory = malloc(memsize * sizeof(unsigned long));
}
