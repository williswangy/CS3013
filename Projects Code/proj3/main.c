/**
 * CS3013
 * Yuan Wang(ywang19)
 * main.c
 * For this assignment I found it is easy to parse the data from the .txt file
 * In main.c:
 * parse fucntion to parse data from the input file
 * main function
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "utility.h"

#define MAXTHREAD = 10;
#define checker " \a\n\r\t"

struct msg * mailbox; //global variable mailbox
sem_t * send_sem;//global semaphores
sem_t * rec_sem;//global semaphores
FILE *fp;//global file pointer
int nb;//nb arugument

char **parse(char *input);//parse the date from the input file

char **parse(char *input)
{
    int i = 0;//counter
    char *arguement = malloc(128 * sizeof(char));//allocate memory for the arugument = 1 dimension
    arguement = strtok(input, checker); //divide the input strings into severl pieces
    char **arguements = malloc(128 * sizeof(char*));//allocate memory for the arguments = 2 dimensions
    
    if(!arguements)
    {
        printf("EORROR!");
        exit(1);
    }
    
    //put arguements in
    while(arguement != NULL)
    {
        arguements[i] = arguement;
        i ++;
        arguement = strtok(NULL, checker);
    }
    
    //if there exits NULL arguement, put them int the end
    arguements[i] = arguement;
    return arguements;
}



int main(int argc, char *argv[]) {
    long NUM_THREAD;//number of input threads
    int routine; // routine counter
    pthread_t main_threads;//main thread
    
    if(argc != 3 && argc != 4)// check whether the commandline input are valid
    {
        printf("ERROR! Please read the instruction.");
        exit(-1);
    }
    
    NUM_THREAD = atoi(argv[1]);//the first one is the number of thread
    if(NUM_THREAD > 10)//Automatically set to 10 if the imput number > 10
    {
        printf("The Input Threads cannot exceed 10.\n");
        printf("THREAD NUMBERS AUTOMATICALLY SET TO THE MAX 10.\n");
        NUM_THREAD = 10;
    }
    
    mailbox = malloc(sizeof(struct msg) * NUM_THREAD);//allocate memory for mailbox
    
    //read input file
    fp = fopen(argv[2], "r");
    if(fp == NULL)
    {
        perror("ERROR!\n");
        exit(-1);
    }
    //nb options
    if(argc == 4) {
        nb = 1;
        printf("YOUR NB option : ON.\n\n\n");
    }
    else {
        nb = 0;
        printf("YOUR NB option : OFF.\n");
    }
    //inialize the mailbox
    routine = pthread_create(&main_threads, NULL, MailBox, NULL);
    if (routine)
    {
        printf("ERROR!");
        exit(-1);
    }
    //create main thread
    routine = pthread_create(&main_threads, NULL, main_thread, (void *)NUM_THREAD);
    if (routine)// Failure on _create: return value < 0
    {
        printf("ERROR!");
        exit(-1);
    }
    pthread_exit(NULL);
    return 1;
}


