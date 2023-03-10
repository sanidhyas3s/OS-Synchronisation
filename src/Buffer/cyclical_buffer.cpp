#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#define ITERATIONS 25
#define BUFFER_SIZE 10


sem_t *check, *full, *empty, *print;

int buffer[BUFFER_SIZE];
int in=0,out=0;

void* Reader(void* args)
{
    for(int i=0;i<ITERATIONS;i++){
        sem_wait(print);
        printf("Reader ready %d\n",i+1);
        sem_post(print);

        sem_wait(full);
        sem_wait(check);

        sleep(1);
        //------------ Data Read---------
        printf("Value read from the memory %d at memory location %d\n",buffer[out],out+1);
        out= (out+1)%BUFFER_SIZE;
        //------------ Data Read---------

        sem_post(check);
        sem_post(empty);
    }

    return NULL;
}

void* Writer(void* args)
{
    for(int i=0;i<ITERATIONS;i++){
        sem_wait(print);
        printf("Writer ready %d\n",i+1);
        sem_post(print);
        
        sem_wait(empty);
        sem_wait(check);

        sleep(2);
        //------------ Data Written---------
        buffer[in] = rand()%100;
        printf("Value written to the memory %d at memory location %d\n",buffer[in],in+1);
        in = (in+1)%BUFFER_SIZE;
        //------------ Data Written---------

        sem_post(check);
        sem_post(full);
    }
    return NULL;  
}

int main()
{
    check = sem_open("check",O_CREAT, S_IRUSR|S_IWUSR, 1);
    full = sem_open("full",O_CREAT, S_IRUSR|S_IWUSR, 0);
    empty = sem_open("empty",O_CREAT, S_IRUSR|S_IWUSR, BUFFER_SIZE);
    print = sem_open("print",O_CREAT, S_IRUSR|S_IWUSR, 1);

    sem_unlink("check");
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("print");

    pthread_t read,write;

    pthread_create(&write,NULL,Writer,NULL);
    pthread_create(&read,NULL,Reader,NULL);
   
    pthread_join(read,NULL);
    pthread_join(write,NULL);

    return 1;
}