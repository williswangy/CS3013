// CS3013 proj4
// Yuan Wang
// ywang19@wpi.edu
// some ideas are the reults after the discussion with the classmates
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>

#include "utility.h"
#include "thread.h"

sem_t * semlist;          // global semaphore
int indexofThread;        // thread index
int maxthread;            // max of threads
char ** FN_buffer_thread; // child thread buffer

unsigned long int badfile_num;    // sem 0 -> bad files
unsigned long int directory_num;  // sem 1 -> directory
unsigned long int regfile_num;    // sem 2 -> regular files
unsigned long int spcfile_num;    // sem 3 -> special files
unsigned long int reg_byte;       // sem 4 -> total bytes of regular files
unsigned long int reg_text;       // sem 5 -> Text files
unsigned long int text_byte;      // sem 6 -> total bytes of text files


int main(int argc, char *argv[])
{
    
    size_t FN_buf_size = 512;
    char *FN_buffer = malloc(sizeof(char) * FN_buf_size);//file buffer
    
    
    //set global varibales' values
    badfile_num   = 0;
    directory_num = 0;
    regfile_num   = 0;
    spcfile_num   = 0;
    reg_byte      = 0;
    reg_text      = 0;
    text_byte     = 0;
    
    //get time for printing performance
    int temp = RUSAGE_SELF;
    struct rusage usage;
    struct timeval userStart, sysStart;//user time when start, system time when start
    getrusage(temp, &usage);
    userStart = usage.ru_utime;
    sysStart  = usage.ru_stime;
    time_t wall_ini = time(NULL);
    
    if(argc == 1)
    {
        
        struct stat state_buffer;
        
        while(getline(&FN_buffer, &FN_buf_size, stdin) != -1)
        {
            FN_buffer[strlen(FN_buffer) - 1] = '\0';
            if(stat(FN_buffer, &state_buffer) == -1)//bad files
            {
                badfile_num ++;
            }
            else
            {
                if(S_ISREG(state_buffer.st_mode) != 0)//regular file
                {
                    regfile_num++;
                    reg_byte += state_buffer.st_size;
                    
                    if(textfile(FN_buffer))
                    {
                        reg_text ++;
                        text_byte += state_buffer.st_size;
                    }
                }
                else if(S_ISDIR(state_buffer.st_mode) != 0)//directory
                {
                    directory_num++;
                }
                else //special files
                {
                    spcfile_num++;
                }
            }
        }
        
        printf("Bad Files: %ld\n", badfile_num);
        printf("Directories: %ld\n", directory_num);
        printf("Regular Files: %ld\n", regfile_num);
        printf("Special Files: %ld\n", spcfile_num);
        printf("Regular File Bytes: %ld\n", reg_byte);
        printf("Text Files: %ld\n", reg_text);
        printf("Text File Bytes: %ld\n", text_byte);
        printstats(userStart, sysStart, wall_ini);
        return 1;
    }
    //multithread
    else if((argc == 2 || argc == 3) && !strcmp("thread", argv[1]))
    {
        //local variable initializations
        int routine;
        long t = 0;
        int i;
        
        pthread_t * workers;
        void *status;
        
        //Get the max thread values
        if(argc == 2)
        {
            maxthread = 10;
            printf("No maximum thread number specified, default to 10.\n");
        }
        else
        {
            maxthread = atoi(argv[2]);
            if(maxthread < 1)
            {
                maxthread = 1;
                printf("Thread Value Error(Out of boundary), set to minvalue 1.\n");
            }
            else if(maxthread > 15)
            {
                maxthread = 15;
                printf("Thread Value Error(Out of boundary), set to maxvalue 15. \n");
            }
            else printf("MAX THREAD VALUE:  %d. \n", maxthread);
        }
        
        //assign variable values
        workers = malloc(sizeof(pthread_t) * maxthread);
        indexofThread = 0;
        
        FN_buffer_thread = malloc(maxthread * (sizeof(char *)));
        for(i = 0; i < maxthread; i ++)
        {
            FN_buffer_thread[i] = malloc(sizeof(char) * FN_buf_size);
        }
        semlist = malloc(sizeof(sem_t) * 8);
        //initializes semaphores
        for(i = 0; i < 8; i++)
        {
            sem_init(&semlist[i], 1, 1);
        }
        
        while(getline(&FN_buffer, &FN_buf_size, stdin) != -1)//temporarily store the file in the buffer
        {
            FN_buffer[strlen(FN_buffer) - 1] = '\0';
            
            if(indexofThread >= maxthread)//index > max thread
            {
                routine = pthread_join(workers[indexofThread % maxthread], &status);//wait for old
                if (routine)
                {
                    printf("ERROR.\n");
                    exit(-1);
                }
            }
            
            //filename -> thread buffer
            strcpy(FN_buffer_thread[indexofThread % maxthread], FN_buffer);
            
            routine= pthread_create(&workers[indexofThread % maxthread], NULL, worker, (void *)t);
            if (routine)
            {
                printf("ERROR!\n");
                exit(-1);
            }
            t++;
            indexofThread ++; //index of thread + 1
        }
        
        for(i = 0; i < maxthread; i ++)
        {
            routine = pthread_join(workers[i], &status);
        }
        
        printf("Bad Files: %ld\n", badfile_num);
        printf("Directories: %ld\n", directory_num);
        printf("Regular Files: %ld\n", regfile_num);
        printf("Special Files: %ld\n", spcfile_num);
        printf("Regular File Bytes: %ld\n", reg_byte);
        printf("Text Files: %ld\n", reg_text);
        printf("Text File Bytes: %ld\n", text_byte);
        
        //delete used semaphores
        for(i = 0; i < 8; i++)
        {
            sem_destroy(&semlist[i]);
        }
        printstats(userStart, sysStart, wall_ini);
        return 1;
    }
    else
    {
        printf("ERROR! \n");
        return 0;
    }
}




