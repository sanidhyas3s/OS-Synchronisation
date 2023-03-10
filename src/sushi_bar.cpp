#include <bits/stdc++.h> 
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#define NO_OF_CUSTOMERS 18


int eating = 0, waiting = 0; 
sem_t *block, *mutex;

void* sushi_bar(void* args){
    int n;
    int identity = *(int*)args;
    sem_wait(mutex);
    sleep(1);
    if(eating == 5){
        waiting++;
        sem_post(mutex);
        printf("\tWaiting customer...%d\n", identity+1);
        sem_wait(block);
    }
    else{
        eating++;
        sem_post(mutex);
    }
    printf("Eating customer...%d\n", identity+1);
    sleep(3);
    sem_wait(mutex);
    eating--;
    printf("\t\tLeaving customer...%d\n", identity+1);
    if(eating == 0 && waiting){
        n = fmin(waiting, 5);
        waiting -= n;
        eating += n;
        for(int i = 0; i < n; i++){
            sem_post(block);
        }
    }
    sem_post(mutex);
    sleep(3);
    return NULL;
}

int main(){
    mutex = sem_open("mutex",O_CREAT, S_IRUSR|S_IWUSR, 1);
    block = sem_open("block",O_CREAT, S_IRUSR|S_IWUSR, 0);

    sem_unlink("mutex");
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