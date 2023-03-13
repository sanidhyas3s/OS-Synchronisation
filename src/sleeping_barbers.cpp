#include <bits/stdc++.h> 
#include "Brutel5.cpp"
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define MAX_CUSTOMERS 50
#define NO_OF_BARBERS 5
#define WAITING_CHAIRS 10

int waiting_customers = 0;
sem_t wait, block;
// semaphores wait,block;

queue<int>customers;
void* customer(void* args){
	int identity = *(int*)args;
	sem_wait(&mutex);
	sleep(1);
	if (waiting_customers < WAITING_CHAIRS){
		waiting_customers++;
		customers.push(id);		
		sem_signal(&mutex);		
		cout<<"customer "<<identity+1<<" entered waiting area\n";
	} 
	else{				
		sem_signal(&mutex);
		cout<<"customer "<<identity+1<<" left without hair cut\n";
	}
	
}
bool barbers_state[NO_OF_BARBERS];
void* barber(void* args){
	int identity = *(int*)args;
	sem_wait(&block);
	sleep(1);
	if (waiting_customers == 0){		
		barbers_state[id] = 1;
		sem_signal(&block);
		cout<<"Barber "<<identity+1<<" going to sleep\n";
	}
	else{		
		waiting_customers--;
		int customer_id = customers.front();
		customers.pop();
		sem_signal(&block);
		if (barbers_state[id])
			cout<<"Barber "<<identity+1<<" wake up from sleep\n";
		barbers_state[identity+1] = 0;		
		sleep(2);
		cout<<"Barber "<<identity+1<<" is going to cut the hairs of customer "<<customer_id<<'\n';
		cout<<"customer "<<customer_id<<" is leaving after his hair cut\n";		
	}
}

int main(){

	sem_init(&block, 0, 1);
	sem_init(&mutex, 0, 1);

	pthread_t barbers[NO_OF_BARBERS];
	pthread_t customers[MAX_CUSTOMERS];
    for(int i = 0; i < NO_OF_BARBERS; i++)
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
    for(int i = 0; i < NO_OF_BARBERS; i++)
    {
        pthread_join(barbers[i],NULL);
    }
    for(int i = 0; i < MAX_CUSTOMERS; i++)
    {
        pthread_join(customers[i],NULL);
    }

    sem_destroy(&block);
    sem_destroy(&mutex);
}








