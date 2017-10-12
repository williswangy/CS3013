//CS3013 proj4
//Yuan Wang
//ywang19@wpi.edu

#ifndef utility_h
#define utility_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "thread.h"

extern sem_t * semlist;

int textfile(char * file_path);
void beginRegion(int i);
void endRegion(int i);
void printstats(struct timeval userBefore, struct timeval sysBefore, time_t wall_ini);

#endif /* utility_h */
