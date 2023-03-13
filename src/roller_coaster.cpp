#include <bits/stdc++.h>
#include <pthread.h>
#include "semaphore.cpp"
#include <unistd.h>

using namespace std;

Semaphore allAboard = Semaphore(0);
Semaphore allAshore = Semaphore(0);
Semaphore boardQueue = Semaphore(0);
Semaphore unboardQueue = Semaphore(0);
Semaphore mutex1 = Semaphore(1);
Semaphore mutex2 = Semaphore(1);
Semaphore print = Semaphore(1);

const int Capacity = 2;
int boarders = 0;
int unboarders = 0;
int waiting_passengers = 7;

void load()
{
    print.wait();
    sleep(1);
    printf("Passengers can now board the Roller Coaster.\n");
    print.release();
}

void run()
{
    print.wait();
    printf("Roller Coaster is running.\n");
    sleep(10);
    print.release();
}

void unload()
{
    print.wait();
    printf("Passengers can now unboard the Roller Coaster.\n");
    print.release();
}

void board(int identity)
{
    print.wait();
    sleep(1);
    printf("Passenger %d is boarding the Roller Coaster.\n", identity);
    print.release();
}

void unboard(int identity)
{
    print.wait();
    sleep(1);
    printf("Passenger %d is unboarding the Roller Coaster.\n", identity);
    print.release();
}

void *RollerCoaster(void *args)
{
    int ride_count = 0;

    while (true)
    {
        load();
        boardQueue.release();
        allAboard.wait();
        run();
        ride_count++;
        print.wait();
        printf("Ride %d Finished.\n", ride_count);
        sleep(1);
        print.release();
        waiting_passengers -= Capacity;
        unload();
        for (int i = 0; i < Capacity; i++)
            unboardQueue.release();
        if (waiting_passengers < Capacity)
        {
            allAshore.wait();
            if (waiting_passengers > 0)
            {
                printf("%d Passangers are waiting for new Passengers to arrive.\n", waiting_passengers);
                boardQueue.wait();
            }

            else
                break;
        }
        allAshore.wait();
        boarders = 0;
        unboarders = 0;
    }
}

void *Passenger(void *args)
{
    int identity = *(int *)args;
    free(args);

    print.wait();
    sleep(2);
    printf("Passenger %d is waiting to board the Roller Coaster.\n", identity);
    print.release();

    while (true)
    {
        mutex1.wait();
        boardQueue.wait();
        if (boarders < Capacity)
        {
            board(identity);
            boarders++;
            if (boarders == Capacity)
            {
                allAboard.release();
            }
            else
                boardQueue.release();
            mutex1.release();
            unboardQueue.wait();
            mutex2.wait();
            unboard(identity);
            unboarders++;
            mutex2.release();
            if (unboarders == Capacity)
                allAshore.release();
            break;
        }
        mutex1.release();
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t Roller_Coaster_thread;
    pthread_t Passenger_threads[waiting_passengers];

    pthread_create(&Roller_Coaster_thread, NULL, RollerCoaster, NULL);

    for (int i = 0; i < waiting_passengers; i++)
    {
        int *identity = (int *)malloc(sizeof(int));
        *identity = i + 1;
        pthread_create(&Passenger_threads[i], NULL, Passenger, identity);
    }

    for (int i = 0; i < waiting_passengers; i++)
    {
        pthread_join(Passenger_threads[i], NULL);
    }

    pthread_join(Roller_Coaster_thread, NULL);

    return 0;
}
