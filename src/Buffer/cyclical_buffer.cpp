#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

#include "../semaphore.cpp"

#define ITERATIONS 8
#define BUFFER_SIZE 4

Semaphore check = Semaphore(1);
Semaphore full = Semaphore(0);
Semaphore print = Semaphore(1);
Semaphore empty = Semaphore(BUFFER_SIZE);

int buffer[BUFFER_SIZE];
int in=0,out=0;

void* Consumer(void* args)
{
    int identity = *(int*)args;
    sleep(identity%3);
    print.wait();
    printf("Consumer ready %d\n",identity+1);
    print.release();

    full.wait();
    check.wait();
    
    //------------ Data Read---------
    printf("Consumer %d Thread: Value read from the memory %d at memory location %d\n",identity+1,buffer[out],out+1);
    out=(out+1)%BUFFER_SIZE;
    //-------------------------------

    check.release();
    empty.release();

    return NULL;
}

void* Producer(void* args)
{
    int identity = *(int*)args;
    sleep(identity%3);
    print.wait();
    printf("Producer ready %d\n",identity+1);
    print.release();
    
    empty.wait();
    check.wait();

    sleep(1);
    //------------ Data Written---------
    buffer[in] = rand()%100; 
    printf("Producer %d Thread: Value written to the memory %d at memory location %d\n",identity+1,buffer[in],in+1); 
    in=(in+1)%BUFFER_SIZE;
    //-------------------------------

    check.release();
    full.release();

    return NULL;  
}

int main()
{
    pthread_t cons,prod;
    for(int i=0;i<ITERATIONS;i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&cons,NULL,Consumer,identity);
        pthread_create(&prod,NULL,Producer,identity);
    }
    for(int i=0;i<ITERATIONS;i++)
    {
        pthread_join(prod,NULL);
        pthread_join(cons,NULL);
    }

    return 1;
}