#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

#include "semaphore.cpp"

#define MAX_SAVAGES 4
#define SERVINGS 5
#define MAX_REFILLS 3

Semaphore pot_mutex = Semaphore(1);
Semaphore pot_empty = Semaphore(0);
Semaphore pot_full = Semaphore(0);
Semaphore print = Semaphore(1);

int servings = 0;
int refill_count = 0;

void *Savage(void *arg)
{
    int id = *((int *)arg);

    while(1)
    {
        pot_mutex.wait();
        if (servings == 0)
        {
            print.wait();
            printf("Savage %d is waiting for the pot to be refilled.\n", id);
            print.release();
            pot_empty.release();
            pot_full.wait();
        }

        if(servings>0)
        {
            servings--;
            print.wait();
            printf("Savage %d took a serving from the pot. Servings left: %d\n", id, servings);
            print.release();
        }
        pot_mutex.release();
        
    }
    

    return NULL;
}

void *Cook(void *arg)
{
    while(1)
    {
        pot_empty.wait();
        if(refill_count>=MAX_REFILLS)
        {
            print.wait();
            printf("Cook denied refill as already they have exhausted all refills.");
            exit(0);
        }

        if(servings == 0)
        {
            servings = SERVINGS;
            refill_count++;
            print.wait();
            printf("Cook refilled the pot. Servings left: %d\n", servings);
            print.release();
        }
        pot_full.release();

        
    }

    return NULL;
}

int main()
{
    pthread_t savages[MAX_SAVAGES];
    pthread_t cook_thread;
    int identity[MAX_SAVAGES];

    pthread_create(&cook_thread, NULL, Cook, NULL);

    for (int i = 0; i < MAX_SAVAGES; i++)
    {
        identity[i] = i + 1;
        pthread_create(&savages[i], NULL, Savage, &identity[i]);
    }

    for (int i = 0; i < MAX_SAVAGES; i++)
    {
        pthread_join(savages[i], NULL);
    }
    pthread_join(cook_thread, NULL);

    return 0;
}
