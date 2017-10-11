/**
 * CS3013
 * Yuan Wang(ywang19)
 * utility.h
 * Header Files.
 * About this part, it is very similer to the producer and consumer module.
 */

#ifndef utility_h
#define utility_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <limits.h>

//extern global variblies fromt the main fucntion.
extern struct msg * mailbox;
extern sem_t * send_sem;
extern sem_t * rec_sem;
extern FILE *fp;
extern int nb;

struct msg
{
    int iFrom; //sender
    int value; //message value
    int cnt;   //count of operations (not needed by all msgs)
    int tot;   //total time (not needed by all msgs)
};

void set_msg(struct msg *m, int iFrom, int value, int cnt, int tot);
void SendMsg(int iTo, struct msg *pMsg);

int NBSendMsg(int iTo, struct msg *pMsg);
void RecvMsg(int iFrom, struct msg *pMsg);

void main_thread(void * NUM_THREADS);
void adder(void *threadid);
void MailBox();//initialize the mailbox

/**
 * Stack Node
 */
struct StackNode//StackNode for the nb section.
{
    int iTo;
    int value;
    struct StackNode* next;
};
struct StackNode* newNode(int iTo, int value);
int isEmpty(struct StackNode *root);
void push(struct StackNode** root, int iTo, int value);
int pop(struct StackNode** root);
int peek_iTo(struct StackNode* root);
int peek_value(struct StackNode* root);


#endif /* utiity_h */
