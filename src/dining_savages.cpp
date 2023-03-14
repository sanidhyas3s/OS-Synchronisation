#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include "semaphore.cpp"

#define MAX_SAVAGES 5
#define SERVINGS 3
#define ITERATIONS 10
#define MAX_REFILLS 2

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
        while(servings == 0)
        {
            print.wait();
            printf("Savage %d is waiting for the pot to be refilled.\n", id);
            sleep(1);
            print.release();
            pot_mutex.release();
            pot_empty.wait();
        }

        print.wait();
        servings--;
        printf("Savage %d took a serving from the pot. Servings left: %d\n", id, servings);
        print.release();
        sleep(1);

        if (servings == 0)
        {
            print.wait();
            printf("Savage %d signaled the cook to refill the pot.\n", id);
            print.release();
            sleep(1);
            pot_full.release();
        }

        pot_mutex.release();

        sleep(10);
    }

    return NULL;
}

void *Cook(void *arg)
{
    while (1)
    {
        pot_mutex.wait();

        while (servings > 0)
        {
            sleep(1);
            pot_mutex.release();
            pot_full.wait();
        }

        if (refill_count >= MAX_REFILLS)
        {
            print.wait();
            printf("Cook denied refill as already they have exhausted all refills.");
            print.release();
            exit(0);
        }
        print.wait();
        servings = SERVINGS;
        printf("Cook refilled the pot. Servings left: %d\n", servings);
        refill_count++;
        print.release();

        sleep(1);
        pot_empty.release();
        pot_mutex.release();
    }

    return NULL;
}

int main()
{
    int i;
    pthread_t savages[MAX_SAVAGES];
    pthread_t cook_thread;
    int identity[MAX_SAVAGES];

    pthread_create(&cook_thread, NULL, Cook, NULL);

    for (i = 0; i < MAX_SAVAGES; i++)
    {
        identity[i] = i+1;
        pthread_create(&savages[i], NULL, Savage, &identity[i]);
    }

    for (i = 0; i < MAX_SAVAGES; i++)
    {
        pthread_join(savages[i], NULL);
    }
    pthread_join(cook_thread, NULL);

    return 0;
}
