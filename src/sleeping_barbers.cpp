#include <bits/stdc++.h> 
#include "semaphore.cpp"
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define MAX_CUSTOMERS 10
#define NO_OF_BARBERS 3
#define WAITING_CHAIRS 5
#define MAX_ITERATIONS 10

int waiting_customers = 0;
Semaphore block = Semaphore(1);
Semaphore mutex1 = Semaphore(1);
Semaphore S = Semaphore(1);
queue<int>customers;
void* customer(void* args){
	int identity = *(int*)args;
	mutex1.wait();
	sleep(1);
	if (waiting_customers < WAITING_CHAIRS){
		S.wait();
		waiting_customers++;
		customers.push(identity+1);		
		printf("customer %d entered waiting area\n", identity+1);
		S.release();
		mutex1.release();
				
	} 
	else{				
		printf("customer %d left without hair cut\n", identity+1);
		mutex1.release();
		
	}
	return NULL;
}
bool barbers_state[NO_OF_BARBERS];
void* barber(void* args){

	for (int i=0;i<MAX_ITERATIONS;i++){
	int identity = *(int*)args;
	block.wait();
	sleep(1);
	if (waiting_customers == 0){				
		block.release();
		if (!barbers_state[identity+1]){
			barbers_state[identity+1] = 1;
			printf("Barber %d going to sleep\n", identity+1);	
		}
		
	}
	else{		
		S.wait();
		waiting_customers--;
		int customer_id = customers.front();
		customers.pop();
		S.release();
		if (barbers_state[identity+1])
			printf("Barber %d wake up from sleep\n", identity+1);
		barbers_state[identity+1] = 0;		
		
		cout<<"Barber "<<identity+1<<" is going to cut the hairs of customer "<<customer_id<<'\n';
		sleep(5);
		cout<<"customer "<<customer_id<<" is leaving after his hair cut\n";
		
		block.release();
				
	}}
	return NULL;
}

int main(){

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
        sleep(rand()%2);
    }
}
