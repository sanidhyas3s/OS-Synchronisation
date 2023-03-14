#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <bits/stdc++.h>
#define NO_PHILOSPHERS 5
/*   0 -> eating   1 -> hungry   2 -> thinking*/
using namespace std;

int philosopher_i[NO_PHILOSPHERS];
int philosopher_state[NO_PHILOSPHERS]; 
int time_duration;

sem_t waiting_fork[NO_PHILOSPHERS];
/* 0-> waiting for fork    1-> not in waiting stage  */
sem_t mutexo;

void test_condition(int philosopher_number){
    if(philosopher_state[philosopher_number]==1&& philosopher_state[(philosopher_number+1)%NO_PHILOSPHERS]!=0&&philosopher_state[(philosopher_number+NO_PHILOSPHERS-1)%NO_PHILOSPHERS]!=0){
        philosopher_state[philosopher_number]=0;
        sleep(2);
        cout<< "Philosopher "<< philosopher_number+1<<" takes up fork number "<< (philosopher_number)%NO_PHILOSPHERS+1<< " and "<<(philosopher_number+1)%NO_PHILOSPHERS+1<<"\n";
        cout<< "The philosopher "<< philosopher_number+1<< " is eating\n";
        sem_post(&waiting_fork[philosopher_number]);
    }
} 

void if_hungry(int philosopher_number){
     sem_wait(&mutexo);
     philosopher_state[philosopher_number]=1;
     cout<<"philospher "<< philosopher_number+1<< " is hungry\n";
     test_condition(philosopher_number);
     sem_post(&mutexo);
     if(philosopher_state[philosopher_number]!= 0){
        cout<<"philospher "<< philosopher_number+1<< " is waiting for fork\n";
        sem_wait(&waiting_fork[philosopher_number]);
     }
     sleep(1);
}

void if_full(int philosopher_number){
    sem_wait(&mutexo);
    philosopher_state[philosopher_number]=2;
    cout<< "Philosopher "<< philosopher_number+1<< " is putting down fork and is thinking\n";
    test_condition((philosopher_number+NO_PHILOSPHERS-1)%NO_PHILOSPHERS);
    test_condition((philosopher_number+1)%NO_PHILOSPHERS);
    sem_post(&mutexo);
}



void* philosopher(void * philosopher_number){
    while(1){
        int i=*((int *) philosopher_number);
        sleep(1);
        if_hungry(i);
        sleep(0);
        if_full(i);
    }

    return NULL;
}

void initialize(){
     //name, option flag , permission flag, value
    sem_init(&mutexo, 0, 1);
    for(int i=0;i<NO_PHILOSPHERS;i++){
      philosopher_i[i]=i;
      sem_init(&waiting_fork[i],0,0);
      philosopher_state[i]=2;
    }
}

int main(){
    initialize();
    pthread_t  tid[NO_PHILOSPHERS];
    
    for(int i=0;i<NO_PHILOSPHERS;i++){
      pthread_create(&tid[i], NULL, philosopher, &philosopher_i[i]);
    }
    for(int i=0;i<NO_PHILOSPHERS;i++){
        pthread_join(tid[i], NULL); 
    }

    return 1;
}