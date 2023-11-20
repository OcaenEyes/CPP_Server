#ifndef __OCEANHTTP_LOCKER_H__
#define __OCEANHTTP_LOCKER_H__

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
private:
    /* data */
    sem_t m_sem;

public:
    sem(/* args */);
    sem(int num);
    ~sem();

    bool wait();
    bool post();
};

class cond
{
private:
    /* data */
    pthread_cond_t m_cond;

public:
    cond(/* args */);
    ~cond();

    bool wait(pthread_mutex_t *m_mutex);
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t);
    bool signal();
    bool broadcast();
};

class locker
{
private:
    /* data */
    pthread_mutex_t m_mutex;

public:
    locker(/* args */);
    ~locker();

    bool lock();
    bool unlock();

    pthread_mutex_t *get();
};

#endif