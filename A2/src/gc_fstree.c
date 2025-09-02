#include <stdio.h>
#include "fstree.h"
#include "gc.h"

/* Traverse the fstree data-structure and call mark_one(void *vptr) on
 * each piece of memory that is still used in the fstree
 *
 * mark_one returns 0 if the chunk was marked for the first time and 1 if
 * it has been seen already. We need this to avoid revisiting pieces of the
 * tree that we have already marked -- where hard links have created cycles
 */

void mark_fstree(void *head) {
    //Typecasting and initializing head of fstree
    // For traversing
    Fstree * curr_node = (Fstree *) head;
    
    if (curr_node == NULL) {
	//Checking for typecasting error
	fprintf(stderr, "Typcasting error");
	return;
    }

    //Checking the head of the fstree first
    //And marking the memory as used, checking
    //for hard links in the process

    int check1 = mark_one((void *) curr_node);

    if (check1 == 2) {
	//If memory doesn't exist we still need to continue traversing
        fprintf(stderr, "Memory doesn't exist in garbage collection\n");
  
    } else if (check1 == 1) {

        fprintf(stderr, "Hard Link detected\n");
        return; 
    }

    //Must free the name as well
    mark_one((void*) curr_node ->name);  
	
    //Start to traverse links 
    Link * curr_link = curr_node->links;

    while (curr_link != NULL) {

	//Mark the curr_link memory 
	int check2 = mark_one((void*) curr_link);
        
	if (check2 == 2) {
        
	   fprintf(stderr, "Memory doesnt exist in garbage collection\n");

        } else if (check2 == 1) {
	
	// If a link has been traversed before, we still 
	// continue traversing to find other possible links

           fprintf(stderr, "Hard Link detected");
           curr_link = curr_link -> next;
	   continue;

	}
	
        //Recursively call the function
	//On the current links node that it referse to traverse 
	//the entire file structure

        mark_fstree(curr_link -> fptr);
	curr_link = curr_link->next;
    }

}
