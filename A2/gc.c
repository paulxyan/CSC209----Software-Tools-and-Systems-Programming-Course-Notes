#include <stdio.h>
#include <stdlib.h>
#include "gc.h"

// global variable to be head of allocated pieces
struct mem_chunk *memory_list_head = NULL;

// global variable for debugging
int debug = 0;

void *gc_malloc(int nbytes) {
	
    //Calling malloc and storing pointer 
    //Return later to avoid memory leaks    
    void * memory_val = malloc(nbytes);

    //Error checking for malloc sys. call
    if (memory_val == NULL) {

	perror("malloc1: ");
	exit(1);

    }

    //Creating mem_chunk to record dynamic memory
    //Used for mark_and_sweep later on    
    struct mem_chunk *new_node = malloc(sizeof(struct mem_chunk));

    //Once again checking for errors
    if (new_node == NULL) {
	
	perror("malloc2: ");
	free(memory_val);
	exit(1);

    }

    //Storing info about memory_val pointer and 
    //adding to the memory chunk data struct
    new_node->address = memory_val;

    new_node->in_use = USED; //Memory when called will default be in-use

    new_node->next = memory_list_head; //Inserting the node into the mem chunk

    memory_list_head = new_node;
    
    return memory_val; // return pointer to allocated memory

}

/* Executes the garbage collector.
 * obj is the root of the data structure to be traversed
 * mark_obj is a function that will traverse the data structure rooted at obj
 *
 * The function will also write to the LOGFILE the results messages with the
 * following format strings:
 * "Mark_and_sweep running\n"
 * "Chunks freed this pass: %d\n"
 * "Chunks still allocated: %d\n"
 */

void mark_and_sweep(void *obj, void (*mark_obj)(void *)) {


    //Initialize a stream to logfile, appending to it so
    //We can store multiple data, LOGFILE defined in gc.h

    FILE *log_file = fopen(LOGFILE, "a");
	
    //Checking if opening file caused error	

    if (log_file == NULL) {
    	perror("Error opening file: ");
    	exit(1);
    }

    fprintf(log_file, "Mark_and_sweep running\n");

    // 1. Mark phase: Set all chunks to UNUSED
    struct mem_chunk *curr = memory_list_head;

    while (curr != NULL) {
    	curr->in_use = NOT_USED;
    	curr = curr->next;
    }

    //Started marking the dynamically located memory in the 
    //data struct <obj> as used
    
    mark_obj(obj);

    // Start traversing the memory chunks, keeping track of
    // How many memory chunks are removed and how many are kept respectively
    int freed_count = 0;
    int allocated_count = 0;
    
    //Initialize sweep to keep track of current node in memory list 
    //So we can traverse and find unused memory to free
    struct mem_chunk *sweep = memory_list_head;
    struct mem_chunk *prev = NULL;
    while (sweep != NULL) {
	//Free the dynamically allocated memory stored in
	//Mem chunks address if its marked as not used
	//Also chunk containing unused memory must be removed from 
	//memory list, and also whose memory must be freed as well
    	if (sweep->in_use == NOT_USED) {

    	    struct mem_chunk *temp = sweep;
	    //Checking if we are removing from the head
    	    if (prev == NULL) {

    		memory_list_head = sweep->next;
    	
	    } else {

		prev->next = sweep->next;

       	    }
	    //Removing memory chunk, freeing the unused address 
	    //And the memory chunk holding that address
            sweep = sweep->next;

            free(temp->address);

            free(temp);

            freed_count++;
       		
       	} else {
	      //If memory chunk is in use, continue traversing until 
	      //We've reached the end
       	      prev = sweep;
       	      sweep = sweep->next;
       	      allocated_count++;
       	 }
      }	 

	fprintf(log_file, "Chunks freed this pass: %d\n", freed_count);
	fprintf(log_file, "Chunks still allocated: %d\n", allocated_count);
	fclose(log_file);
}

    
/*
 Mark the chunk of memory pointed to by vptr as in use.
 Return codes:
   0 if successful
   1 if chunk already marked as in use
   2 if chunk is not found in memory list

   Here is a print statement to print an error message:
   fprintf(stderr, "ERROR: mark_one address not in memory list\n");
 */
int mark_one(void *vptr) {
    //Initialize head of memory list to traverse over
    struct mem_chunk* curr = memory_list_head;

    while (curr != NULL) {

        if (curr->address == vptr) {
       	    //Checking for case if we've already marked memory
            if (curr->in_use == USED) {

                return 1;
            }
	    //Case where marking memory is successful
            curr -> in_use = USED;
            return 0;
        }
	//Continue traversing if memory hasn't been found

        curr = curr->next;

    } 

    //When we've finished traversing, then we haven't found the address in 
    //Memory list, and so an error has occured

    fprintf(stderr, "ERROR: mark_one address not in memory list\n");
    return 2;


}

void print_memory_list() {
    struct mem_chunk *current = memory_list_head;
    while (current != NULL) {
        printf("%lx (%d) -> ",(unsigned long) current->address, current->in_use);
        current = current->next;
    }
    printf("\n");
}
