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
    this->value--;
    if(this->value<0)
    {
        waiting.push(pthread_self());
        // while(this->popped.find(pthread_self())==this->popped.end());
        while(this->popped!=pthread_self());
    }
}

void Semaphore::release()
{
    this->value++;
    if(!this->waiting.empty())
    {
        // this->popped.insert(this->waiting.front());
        this->popped = this->waiting.front();
        this->waiting.pop();
    }
}