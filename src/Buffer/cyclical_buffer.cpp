#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

#include "../semaphore.cpp"

#define ITERATIONS 7
#define BUFFER_SIZE 4

Semaphore check = Semaphore(1);
Semaphore full = Semaphore(0);
Semaphore print = Semaphore(1);
Semaphore empty1 = Semaphore(BUFFER_SIZE);

int buffer[BUFFER_SIZE];
int in=0,out=0;

void* Consumer(void* args)
{
    int identity = *(int*)args;
    sleep(rand()%3);
    print.wait();
    printf("Consumer ready %d\n",identity+1);
    print.release();

    full.wait();
    check.wait();
    
    //------------ Data Read---------
    print.wait();
    printf("Consumer %d Thread: Value read from the memory %d at memory location %d\n",identity+1,buffer[out],out+1);
    print.release();
    out=(out+1)%BUFFER_SIZE;
    //-------------------------------

    check.release();
    empty1.release();

    return NULL;
}

void* Producer(void* args)
{
    int identity = *(int*)args;
    sleep(rand()%3);
    print.wait();
    printf("Producer ready %d\n",identity+1);
    print.release();
    
    empty1.wait();
    check.wait();

    sleep(1);
    //------------ Data Written---------
    buffer[in] = rand()%100; 
    print.wait();
    printf("Producer %d Thread: Value written to the memory %d at memory location %d\n",identity+1,buffer[in],in+1);
     print.release(); 
    in=(in+1)%BUFFER_SIZE;
    //----------------------------------

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