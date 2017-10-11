/**
 * CS3013
 * Yuan Wang(ywang19)
 * utility.c
 * About this part, it is very similer to the producer and consumer module.
 * include the set-msg, Send, Rec-sem, main thread, add child thread, stack structure
 */

#include "utility.h"
#define line_buffer_size 64

/* iTo - mailbox to send to */
/* pMsg - message to be sent */

//set-msg Section
void set_msg(struct msg *m, int iFrom, int value, int cnt, int tot)
{
    m->iFrom = iFrom;
    m->value = value;
    m->cnt = cnt;
    m->tot = tot;
}

//SendMsg Section
void SendMsg(int iTo, struct msg *pMsg)
{

    sem_wait(&send_sem[iTo]);  //wait until it receive the instruction
    mailbox[iTo].iFrom = pMsg->iFrom;
    mailbox[iTo].value = pMsg->value;
    mailbox[iTo].cnt = pMsg->cnt;
    mailbox[iTo].tot = pMsg->tot;
    sem_post(&rec_sem[iTo]);
    
}
//NBSendMsg section
int NBSendMsg(int iTo, struct msg *pMsg)
{
    int s; //counter
    sem_getvalue(&send_sem[iTo], &s);
    
    sem_trywait(&send_sem[iTo]);
    if(s == 0) //Already has a message
    {
        return -1;
    }
    mailbox[iTo].iFrom = pMsg->iFrom;
    mailbox[iTo].value = pMsg->value;
    mailbox[iTo].cnt   = pMsg->cnt;
    mailbox[iTo].tot   = pMsg->tot;
    sem_post(&rec_sem[iTo]);
    return 1;
}

void RecvMsg(int iFrom, struct msg *pMsg)
{
    sem_wait(&rec_sem[iFrom]);//wait to receive the message
    if(mailbox[iFrom].value < 0)//termination msg
    {
       return;
    }
    //handling msg value
    if(mailbox[iFrom].value > 0)//When its value is non-negative, it will counts
    {
        pMsg->cnt ++;
    }
    pMsg->value += mailbox[iFrom].value; //value = value + mailbox's value
    set_msg(&mailbox[iFrom], -1, 0, 0, 0);//clear the mailbox
    sem_post(&send_sem[iFrom]);
    sleep(1);
}

//routine for main thread0
void main_thread(void * NUM_THREADS)
{
   
    
  
    long i;
    int routine;
    struct StackNode* root = NULL; //waiting to send the queue.
    
    char * line = malloc(sizeof(char *) * line_buffer_size);
    char ** line_arguments =  malloc(sizeof(char **) * line_buffer_size); //input arguments from file test.txt
    struct msg current_msg;
    struct msg terminte_msg; //termination msg
    void *status;
    
    //create child threads
    pthread_t threads[(long)NUM_THREADS]; //array of child threads
    for(i=0; i<(long)NUM_THREADS; i++)
    {
        printf("--------------------------------\n");
        printf("| ATTENTION!! CREATING THERAD %ld |\n", i+1);
        printf("--------------------------------\n");
        routine = pthread_create(&threads[i], NULL, adder, (void *)(i+1));
        if (routine)
        {
            printf("ERROR!\n");
            exit(-1);
        }
    }
    
    //initialize global semaphores
    send_sem = malloc(sizeof(sem_t) * (long)NUM_THREADS);
    rec_sem = malloc(sizeof(sem_t) * (long)NUM_THREADS);
    
    for(i = 0; i < (long)NUM_THREADS; i ++)
    {
        sem_init(&send_sem[i], 1, 1);
    }
    for(i = 0; i < (long)NUM_THREADS; i ++)
    {
        sem_init(&rec_sem[i], 1, 0);
    }
    
    
    i = 0;
    while(fgets(line, line_buffer_size, fp) != NULL)
    {
        i ++;
        line_arguments = parse(line);
        
        //conditions to check child threads
        if(line_arguments[0] == NULL)//NULL VALUE
        {
            printf("ERROR!");
            break;
        }
        else if(line_arguments[1] == NULL)//only one argument
        {
            printf("ERROR!");
            break;
        }
        if(atoi(line_arguments[1]) < 0)//negative thread index
        {
            printf("ERROR!");
            break;
        }
        if(atoi(line_arguments[0]) == 0)// 0
        {
            printf("ERROR!");
            break;
        }
        if(atoi(line_arguments[1]) > (long)NUM_THREADS)//out of bound
        {
            printf("ERROR!");
            break;
        } //Terminate CHILD threads
        
        //creating msg based on inputs
        set_msg(&current_msg, 0, atoi(line_arguments[0]), 0, 0);//send msg
        if(nb)
        {
            if(NBSendMsg(atoi(line_arguments[1]) - 1, &current_msg) == -1)
            {
                push(&root, atoi(line_arguments[1]) - 1 ,current_msg.value);
            }
        }
        else SendMsg(atoi(line_arguments[1]) - 1, &current_msg);//index - 1 = real index number
        
    }
    
    //NB ENABLED: resent buffered msg
    if(nb)
    {
        while(!isEmpty(root))
        {
            set_msg(&current_msg, 0, peek_value(root), 0, 0);
            if(NBSendMsg(peek_iTo(root), &current_msg) != -1)
            {
                pop(&root);
                
            }
        }
    }
    
    //send termination msg
    set_msg(&terminte_msg, 0, -1, 0, 0);
    for(i=0; i<(long)NUM_THREADS; i++)
    {
        SendMsg(i, &terminte_msg);
    }
    
    
    //pthread join
    for(i=0; i<(long)NUM_THREADS; i++)
    {
        routine = pthread_join(threads[i], &status);
        
        if (routine)
        {
            printf("ERROR!!\n");
            exit(-1);
        }
    }
    
    for(i = 0; i < (long)NUM_THREADS; i++)
    {
        printf("The result from thread %ld is %d from %d operations during %d secs. \n", i + 1, mailbox[i].value, mailbox[i].cnt, mailbox[i].tot);
    }
    
    //sem_destroy
    for(i = 0; i < (long)NUM_THREADS; i ++)
    {
        sem_destroy(&send_sem[i]);
    }
    for(i = 0; i <(long)NUM_THREADS; i ++)
    {
        sem_destroy(&rec_sem[i]);
    }
    
    pthread_exit(NULL); //exit
}

//routine for child thread
void adder(void * threadid)
{
    
    //timer variables
    time_t c1;
    time_t c2;
    c1 = time(NULL);
    
    struct msg ret_msg;
    printf("================================\n");
    printf("||Children thread #%ld created.||\n", (long)threadid);
    printf("================================\n");
    //receiving msg
    while(mailbox[(long)threadid-1].value >= 0)
    {
        if(mailbox[(long)threadid-1].value < 0)//break on termination
        {
            break;
        }
        
        RecvMsg((long)threadid-1, &ret_msg);//fill in the return msg
        
    }
    
    
    c2 = time(NULL);
    ret_msg.tot = (long)(c2 - c1);
    
    sem_post(&send_sem[(long)threadid-1]);
    SendMsg((long)threadid-1, &ret_msg); //After termination, sending back to the informaiton.
    pthread_exit(NULL);
}

//Initilise the mainbox
void MailBox()
{
    int i;
    for(i = 0; i < (sizeof(mailbox)/sizeof(struct msg)); i ++)
    {
        set_msg(&mailbox[i], -1, 0, 0, 0);
    }
    pthread_exit(NULL); //exit
}


/**
 * Stack Structure holding for the additonal nb section
 * put the thread into the stack value, using LIFO
 */

struct StackNode* newNode(int iTo, int value)
{
    struct StackNode* stackNode = (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->iTo = iTo;
    stackNode->value = value;
    stackNode->next = NULL;
    return stackNode;
}

//check whether the it is empty or not.
int isEmpty(struct StackNode *root)
{
    return !root;
}

//push the value to the StackNode
void push(struct StackNode** root, int iTo, int value)
{
    struct StackNode* stackNode = newNode(iTo, value);
    stackNode->next = *root;
    *root = stackNode;
    
}

//pop up fromt the Stack.
int pop(struct StackNode** root)
{
    if (isEmpty(*root))
        return INT_MIN;
    struct StackNode* temp = *root;
    *root = (*root)->next;
    int popped_iTo = temp->iTo;
    free(temp);
    
    return popped_iTo;
}

// peek iTo section
int peek_iTo(struct StackNode* root)
{
    if (isEmpty(root))
    {
        printf("ERROR!\n");
        return INT_MIN;
    }
    return root->iTo;
}

//peek value section
int peek_value(struct StackNode* root)
{
    if (isEmpty(root))
    {
        printf("ERROR!\n");
        return INT_MIN;
    }
    return root->value;
}


