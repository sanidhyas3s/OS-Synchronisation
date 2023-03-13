// #include <bits/stdc++.h>
#include <queue>
#include <pthread.h>

class Semaphore
{
    private:
        int value;
        std::queue<pthread_t> waiting;
        // std::set<pthread_t> popped;
        pthread_t popped;
        bool waitcheck = true;
        bool releasecheck = true;
    public:
        Semaphore(int init);
        void wait();
        void release();

};

Semaphore::Semaphore(int init)
{
    this->value = init;
}

void Semaphore::wait()
{
    while(!waitcheck);
    waitcheck = false;
    this->value--;
    if(this->value<0)
    {
        waiting.push(pthread_self());
        waitcheck = true;
        // while(this->popped.find(pthread_self())==this->popped.end());
        while(!pthread_equal(this->popped, pthread_self()));
    }
    else{
        waitcheck = true;
    }
}

void Semaphore::release()
{
    while(!releasecheck);
    releasecheck = false;
    this->value++;
    if(!this->waiting.empty())
    {
        // this->popped.insert(this->waiting.front());
        this->popped = this->waiting.front();
        this->waiting.pop();
    }
    releasecheck = true;
}