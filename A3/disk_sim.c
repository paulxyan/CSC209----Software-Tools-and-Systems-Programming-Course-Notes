/* This code is provided solely for the personal and private use of students
 * taking the CSC209H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Karen Reid, Paul He, Philip Kukulak
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2025 Karen Reid
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "raid.h"


int debug = 1;  // Set to 1 to enable debug output, 0 to disable


/*
 * Main function for the disk simulation process, which runs in a child process
 * created by the RAID controller.
 *
 * id is the disk number or index into the controllers table,
 * to_parent is the pipe descriptor for writing to the parent,
 * from_parent is the pipe descriptor for reading from the parent.
 *
 * Returns 0 on success and 1 on failure.
 */

static int checkpoint_disk(char *disk_data, int id); //To avoid implicit function declaration error

int start_disk(int id, int to_parent, int from_parent) {

    int status = 0;

    // Allocate memory for disk data

    int num_blocks = disk_size / block_size;
    char all_blocks[num_blocks][block_size]; //My implementation is using a 2D array for the disk, each entry represents a block

    //Use memset to initialize all elements of all_blocks to zero
    memset(all_blocks, 0, sizeof(all_blocks));

    // Main command loop to handle requests from the parent.
    // This is an infinite loop that is only terminated when
    // an exit command is received.
    while (1) {
        disk_command_t cmd;

        // Read command from the parent 

        int nread = read(from_parent, &cmd, sizeof(disk_command_t));

        //Error checking throughly when reading a command from pipe

        if (nread < (int) sizeof(disk_command_t)) {
        
            fprintf(stderr, "Reading bytes insufficient");
            return 1;

        } else if (nread == -1) {

            perror("Reading error");
            return 1;

        }

        // The type of command received from the parent
        // determines which action is taken next.
        switch (cmd) {
            case CMD_READ: {
                int block_num;
                //To avoid same variable name error, 
                //make a dummy variable for error checking nread2
                int nread2 = read(from_parent, &block_num, sizeof(int));
                nread2 = nread2; //This line is to avoid undeclared var error in Markus testing

                if (nread2 < (int)sizeof(int)) {
                    fprintf(stderr, "Reading bytes insufficient");
                    return 1;
                } else if (nread == -1) {
                    perror("Reading error");
                    return 1;
                }

                int nwrites;
                //Writing to the pipe from all_blocks
                if ((nwrites=write(to_parent, all_blocks[block_num], block_size)) < block_size) {
                    fprintf(stderr, "Writing insufficient bytes");
                    return 1;
                }
            }
                break;

            case CMD_WRITE: {
                int block_num;
                //Using another dummy variable nread3 for same reason as above
                int nread3 = read(from_parent, &block_num, sizeof(int));

                if ((nread3 < (int)sizeof(int))) {
                    fprintf(stderr, "Reading bytes insufficient");
                    return 1;
                } else if (nread == -1) {
                    perror("Reading error");
                    return 1;
                }
                //Storing what parent wrote to child back in the disk at index <block_num>
                if ((nread=read(from_parent, all_blocks[block_num], block_size)) < block_size) {
                    fprintf(stderr, "Reading bytes insufficient");
                    return 1;

                } else if (nread == -1) {
                    perror("Reading error");
                    return 1;
                }
            }
                break;
            case CMD_EXIT: {
                char combined[num_blocks * block_size];
                //Converting from 2D array all_blocks to 1D array combined in order to fit the type
                //Description of checkpoint_disk, also using memcpy to copy elements exactly
                memcpy(combined, all_blocks, sizeof(combined));
                //using n to error check when calling checkpoint_disk
                int n = checkpoint_disk(combined, id);
                if (n == -1 || n == 1) {
                    fprintf(stderr, "Checkpoint_disk failure\n");
                    return 1;
                }
                //Error checking for closing pipes
                if (close(to_parent) == -1) {
                    perror("close error:");
                    return 1;
                }
            
                if (close(from_parent) == -1) {
                    perror("close error:");
                    return 1;
                }
            }
                //Finally exiting the program
                exit(status);
                break;

            default:
                fprintf(stderr, "Error: Unknown command %d received\n", cmd);
                status = 1;
                break;
        }
    }

    // Checkpoint and cleanup before exiting
    //Closing all the pipes before exiting and cleaning up 

    if (close(to_parent) == -1) {
        perror("close error:");
        return 1;
    }
    if (close(from_parent) == -1) {
        perror("close error:");
        return 1;
    }

    exit(status);
}

/* Save the disk's data, pointed to by disk_data, to a file named id.
 *
 * Returns 0 on success, and -1 on failure.
 */
static int checkpoint_disk(char *disk_data, int id) {
    if (!disk_data) {
        fprintf(stderr, "Error: Invalid parameters for checkpoint\n");
        return -1;
    }

    // Create a file name for this disk
    char disk_name[MAX_NAME];
    if (snprintf(disk_name, sizeof(disk_name), "disk_%d.dat", id) >= (int)sizeof(disk_name)) {
        fprintf(stderr, "Error: Disk name too long for disk %d\n", id);
        return 1;
    }

    FILE *fp = fopen(disk_name, "wb");
    if (!fp) {
        perror("Failed to create checkpoint file");
        return -1;
    }

    size_t bytes_written = fwrite(disk_data, 1, disk_size, fp);
    if (bytes_written != (size_t)disk_size) {
        if (ferror(fp)) {
            fprintf(stderr, "Failed to write checkpoint data");
        } else {
            fprintf(stderr, "Error: Incomplete write during checkpoint\n");
        }
        fclose(fp);
        return -1;
    }

    if (fclose(fp) != 0) {
        perror("Failed to close checkpoint file");
        return -1;
    }

    return 0;
}
