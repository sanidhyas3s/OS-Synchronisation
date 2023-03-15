#include <queue>
#include <pthread.h>
#include <atomic>

using namespace std;

class Semaphore
{
    private:
        volatile atomic_flag exclusion;
        int value;
        std::queue<pthread_t> waiting;
        pthread_t popped;
    public:
        Semaphore();
        Semaphore(int init);
        void wait();
        bool try_wait();
        void release();
};

Semaphore::Semaphore(){}

Semaphore::Semaphore(int init)
{
    this->value = init;
}

void Semaphore::wait()
{
    while(atomic_flag_test_and_set(&(this->exclusion)));
    this->value--;
    atomic_flag_clear(&(this->exclusion));
    if(this->value<0)
    {
        waiting.push(pthread_self());
        while(!pthread_equal(this->popped, pthread_self()));
        this->popped = NULL;
        atomic_flag_clear(&(this->exclusion));
    }
}

bool Semaphore::try_wait()
{
    return this->value>0;
}

void Semaphore::release()
{
    while(atomic_flag_test_and_set(&(this->exclusion)));
    this->value++;
    if(!this->waiting.empty())
    {
        this->popped = this->waiting.front();
        this->waiting.pop();
    }
    else atomic_flag_clear(&(this->exclusion));
}
