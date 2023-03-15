#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <ctime>
#include <cassert>
#include <complex>
#include <string>
#include <cstring>
#include <chrono>
#include <random>
#include <bitset>
#include <array>


#define TOTAL_BARBERS 3
#define WAITING_CHAIRS 5
#define MAX_CUSTOMERS 20

using namespace std;
sem_t *waiting_room;
sem_t *barber_chair;
sem_t *mutexo;
sem_t *io;

queue<int>q;

// void initialize(){
     //name, option flag , permission flag, value

    // sem_init(&mutexo, 1, 1);
    // sem_init(&waiting_room, 1, 1);
    // sem_init(&barber_chair, 1, 1);
    // sem_init(&io, 1, 1);

    // mutexo = sem_open("mutexo", O_CREAT, S_IRUSR|S_IWUSR, 1);
    // waiting_room = sem_open("waiting_room", O_CREAT, S_IRUSR|S_IWUSR, 1);
    // barber_chair = sem_open("barber_chair", O_CREAT, S_IRUSR|S_IWUSR, 1);
    // io = sem_open("io", O_CREAT, S_IRUSR|S_IWUSR, 1);

    // sem_unlink("mutexo");
    // sem_unlink("waiting_room");
    // sem_unlink("barber_chair");
    // sem_unlink("io");
// }
bool barbers_awake[TOTAL_BARBERS];
int available_barbers = TOTAL_BARBERS;
void* barber(void* args){
	int id = *(int*)args;
	for (int i=0;i<10;i++){
		sem_wait(waiting_room);		
		// cout<<"barber id -- "<<id<<'\n';
		if (barbers_awake[id] && q.size() == 0){
			sem_wait(io);
			cout<<"Barber "<<id+1<<" goes to sleep\n";
			sem_post(io);
			barbers_awake[id] = 0;
			available_barbers++;
			sem_post(waiting_room);		
		}
		else if (q.size() > 0){
			if (!barbers_awake[id]){
				available_barbers--;
				sem_wait(io);
				cout<<"Barber "<<id+1<<" wakes up\n";
				sem_post(io);
				barbers_awake[id] = 1;
			}
			int x = q.front(); q.pop();
			sem_wait(io);
			cout<<"Barber "<<id+1<<" calls customer "<<x+1<<'\n';
			sem_post(io);
			sem_post(waiting_room);
			sleep(2);
			sem_wait(io);
			cout<<"Barber "<<id+1<<" finishes customer "<<x+1<<" and customer "<<x+1<<" leaves\n";
			sem_post(io);
		}
		else sem_post(waiting_room);


	}
	return NULL;	
}

void* customer(void* args){
	int id = *(int*)args;	
	for (int i=0;i<5;i++){
		sem_wait(waiting_room);	
		// cout<<"id -- "<<id<<'\n';
		if (q.size() < WAITING_CHAIRS){
			q.push(id);
			sem_wait(io);
			cout<<"Customer "<<id+1<<" entered waiting room\n";
			sem_post(io);
			sem_post(waiting_room);	
			return NULL;
		}
		else if (available_barbers == 0){
			sem_wait(io);
			cout<<"Customer "<<id+1<<" left without hair cut\n";
			sem_post(io);
			sem_post(waiting_room);	
			return NULL;
		}
		sem_post(waiting_room);	
		sleep(0.25);
	}	
	return NULL;	
}

int main(){    
	// cout<<"P\n";
	// initialize();

    mutexo = sem_open("mutexo", O_CREAT, S_IRUSR|S_IWUSR, 1);
    waiting_room = sem_open("waiting_room", O_CREAT, S_IRUSR|S_IWUSR, 1);
    barber_chair = sem_open("barber_chair", O_CREAT, S_IRUSR|S_IWUSR, 1);
    io = sem_open("io", O_CREAT, S_IRUSR|S_IWUSR, 1);

    sem_unlink("mutexo");
    sem_unlink("waiting_room");
    sem_unlink("barber_chair");
    sem_unlink("io");

    pthread_t barbers[TOTAL_BARBERS];
    pthread_t customers[MAX_CUSTOMERS];
    for(int i = 0; i < TOTAL_BARBERS; i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&barbers[i],NULL,barber,identity);
    }
    for(int i = 0; i < MAX_CUSTOMERS; i++)
    {
        int* identity = (int*)malloc(sizeof(int));
        *identity = i;
        pthread_create(&customers[i],NULL,customer,identity);
    }

    for (int i=0;i<TOTAL_BARBERS;i++)
        pthread_join(barbers[i],NULL);      
    for (int i=0;i<MAX_CUSTOMERS;i++){
        pthread_join(customers[i],NULL);      
        sleep(rand()%2+1);
    }
	return 0;
	    
}
