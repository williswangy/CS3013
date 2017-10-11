/**
 * CS 3013 Project 2
 * Yuan Wang and YuanDa Song ywang19 & ysong5
 * part2test.c
 * We will run 3 children process and 1 parent process to test the syscall
 */
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "processinfo.h"
#include "getprocessinfo.h"

#define LARGE_NUM 500000000L // Used for giving program run time in for loops

//print out the infomation about the 'processinfo' struct
void print_stats()
{
    struct processinfo* info = (struct processinfo*)malloc(sizeof(struct processinfo));
    getprocessinfo(info);
    printf(" State           : %14ld\n", info->state);
    printf(" Pid             : %14d\n", info->pid);
    printf(" Parent Pid      : %14d\n", info->parent_pid);
    printf(" Youngest Child  : %14d\n", info->youngestChild);
    printf(" Younger Sibling : %14d\n", info->youngerSibling);
    printf(" Older Sibling   : %14d\n", info->olderSibling);
    printf(" Uid             : %14d\n", info->uid);
    printf(" Start time      : %14lld\n", info->start_time);
    printf(" User time       : %14lld\n", info->user_time);
    printf(" System time     : %14lld\n", info->sys_time);
    printf(" CU Time         : %14lld\n", info->cutime);
    printf(" CS Time         : %14lld\n", info->cstime);
}

/* Main function
 * Run 3 children and 1 parent process
 */

int main () {
    int pid; // Child process PID
    int i;   // Counter
    long l;  // Run time counter
    long times[] = {LARGE_NUM * 2, LARGE_NUM * 1, LARGE_NUM * 3}; //let child 1 finish before child 0, showing younger and older siblings
    
    for (i = 0; i < 3; i++)
    {
        if ((pid = fork()) == 0)
        {
            for (l = 0; l < times[i]; l++); // Run time
            printf("\n");
            printf("Child %d (%d) info\n", i, getpid());
            print_stats();
            exit(0);
        }
    }
    for (l = 0; l < LARGE_NUM/2; l++); // Run time
    printf("\n");
    printf("Parent process info\n");
    print_stats();
    while(wait(0) > 0); // Wait until all the child process done.
    return 0;
}
