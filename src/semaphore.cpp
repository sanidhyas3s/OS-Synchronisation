#include <queue>
#include <pthread.h>

class Semaphore
{
    private:
        int value;
        std::queue<pthread_t> waiting;
        pthread_t popped;
        bool waitcheck = true;
        bool releasecheck = true;
    public:
        Semaphore();
        Semaphore(int init);
        void wait();
        void release();
        bool try_wait();
};

Semaphore::Semaphore(){}

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
        while(!pthread_equal(this->popped, pthread_self()));
    }
    else
    {
        waitcheck = true;
    }
}

bool Semaphore::try_wait()
{
    return value>0;
}

void Semaphore::release()
{
    while(!releasecheck);
    releasecheck = false;
    this->value++;
    if(!this->waiting.empty())
    {
        this->popped = this->waiting.front();
        this->waiting.pop();
    }
    releasecheck = true;
}
