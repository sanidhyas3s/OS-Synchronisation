#include "semaphore.cpp"
#include <pthread.h>
#include <bits/stdc++.h>
#include <unistd.h>
#define ITERATIONS 10

using namespace std;

Semaphore Match = Semaphore(1);
Semaphore Tobacco = Semaphore(1);
Semaphore Paper = Semaphore(1);
Semaphore more_needed = Semaphore(1);
Semaphore IO = Semaphore(1);

void smoke(int i){
    IO.wait();
    if(i==0) printf("Smoker with tobacco🌿 is smoking\n");
    else if(i==1) printf("Smoker with paper📃 is smoking\n");
    else printf("Smoker with match🔥 is smoking\n");
    IO.release();
}

void* agent (void* args)
{
    for(int i=0;i<ITERATIONS;i++){
      sleep(1);
      int number = rand()%3;
      switch (number)
      {
          case 0: IO.wait();
                  cout<<"Agent provides match🔥 and paper📃\n";
                  IO.release();
                  Match.release() ; 
                  Paper.release();
                  break;
          case 1: IO.wait();
                  cout<<"Agent provides match🔥 and tobacco🌿\n";
                  IO.release();
                  Tobacco.release();
                  Match.release();
                  break;
         case 2:  IO.wait();
                  cout<<"Agent provides tobacco🌿 and paper📃\n";
                  IO.release();
                  Tobacco.release();
                  Paper.release();
                  break;
      }

      if(i<ITERATIONS-1) more_needed.wait();
      else exit(0);

    }

    return NULL;
}

void* smoker_with_tobacco (void* args)
{
  while (true)
    {
      if (Match.try_wait() == 1 && Paper.try_wait() == 1)
        {
          Match.wait();
          Paper.wait();
          smoke(0);
          more_needed.release();
        }
    }
    return NULL;
}

void* smoker_with_paper (void* args)
{
  while(true)
    {
      if (Match.try_wait() == 1 && Tobacco.try_wait()==1)
        {
          Match.wait();
          Tobacco.wait();
          smoke(1);
          more_needed.release();
        }
    }
    return NULL;
}

void* smoker_with_match (void* args)
{
  while (true)
    {
      if (Paper.try_wait() == 1 && Tobacco.try_wait()==1)
        {
          Paper.wait();
          Tobacco.wait();
          smoke(2);
          more_needed.release(); 
        }
    }
    return NULL;
}

int main() {

    pthread_t agent_thread, smoker_thread_1, smoker_thread_2, smoker_thread_3;

    Match.wait();
    Tobacco.wait();
    Paper.wait();
    more_needed.wait();

    pthread_create(&agent_thread, NULL, agent, NULL);
    pthread_create(&smoker_thread_1, NULL, smoker_with_tobacco, NULL);
    pthread_create(&smoker_thread_2, NULL, smoker_with_paper, NULL);
    pthread_create(&smoker_thread_3, NULL, smoker_with_match, NULL);

    pthread_join(agent_thread, NULL);
    pthread_join(smoker_thread_1, NULL);
    pthread_join(smoker_thread_2, NULL);
    pthread_join(smoker_thread_3, NULL);

}