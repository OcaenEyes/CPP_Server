/*
 * @Author: OCEAN.GZY
 * @Date: 2023-11-23 21:42:40
 * @LastEditors: OCEAN.GZY
 * @LastEditTime: 2023-11-23 21:47:37
 * @FilePath: /ocean_http/threadpool/threadpool.cpp
 * @Description: 注释信息
 */
#include "threadpool.h"

template <typename T>
threadpool<T>::threadpool(int actor_model, sql_connection_pool *connPool, int thread_number = 8, int max_requests = 10000)
{
    if (thread_number <= 0 || max_requests <= 0)
    {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
    {
        throw std::exception();
    }
    for (int i = 0; i < thread_number; i++)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }

        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
}

template<typename T>
bool threadpool<T>::append(T *request, int state)
{
    
}

template<typename T>
bool threadpool<T>::append_p(T *request)
{
    
}

template<typename T>
void* threadpool<T>::worker(void *arg)
{
    
}

template<typename T>
void threadpool<T>::run()
{
    
}

