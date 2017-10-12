//CS3013 proj4
//Yuan Wang
//ywang19@wpi.edu
#include "thread.h"

void * worker(void *t)
{
    struct stat state_buffer;
    long tid;
    tid = (long)t;
    size_t FN_buf_size = 512;
    
    char * t_fn_buffer = malloc(sizeof(char) * FN_buf_size);//tread's own buffer storing file name for security sake
    
    strcpy(t_fn_buffer, FN_buffer_thread[tid % maxthread]);
    
    
    beginRegion(7);
    int tf = textfile(t_fn_buffer);
    endRegion(7);
    
    
    if(stat(t_fn_buffer, &state_buffer) == -1)//bad files
    {
        beginRegion(0);
        badfile_num ++;
        endRegion(0);
    }
    else
    {
        //test for file type
        //test if a regular file
        if(S_ISREG(state_buffer.st_mode) != 0)
        {
            beginRegion(2);
            regfile_num++;
            endRegion(2);
            beginRegion(4);
            reg_byte += state_buffer.st_size; //add the number of bytes
            endRegion(4);
            if(textfile)
            {
                beginRegion(5);
                reg_text ++;
                endRegion(5);
                beginRegion(6);
                text_byte += state_buffer.st_size;
                endRegion(6);
            }
        }
        //test if a directory
        else if(S_ISDIR(state_buffer.st_mode) != 0)
        {
            beginRegion(1);
            directory_num++;
            endRegion(1);
        }
        else
        { //else this is a special file
            beginRegion(3);
            spcfile_num++;
            endRegion(3);
        }
    }
    free(t_fn_buffer);
   
    pthread_exit(NULL);
}
