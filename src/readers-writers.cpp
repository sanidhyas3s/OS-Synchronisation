#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define ITERATIONS 10

sem_t check, rupdate, rwsync, print;
int rcount = 0;

int* shared = (int*)malloc(sizeof(int));

void* Reader(void* args)
{
    sem_wait(&print);
    int identity = *(int*)args;
    printf("Reader %d ready.\n",identity+1);
    sem_post(&print);
    sem_wait(&rwsync);
    sem_wait(&rupdate);
    rcount++;
    if(rcount==1) sem_wait(&check);
    sem_post(&rupdate);
    sem_post(&rwsync);
    //---READ DATA---
    sleep(1);
    printf("Value read from the memory by Reader %d: %d\n",identity+1,*shared);
    //---------------
    sem_wait(&rupdate);
    rcount--;
    sem_post(&rupdate);
    if(rcount==0) sem_post(&check);

    return NULL;
}

void* Writer(void* args)
{
    sem_wait(&print);
    int identity = *(int*)args;
    printf("Writer %d ready.\n",identity+1);
    sem_post(&print);
    sem_wait(&rwsync);
    sem_wait(&check);
    //---WRITE DATA---
    sleep(1);
    *shared = rand()%100;
    printf("Value written to the memory by Writer %d: %d\n",identity+1,*shared);
    //----------------
    sem_post(&check);
    sem_post(&rwsync);
    return NULL;  
}

int main()
{
    sem_init(&check,0,1);
    sem_init(&rupdate,0,1);
    sem_init(&rwsync,0,1);
    sem_init(&print,0,1);
    pthread_t read,write;
    for(int i=0;i<ITERATIONS;i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&read,NULL,Reader,identity);
        pthread_create(&write,NULL,Writer,identity);
    }
    for(int i=0;i<ITERATIONS;i++)
    {
        pthread_join(read,NULL);
        pthread_join(write,NULL);
    }
    return 1;
}