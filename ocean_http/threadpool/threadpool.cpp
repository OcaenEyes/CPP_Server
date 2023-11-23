#include "threadpool.h"

template <typename T>
threadpool<T>::threadpool(int actor_model, sql_connection_pool *connPool, int thread_number = 8, int max_requests = 10000)
{
    if (thread_number <= 0 || max_requests <= 0)
    {
        throw std::exception();
    }

    m_threads = new pthread_t()
}

template <typename T>
threadpool<T>::~threadpool()
{
}