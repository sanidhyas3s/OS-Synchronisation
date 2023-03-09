#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

sem_t allAboard, allAshore, boardQueue, unboardQueue, mutex1, mutex2;
sem_t print;

const int Capacity = 6;
int boarders = 0;
int unboarders = 0;
int waiting_passengers = 22; 

void load() {
  sem_wait(&print);
  cout << "Passengers can now board the Roller Coaster." << endl;
  sem_post(&print);
}

void run() {
  sem_wait(&print);
  cout << "Roller Coaster is running." << endl;
  sem_post(&print);
}

void unload() {
  sem_wait(&print);
  cout << "Passengers can now unboard the Roller Coaster." << endl;
  sem_post(&print);
}

void board(int identity) {
  sem_wait(&print);
  cout << "Passenger " << identity << " is boarding the Roller Coaster." << endl;
  sem_post(&print);
}

void unboard(int identity) {
  sem_wait(&print);
  cout << "Passenger " << identity << " is unboarding the Roller Coaster." << endl;
  sem_post(&print);
}

void* RollerCoaster(void* args) {
  int ride_count = 0;


  while (true) {
    load();
    sem_post(&boardQueue);
    sem_wait(&allAboard);
    run();
    ride_count++;
    sem_wait(&print);
    cout<<"Ride "<<ride_count<<" Finished."<<endl;
    waiting_passengers -=Capacity;
    sem_post(&print);
    unload();
    for(int i = 0; i<Capacity; i++)
    sem_post(&unboardQueue);
    sem_wait(&allAshore);
    boarders = 0;
    unboarders = 0;

    if(waiting_passengers<Capacity)
    break;
  }
}

void* Passenger(void* args) {
  int identity = *(int*)args;
  free(args);

  sem_wait(&print);
  cout << "Passenger " << identity << " is waiting to board the Roller Coaster." << endl;
  sem_post(&print);

  while (true) {
    sem_wait(&mutex1);
    sem_wait(&boardQueue);
    if (boarders < Capacity) {
      board(identity);
      boarders++;
      if (boarders == Capacity) {
        sem_post(&allAboard);
      }
      else sem_post(&boardQueue);
      sem_post(&mutex1);
      sem_wait(&unboardQueue);
      sem_wait(&mutex2);
      unboard(identity);
      unboarders++;
      sem_post(&mutex2);
      if (unboarders == Capacity) 
      sem_post(&allAshore);
      break;
    }
    sem_post(&mutex1);
    sleep(50000);
  }

  pthread_exit(NULL);
}

int main() {
  sem_init(&boardQueue, 0, 0);
  sem_init(&unboardQueue, 0, 0);
  sem_init(&allAboard, 0, 0);
  sem_init(&allAshore, 0, 0);
  sem_init(&mutex1, 0, 1);
  sem_init(&mutex2, 0, 1);
  sem_init(&print, 0, 1);

  pthread_t Roller_Coaster_thread;
  pthread_t Passenger_threads[waiting_passengers];

  pthread_create(&Roller_Coaster_thread, NULL, RollerCoaster, NULL);

  for (int i = 0; i < waiting_passengers; i++) {
    int* identity = (int*)malloc(sizeof(int));
    *identity = i + 1;
    pthread_create(&Passenger_threads[i], NULL, Passenger, identity);
  }

  for (int i = 0; i < waiting_passengers; i++) {
    pthread_join(Passenger_threads[i], NULL);
  }

  pthread_join(Roller_Coaster_thread, NULL);

  sem_destroy(&boardQueue);
  sem_destroy(&unboardQueue);
  sem_destroy(&allAboard);
  sem_destroy(&allAshore);
  sem_destroy(&mutex1);
  sem_destroy(&mutex2);

  return 0;
}
