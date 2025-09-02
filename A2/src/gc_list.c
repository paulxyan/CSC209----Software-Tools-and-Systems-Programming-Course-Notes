#include <stdio.h>
#include "list.h"
#include "gc.h"

/* traverse the list data-structure and call mark_one(void *vptr) on
 *  each piece of memory that is still used in the list
 * head is the head of the list data-structure being traversed NOT
 * the head of the memory-list which is part of gc
 */
void mark_list(void *head) {

    // Initalize the head linked list to prepare traversing
    // Type cast the argument into a List pointer
    List *curr = (List *) head;
    
    //Checking for type-casting errors

    if (curr == NULL) {

	fprintf(stderr, "Error: Invalid type casting to List node\n");
	return;
    }

    //After error checking, traverse through linked list
    //And call mark_one to mark each memory address
    while (curr != NULL) {
	//Call mark_one on each memory address
	int check = mark_one((void *)curr);

	//If memory has not been found in memory list, then we have to report and error and
	//exit from the function
	if (check == 2) {
	
 	   fprintf(stderr, "Memory Doesnt Exist in Garbage collector\n");
	   
	} else if (check == 1) {
	  //If memory address was already marked, continue iterating and dont do anything
	  
	}

	curr = curr->next;
 	
    }
}
