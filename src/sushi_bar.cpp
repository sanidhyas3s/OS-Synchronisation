#include <bits/stdc++.h> 
#include <unistd.h>
#include <pthread.h>

#include "semaphore.cpp"

#define NO_OF_CUSTOMERS 16

int eating = 0, waiting = 0; 
Semaphore block = Semaphore(0);
Semaphore mutex = Semaphore(1);
Semaphore mutex1 = Semaphore(1);
bool must_wait = false;

void* sushi_bar(void* args){
    int identity = *(int*)args;
    (mutex).wait();
    sleep(1);
    if(must_wait){
        waiting++;
        printf("\tWaiting customer...%d\n", *(int*)args+1);
        mutex.release();
        block.wait();
        waiting--;
    }
    eating++;
    printf("Eating customer...%d\n", *(int*)args+1);
    if(eating == 5){
        must_wait = true;
    }
    if(waiting && !(must_wait)){
        block.release();
    }
    else{
        mutex.release();
    }

    sleep(6); // Eating time

    mutex1.wait();
    eating--;
    printf("\t\tLeaving customer...%d\n", *(int*)args+1);
    if(eating == 0){
        must_wait = false;
    }
    if(waiting && !(must_wait)){
        block.release();
    }
    mutex1.release();
    return NULL;
}

int main(){
    pthread_t customer[NO_OF_CUSTOMERS];
    for(int i = 0; i < NO_OF_CUSTOMERS; i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&customer[i],NULL,sushi_bar,identity);
    }
    for(int i = 0; i < NO_OF_CUSTOMERS; i++)
    {
        pthread_join(customer[i],NULL);
    }
}