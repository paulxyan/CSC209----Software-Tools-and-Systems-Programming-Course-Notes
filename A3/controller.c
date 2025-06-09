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
#include <signal.h>
#include <sys/wait.h>
#include "raid.h"

/*
 * This file implements the RAID controller that manages communication between
 * the main RAID simulator and the individual disk processes. It uses pipes
 * for inter-process communication (IPC) and fork to create child processes
 * for each disk.
 */

// Global array to store information about each disk's communication pipes.
static disk_controller_t* controllers;

/* Ignoring SIGPIPE allows us to check write calls for error rather than
 * terminating the whole system.
 */
static void ignore_sigpipe() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("Failed to set up SIGPIPE handler");
    }
}

/* Initialize the num-th disk controller, creating pipes to communicate
 * and creating a child process to handle disk requests.
 *
 * Returns 0 on success and -1 on failure.
 */
static int init_disk(int num) {
    ignore_sigpipe();

    //Complete this function

    //Keep track of current disk controller
    disk_controller_t* curr_contr = &controllers[num];
    
    //Create both pipes to communicate with disk; also error check as well
    if (pipe(curr_contr->to_disk) == -1) {
        perror("Pipe");
        return -1;
    }

    if (pipe(curr_contr->from_disk) == -1) {
        perror("Pipe");
        return -1;
    }

    int r;

    //Error checking for fork child process
    if ((r=fork()) == -1) {
        perror("Fork");
        return -1;
    }

    else if (r > 0) {
        //In parent process, close the unused ends of pipes 
        curr_contr->pid = r;
        if (close(curr_contr->to_disk[0]) == -1) {
            perror("Closing pipe:");
            return -1;
        }
        if (close(curr_contr->from_disk[1]) == -1){
            perror("Closing pipe:");
            return -1;
        }
        
    }

    else if (r == 0) {

        //When in the child/disk process, need to loop through the 
        //Controller array and close all the unopened pipes it inherited
        //child process itself <num> is covered as well
        for (int i = 0; i < num+1; i++) {

            curr_contr = &controllers[i];
            //Further error checking 
            if (close(curr_contr->to_disk[1]) == -1) {
                perror("Closing pipe:");
                return -1;
            }
            if (close(curr_contr->from_disk[0]) == -1){
                perror("Closing pipe:");
                return -1;
            }
        }
        //Start to call start_disk to start simulating a disk in RAID 4 system
        start_disk(num, controllers[num].from_disk[1], controllers[num].to_disk[0]);
        exit(0);
    }

    return 0;
}

/* Restart the num-th disk, whose process is assumed to have already been killed.
 *
 * This function is very similar to init_disk.
 * However, since the other processes have all been started,
 * it needs to close a larger set of open pipe descriptors
 * inherited from the parent.
 *
 * Returns 0 on success and -1 on failure.
*/
int restart_disk(int num) {
    ignore_sigpipe();
    disk_controller_t* curr_contr = &controllers[num];

    //Error checking for re-opening both pipes
    if (pipe(curr_contr->to_disk) == -1) {
        perror("Pipe");
        return -1;
    }

    if (pipe(curr_contr->from_disk) == -1) {
        perror("Pipe");
        return -1;
    }

    int r;
    //Error checking for fork
    if ((r=fork()) == -1) {
        perror("Fork");
        return -1;
    }

    else if (r > 0) {

        //Closing unused pipes once again on the parents end
        if (close(curr_contr->to_disk[0]) == -1) {
            perror("Closing pipe:");
            return -1;
        }

        if (close(curr_contr->from_disk[1]) == -1){
            perror("Closing pipe:");
            return -1;
        }
        
    }

    else if (r == 0) {
        curr_contr->pid = r;
        for (int i = 0; i < num_disks; i++) {
            //Close all the previously opened pipes the child inherits
            if (i == num) continue; //Ignoring own disk for logic clarity, we want to close everythng else
            disk_controller_t* other_contr = &controllers[i];
            //Error checking for closing unused pipe
            if (close(other_contr->to_disk[1]) == -1) {
                perror("Close");
                return -1;
            }
            if (close(other_contr->from_disk[0]) == -1){
                perror("Close");
                return -1;
            }
        }
        //Starting the disk process again
        start_disk(num, controllers[num].from_disk[1], controllers[num].to_disk[0]);
        exit(0);
    }

    //Complete this function
    return 0;
}

/* Initialize all disk controllers by initializing the controllers
 * array and calling init_disk for each disk.
 *
 * total_disks is the number of data disks + 1 for the parity disk.
 *
 * Returns 0 on success and -1 on failure.
 */
int init_all_controllers(int total_disks) {
    //Complete this function
    //Dynamically allocate memory on heap for controllers global variable
    controllers = malloc(sizeof(disk_controller_t) * total_disks);

    if (controllers == NULL) {
        perror("Malloc: ");
        return -1;
    }

    for (int i = 0; i < total_disks; i++) {
        //Initialize all <total_disks> disks
        if (init_disk(i) == -1) {
            fprintf(stderr, "init_disk error");
            free(controllers);
            return -1;
        }
    }

    return 0;
}

/* Read the block of data at block_num from the appropriate disk.
 * The block is stored to the memory pointed to by data.
 *
 * If parity_flag == 1, read from parity disk.
 * If parity_flag == 0, read from data disk.
 *
 * Returns 0 on success and -1 on failure.
 */
int read_block_from_disk(int block_num, char* data, int parity_flag) {
    if (!data) {
        fprintf(stderr, "Error: Invalid data buffer\n");
        return -1;
    }

    // Identify the stripe to read from
    int disk_num;
    if (parity_flag == 1) {
        disk_num = num_disks;
    } else {
        disk_num = block_num % num_disks;
    }

    disk_command_t cmd = CMD_READ;

    // Each disk has a linear array of blocks, so the block number on an
    // individual disk is the same as the stripe number
    block_num = block_num / num_disks; //Just to demonstrate the point

    // Write the command and the block number to the disk process
    // Then read the block from the disk process
    // Complete this function
    
    //Keeping track of the current controller we need to send command to
    disk_controller_t* curr_disk = &controllers[disk_num];
    if (write(curr_disk->to_disk[1], &cmd, sizeof(disk_command_t)) == -1) {
        perror("Write");
        restore_disk_process(disk_num); //Call restore_disk_process if writing causes error
    }
    //Sending the block number to the disk to read 
    if (write(curr_disk->to_disk[1], &block_num, sizeof(int)) == -1) {
        perror("Write");
        return -1;
    }

    //Error checking when reading from a block
    int nread;
    if ((nread = read(curr_disk->from_disk[0], data, block_size)) < block_size) {
        fprintf(stderr, "Error reading from disk");
        return -1;
    }

    else if (nread == -1) {
        perror("Reading");
        return -1;
    }

    return 0;
}

/* Write a block of data to the block at block_num on the appropriate disk.
 * The block is stored at the memory pointed to by data.
 *
 * If parity_flag == 1, write to parity disk.
 * If parity_flag == 0, write to data disk.
 *
 * Returns 0 on success and -1 on failure.
 */
int write_block_to_disk(int block_num, char *data, int parity_flag) {
    // TODO: complete this function (see read_block_from disk for some tips)
    if (!data) {
        fprintf(stderr, "Error: Invalid data buffer\n");
        return -1;
    }

    int disk_num;

    if (parity_flag == 1) {
        disk_num = num_disks;
    } else {
        disk_num = block_num % num_disks;
    }

    disk_command_t cmd = CMD_WRITE;

    block_num = block_num / num_disks;
    //Keeping track of current disk
    disk_controller_t* curr_disk = &controllers[disk_num];
    //Writing command to the pipe
    if (write(curr_disk->to_disk[1], &cmd, sizeof(disk_command_t)) == -1) {
        perror("Write");
        restore_disk_process(disk_num); //Once again calling restore_disk_process
    }
    //Writing the block size
    if (write(curr_disk->to_disk[1], &block_num, sizeof(int)) == -1) {
        perror("Write");
        return -1;
    }
    int nwrites;
    //Error checking for writing <data> to disk pipe
    if ((nwrites = write(curr_disk->to_disk[1], data, block_size)) < block_size) {
        perror("Didn't write enoughbytes");
        return -1;
    }

    if (nwrites == -1) {
        perror("Writing error");
        return -1;
    }

    return 0;
}

/* Write the memory pointed to by data to the block at block_num on the
 * RAID system, handling parity updates.
 * If block_num is invalid (outside the range 0 to disk_size/block_size)
 * then return NULL.
 *
 * Returns 0 on success and the disk number we tried to read from on failure.
 */
int write_block(int block_num, char *data) {
    //Complete this function (see handout for more details)

    //Checking for valid block number
    int stripe_num = block_num / num_disks; 
    int num_blocks = disk_size / block_size;

    //Error checking for if the stripe/block is out of bounds in the disk
    if (stripe_num < 0 || stripe_num >= num_blocks) {
        fprintf(stderr, "Invalid Block Num: %d\n", stripe_num);
        return -1;
    }

    //Using malloc to allocate mem. for the block of memory in 
    //the disk to update parity disk later on
    char *k_block = malloc(block_size);
    //More error checking 
    if (!k_block) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    
    //Reading the old data from block_num before replacing
    if (read_block_from_disk(block_num, k_block, 0) == -1) {
        free(k_block);
        return -1;
    }
    
    //Store memory for reading parity block
    char * parity_block = malloc(block_size);

    //More error checking when reading from disk
    if (!parity_block) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    //Using parity flag to read from parity block instead
    if (read_block_from_disk(block_num, parity_block, 1) == -1) {
        free(k_block);
        free(parity_block);
        return -1;
    }

    //Computing XOR of k_block and parity_block
    for (int i = 0; i < block_size; i++) {
        parity_block[i] ^= k_block[i];
    }

    // Updating parity_block for data we want to write to disk 
    for (int j = 0; j < block_size; j++) {
        parity_block[j] ^= data[j];
    }

    //Updating parity block and writing new data to disk

    if ((write_block_to_disk(block_num, parity_block, 1)) == -1) {
        free(k_block);
        free(parity_block);
        return -1;
    }
    if ((write_block_to_disk(block_num, data, 0) == -1)){
        free(k_block);
        free(parity_block);
        return -1;
    }

    //Freeing malloc'd memory
    free(k_block);
    free(parity_block);

    return 0;
}

/* Read the block at block_num from the RAID system into
 * the memory pointed to by data.
 * If block_num is invalid (outside the range 0 to disk_size/block_size)
 * then return NULL.
 *
 * Returns a pointer to the data buffer on success and NULL on failure.
 */
char *read_block(int block_num, char *data) {
    //Complete this function

    int stripe_num = block_num / num_disks;
    int num_blocks = disk_size / block_size;
    //Making sure stripe_num is within range
    if (stripe_num < 0 || stripe_num >= num_blocks) {
        fprintf(stderr, "Invalid Block Num: %d\n", stripe_num);
        return NULL;
    }
    //callong read_block_from_disk to send command to disk
    else if (read_block_from_disk(block_num, data, 0) == -1){
        fprintf(stderr, "Error called read_block_from_disk");
        return NULL;
    }
    //Return the <data> read
    return data;

}

/* Send exit command to all disk processes.
 *
 * Returns when all disk processes have terminated.
 */
void checkpoint_and_wait() {
    for (int i = 0; i < num_disks + 1; i++) {
        disk_command_t cmd = CMD_EXIT;
        ssize_t bytes_written = write(controllers[i].to_disk[1], &cmd, sizeof(cmd));
        if (bytes_written != sizeof(cmd)) {
            fprintf(stderr, "Warning: Failed to send exit command to disk %d\n", i);
        }
    }
    // wait for all disks to exit
    // we aren't going to do anything with the exit value
    for (int i = 0; i < num_disks + 1; i++) {
        wait(NULL);
    }
}


/* Simulate the failure of a disk by sending the SIGINT signal to the
 * process with id disk_num.
 */
void simulate_disk_failure(int disk_num) {
    if(debug) {
        printf("Simulate: killing disk %d\n", disk_num);
    }
    kill(controllers[disk_num].pid, SIGINT);
}

/* Restore the disk process after it has been killed.
 * If some aspect of restoring the disk process fails, 
 * then you can consider it a catastropic failure and 
 * exit the program.
 */
void restore_disk_process(int disk_num) {
    //First, call restart_disk to be able to communicate with disk 
    if (restart_disk(disk_num) == -1){
        fprintf(stderr, "Catastrophic failure in restore disk");
        exit(1);
    }
    //Calculate total blocks in disk
    int num_blocks = disk_size / block_size;
    //Create an array of char* to represent all blocks in disk 
    char ** restored_block = malloc(sizeof(char*) * num_blocks);
    //Checking for malloc error
    if (!restored_block) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    //Temprorary string to store when we iterate through each block in each disk to store 
    //To restore Each block in disk 
    char * temp = malloc(sizeof(char) * block_size);

    if (!temp) {
        fprintf(stderr, "Temp buffer allocation failed\n");
        exit(1);
    }
    //Pad the restored_block array with zeros
    for (int i = 0; i < num_blocks; i++) {
        restored_block[i] = calloc(block_size, sizeof(char)); // calloc zeroes memory
        if (!restored_block[i]) {
            fprintf(stderr, "Block allocation failed\n");
            exit(1);
        }
    }
    //Prepare to send read cmd to get info on every block in every disk so we can restore disk
    disk_command_t cmd = CMD_READ;

    for (int i = 0; i < num_disks; i++) {
        //Want to ingnore the disk we want to restore, only collect data on other disks' blocks
        if (i == disk_num) {
            continue;
        }

        disk_controller_t* curr_disk = &controllers[i];

        for (int j = 0; j < num_blocks; j++) {
            //Use a nested for loop to iterate through every disk, then in every disk collect 
            //and read block data and XOR that data with the block number in disk_num
            if (write(curr_disk->to_disk[1], &cmd, sizeof(disk_command_t)) == -1) {
                perror("Write");
                exit(1);
            }

            if (write(curr_disk->to_disk[1], &j, sizeof(int)) == -1) {
                perror("Write");
                exit(1);
            }

            int nread;

            if ((nread = read(curr_disk->from_disk[0], temp, block_size)) < block_size) {
                fprintf(stderr, "Error reading from disk");
                exit(1);
            }
        
            else if (nread == -1) {
                perror("Reading");
                exit(1);
            }
            //This for loop is responsible for properly using XOR on every block in every other disk 
            //to restore block
            for (int k = 0; k < block_size; k++) {
                restored_block[j][k] ^= temp[k];
            }
        }
    }

    //The rest of the code is to write the restored_block back to original <disk_num>
    cmd = CMD_WRITE;

    disk_controller_t* curr_disk = &controllers[disk_num];

    for (int i = 0; i < num_blocks; i++) { 

        if (write(curr_disk->to_disk[1], &cmd, sizeof(disk_command_t)) == -1) {
            perror("Write");
            exit(1);
        }

        if (write(curr_disk->to_disk[1], &i, sizeof(int)) == -1) {
            perror("Write");
            exit(1);
        }

        int nwrites;
        //This part is responsible for writing back to disk 
        if ((nwrites = write(curr_disk->to_disk[1], restored_block[i], block_size)) < block_size) {
            perror("Didn't write enough bytes");
            exit(1);
        }
        //Further error checking 
        if (nwrites == -1) {
            perror("Writing error");
            exit(1);
        }

    }

     // Cleanup
     free(temp);
     for (int i = 0; i < num_blocks; i++) {
         free(restored_block[i]);
     }
     free(restored_block);

}
