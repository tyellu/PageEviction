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

int instrn_size;

// addr_t *curr_instrn;

int instrn_num;

// void print_coreMap(struct frame *cmap){
// 	for(int i=0; i<memsize; i++){
// 		printf("%d: %lx\n",i,coremap[i].vaddr);
// 	}
// }


/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int max_distance = 0;
	int frame_index = 0;
	int i;
	for(i=0; i < memsize; i++){
		addr_t curr_frame_addr = coremap[i].vaddr;
		int temp_distance = 0;
		int found = 0;
		int j;
		for(j = instrn_num; j < (instrn_size); j++){
			temp_distance++;
			if(instrn[j] == curr_frame_addr){
				if(max_distance < temp_distance){
					frame_index = i;
					max_distance = temp_distance;
					found = 1;
				}else{
					found = 1;
				}
			}
		}

		if(temp_distance == (instrn_size - instrn_num) && (found == 0)){
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
	//read each &vaddr into an array
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			instrn_size++;
		}
	}

	rewind(tfp);
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
}
