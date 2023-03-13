#include <bits/stdc++.h> 
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#define NO_OF_CUSTOMERS 25


int eating = 0, waiting = 0; 
// Semaphore block = Semaphore(0);
// Semaphore mutex = Semaphore(1);
// Semaphore mutex1 = Semaphore(1);
sem_t *block, *mutex, *mutex1;
bool must_wait = false;

void* sushi_bar(void* args){
    int identity = *(int*)args;
    sem_wait(mutex);
    sleep(2);
    if(must_wait){
        waiting++;
        printf("\tWaiting customer...%d\n", *(int*)args+1);
        sem_post(mutex);
        sem_wait(block);
        waiting--;
    }

    eating++;
    printf("Eating customer...%d\n", *(int*)args+1);
    if(eating == 5){
        must_wait = true;
    }
    if(waiting && !(must_wait)){
        sem_post(block);
    }
    else{
        sem_post(mutex);
    }

    sleep(12);

    sem_wait(mutex1);
    eating--;
    printf("\t\tLeaving customer...%d\n", *(int*)args+1);
    if(eating == 0){
        must_wait = false;
    }
    if(waiting && !(must_wait)){
        sem_post(block);
        sem_post(mutex1);
    }
    else{
        sem_post(mutex1);
    }
    return NULL;
}

int main(){
    mutex = sem_open("mutex",O_CREAT, S_IRUSR|S_IWUSR, 1);
    mutex1 = sem_open("mutex1",O_CREAT, S_IRUSR|S_IWUSR, 1);
    block = sem_open("block",O_CREAT, S_IRUSR|S_IWUSR, 0);

    sem_unlink("mutex");
    sem_unlink("mutex1");
    sem_unlink("block");

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