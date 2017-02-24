#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include <string.h>


extern int memsize;

extern int debug;

extern struct frame *coremap;

// Records whether or not the frame has been referenced before
int* clock_bit;
// The pointer to which the search begins
int clock_hand;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	// search for the first unreferenced frame starting from the clock_hand pointer
	int i;
	for (i = clock_hand; i < memsize; i = (i + 1) % memsize) {
		// if bit is still referenced change bit to unreferenced
		if (clock_bit[i]) {
			clock_bit[i] = 0;
		}
		// otherwise change the bit, return it and update the clock_hand
		else {
			clock_bit[i] = 1;
			clock_hand = i;
			return i;
		}
	}
	return 0;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	int frame;
	frame = p->frame >> PAGE_SHIFT;
	clock_bit[frame] = 1;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	clock_hand = 0;
	clock_bit = malloc(sizeof(int) * memsize);
	memset(clock_bit, 0, sizeof(int) * memsize);
}
