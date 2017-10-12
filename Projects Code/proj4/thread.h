//CS3013 proj4
//Yuan Wang
//ywang19@wpi.edu

#ifndef thread_h
#define thread_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utility.h"

extern sem_t * semlist;

extern int indexofThread;
extern int maxthread;
extern char ** FN_buffer_thread;

extern unsigned long int badfile_num;
extern unsigned long int directory_num;
extern unsigned long int regfile_num;
extern unsigned long int spcfile_num;
extern unsigned long int reg_byte;
extern unsigned long int reg_text;
extern unsigned long int text_byte;

void * worker(void *t);

#endif /* thread_h */
