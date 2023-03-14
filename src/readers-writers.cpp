#include <bits/stdc++.h>
#include <unistd.h>
#include <pthread.h>

#include "semaphore.cpp"

#define ITERATIONS 15

Semaphore check = Semaphore(1);
Semaphore rupdate = Semaphore(1);
Semaphore rwsync = Semaphore(1);
Semaphore print = Semaphore(1);

int rcount = 0;

int* shared = (int*)malloc(sizeof(int));

void* Reader(void* args)
{
    int identity = *(int*)args;
    sleep(identity%5);
    print.wait();
    printf("Reader %d ready.\n",identity+1);
    print.release();
    rwsync.wait();
    rupdate.wait();
    rcount++;
    if(rcount==1) check.wait();
    rupdate.release();
    rwsync.release();
    //---READ DATA---
    sleep(1);
    printf("Value read from the memory by Reader %d: %d\n",identity+1,*shared);
    //---------------
    rupdate.wait();
    rcount--;
    rupdate.release();
    if(rcount==0) check.release();

    return NULL;
}

void* Writer(void* args)
{
    int identity = *(int*)args;
    sleep(identity%5);
    print.wait();
    printf("Writer %d ready.\n",identity+1);
    print.release();
    rwsync.wait();
    check.wait();
    //---WRITE DATA---
    sleep(1);
    *shared = rand()%100;
    printf("Value written to the memory by Writer %d: %d\n",identity+1,*shared);
    //----------------
    check.release();
    rwsync.release();
    return NULL;  
}

int main()
{
    pthread_t read[ITERATIONS],write[ITERATIONS];
    for(int i=0;i<ITERATIONS;i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&read[i],NULL,Reader,identity);
        pthread_create(&write[i],NULL,Writer,identity);
    }
    for(int i=0;i<ITERATIONS;i++)
    {
        pthread_join(read[i],NULL);
        pthread_join(write[i],NULL);
    }
    return 1;
}
