#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

#include "../semaphore.cpp"

#define ITERATIONS 7

Semaphore check = Semaphore(1);
Semaphore full = Semaphore(0);
Semaphore print = Semaphore(1);

std:: vector<int> buffer;
int in=0,out=0;

void* Consumer(void* args)
{
    int identity = *(int*)args;
    sleep(identity%4);
    print.wait();
    printf("Consumer ready %d\n",identity+1);
    print.release();

    full.wait();
    check.wait();
    
    //------------ Data Read---------
    printf("Consumer %d Thread: Value read from the memory %d at memory location %d\n",identity+1,buffer[out],out+1);
    out++;
    //-------------------------------

    check.release();

    return NULL;
}

void* Producer(void* args)
{
    int identity = *(int*)args;
    sleep(identity%4);
    print.wait();
    printf("Producer ready %d\n",identity+1);
    print.release();
    
    check.wait();

    sleep(1);
    //------------ Data Written---------
    buffer.push_back(rand()%100); 
    printf("Producer %d Thread: Value written to the memory %d at memory location %d\n",identity+1,buffer[in],in+1); 
    in++;
    //----------------------------------

    check.release();
    full.release();

    return NULL;  
}

int main()
{
    pthread_t cons[ITERATIONS],prod[ITERATIONS];
    for(int i=0;i<ITERATIONS;i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&cons[i],NULL,Consumer,identity);
        pthread_create(&prod[i],NULL,Producer,identity);
    }
    for(int i=0;i<ITERATIONS;i++)
    {
        pthread_join(cons[i],NULL);
        pthread_join(prod[i],NULL);
    }

    return 1;
}