#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

#include "semaphore.cpp"

using namespace std;
#define NO_PHPRS 5
#define NO_TIMES_HUNGRY 2

int ph_i[NO_PHPRS];
int ph_state[NO_PHPRS]; //0 -> Eating   1 -> Hungry   2 -> Thinking

Semaphore waiting_fork[NO_PHPRS];
Semaphore mutex1 = Semaphore(1);
Semaphore IO = Semaphore(1);

void test_condition(int p_no){
    if(ph_state[p_no]==1&& ph_state[(p_no+1)%NO_PHPRS]!=0&&ph_state[(p_no+NO_PHPRS-1)%NO_PHPRS]!=0){
        ph_state[p_no]=0;
        //------Eating------
        IO.wait();
        cout<<"\tPhilosopher "<<p_no+1<<" takes up fork number "<< (p_no)%NO_PHPRS+1<<" and "<<(p_no+1)%NO_PHPRS+1<<".\n";
        cout<<"\tPhilosopher "<<p_no+1<<" is eating.\n";
        IO.release();
        sleep(2);
        //------------------
        waiting_fork[p_no].release();
    }
} 

void if_hungry(int p_no){
     mutex1.release();
     ph_state[p_no]=1;
     IO.wait();
     cout<<"Philosopher "<<p_no+1<< " is hungry.\n";
     IO.release();
     test_condition(p_no);
     mutex1.release();
     if(ph_state[p_no]!= 0){
        IO.wait();
        cout<<"Philosopher "<<p_no+1<< " is waiting for fork.\n";
        IO.release();
        waiting_fork[p_no].wait();
     }
     sleep(1);
}

void if_full(int p_no){
    mutex1.release();
    ph_state[p_no]=2;
    IO.wait();
    cout<<"\t\tPhilosopher "<<p_no+1<< " is putting down fork and is thinking.\n";
    IO.release();
    test_condition((p_no+NO_PHPRS-1)%NO_PHPRS);
    test_condition((p_no+1)%NO_PHPRS);
    mutex1.release();
}



void* philosopher(void * p_no){
    for(int p=0;p<NO_TIMES_HUNGRY;p++){
        int i=*((int *)p_no);
        sleep(1);
        if_hungry(i);
        sleep(0);
        if_full(i);
    }

    return NULL;
}

void initialize(){
     //name, option flag , permission flag, value
    for(int i=0;i<NO_PHPRS;i++){
        ph_i[i]=i;
        ph_state[i]=2;
    }
}

int main(){
    initialize();
    pthread_t  tid[NO_PHPRS];   
    for(int i=0;i<NO_PHPRS;i++){
        pthread_create(&tid[i], NULL, philosopher, &ph_i[i]);
    }
    for(int i=0;i<NO_PHPRS;i++){
        pthread_join(tid[i], NULL); 
    }
    return 1;
}