//CS3013 proj4
//Yuan Wang
//ywang19@wpi.edu

#include "utility.h"

int textfile(char * file_path)
{
    
    FILE *fp;
    int fd, n; //file descriptor for fp
    char * byte_buffer = malloc(sizeof(char));
    
    
    fp = fopen(file_path, "r");
    
    if(fp == NULL)//NOT TEXT FILE
    {
        return 0;
    }
    
    
    fd = fileno(fp);

    while((n = read(fd, byte_buffer, sizeof(char)) > 0))//get the value of the file byte by byte
    {
        if (write(1,byte_buffer,sizeof(char)) != n)
        {
            printf("ERROR.\n");
        }
         
        
        if(isprint(*byte_buffer) ==  0 && isspace(*byte_buffer) == 0)
        {
            return 0;
        }
        
    }
    free(byte_buffer);
    fclose(fp);
    
    return 1;
}

void beginRegion(int i) {
 
    if(i >= 8)
    {
        printf("ERROR!\n");
        exit(-1);
    }
    sem_wait(&semlist[i]);
   
}

void endRegion(int i)
{
    if(i >= 8)
    {
        printf("ERROR!.\n");
        exit(-1);
    }
    sem_post(&semlist[i]);
    int s;
    sem_getvalue(&semlist[i], &s);
}

void printstats(struct timeval userBefore, struct timeval sysBefore, time_t wall_ini) {
    int who = RUSAGE_SELF;
    struct rusage usage;
    struct timeval userAfter, sysAfter;
    
    getrusage(who, &usage);
    userAfter = usage.ru_utime;
    sysAfter = usage.ru_stime;
    
    printf("the amount of CPU times user used: %f ms\n",
           (userAfter.tv_sec - userBefore.tv_sec)*1000 + (userAfter.tv_usec -userBefore.tv_usec)*0.001);
    printf("the amount of CPU times system used: %f ms\n",
           (sysAfter.tv_sec - sysBefore.tv_sec)*1000 + (sysAfter.tv_usec -sysBefore.tv_usec)*0.001);
    printf("the elapsed wall-clock time for the command to execute: %.2f\n", (double)(time(NULL) - wall_ini));
    printf("the number of times the process gave up the CPN voluntarily: %d\n", (int)usage.ru_nvcsw);
    printf("the number of major page faults (hardwarer): %d\n", (int)usage.ru_majflt);
    printf("the number of minor page faults (software): %d\n\n", (int)usage.ru_minflt);
}
