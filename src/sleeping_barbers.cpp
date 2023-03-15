#include <bits/stdc++.h> 
#include <pthread.h>
#include <unistd.h>

#include "semaphore.cpp"
using namespace std;

#define NO_OF_BARBERS 3
#define NO_CHAIRS 3
#define MAX_CUSTOMERS 10


Semaphore barberReady = Semaphore(0);
Semaphore mutex1 = Semaphore(1);
Semaphore waitingseats = Semaphore(1);     
Semaphore ready = Semaphore(0);         
Semaphore IO= Semaphore(1);

int freeseats = NO_CHAIRS;  
int served_customers = 0;   

queue<int>q;

void* barber(void* args){
    int id = *(int*)args;
    while(1){ 
		waitingseats.wait();
        if (freeseats == NO_CHAIRS){
            IO.wait();
            cout<<"Barber "<<id<<" is going to sleep\n";
            IO.release();
			if (served_customers == MAX_CUSTOMERS){
				waitingseats.release(); 
				sleep(10);
				exit(0);
			}
        }
        waitingseats.release(); 

        ready.wait();                  
        waitingseats.wait();    
		     
        freeseats++;      
        served_customers++;
        int cust_id = q.front();
        q.pop();        

        IO.wait();        
        cout<<"\tBarber "<<id<<" is going to cut the hair of customer "<<cust_id<<"\n";
        IO.release();

        barberReady.release();        
        waitingseats.release(); 
 
        sleep(8); 	// cutting hair of customer

        IO.wait();        
        cout<<"\t\t\t\tBarber "<<id<<" finished cutting hair of customer "<<cust_id<<"\n";
        IO.release();	
    }
    return NULL;
}

void* customer(void* args){
    int id = *(int*)args;

    while(1){
        mutex1.wait();                       
        sleep(rand()%4);
        waitingseats.wait();           
        if (freeseats > 0){
            IO.wait();        
            cout<<"Customer "<<id<<" entered waiting area\n";
            IO.release();
            q.push(id);    
            freeseats--;                
            ready.release();
            waitingseats.release();
            mutex1.release();
            barberReady.wait(); 
            return NULL;    
        }
        else{
            IO.wait();    
            cout<<"\t\tCustomer "<<id<<" left without haircut\n"; 
            IO.release();
            waitingseats.release(); 
            mutex1.release();
            sleep(rand()%5);  
        }
    }
    return NULL;
}

int main(){    
    pthread_t barbers[NO_OF_BARBERS], customers[MAX_CUSTOMERS];
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
    for (int i=0;i<NO_OF_BARBERS;i++)
        pthread_join(barbers[i],NULL);      
    for (int i=0;i<MAX_CUSTOMERS;i++){
        pthread_join(customers[i],NULL);      
        sleep(rand()%2);
    }    
}
