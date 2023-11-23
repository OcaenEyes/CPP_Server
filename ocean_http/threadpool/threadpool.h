#ifndef __OCEANHTTP_THREADPOOL_H__
#define __OCEANHTTP_THREADPOOL_H__

#include <list>
#include <cstdio>
#include <pthread.h>
#include "lock/locker.h"
#include "cgimysql/sql_connection_pool.h"

template <typename T>
class threadpool
{
private:
    /* data */
public:
    // thread_number是线程池中 线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量
    threadpool(int actor_model, sql_connection_pool *connPool, int thread_number = 8, int max_requests = 10000);
    ~threadpool();
    bool append(T *request, int state);
    bool append_p(T *request);

private:
    // 工作线程运行的函数， 不断从工作队列中取出任务并执行
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;             // 线程池中的线程数
    int m_max_requests;              // 请求队列中允许的最大请求数
    pthread_t *m_threads;            // 描述线程池的数组， 其大小为m_thread_number
    std::list<T *> m_workqueue;      // 请求队列
    locker m_queuelocker;            // 保护请求队列的互斥锁
    sem m_queuestat;                 // 是否有任务需要处理
    sql_connection_pool *m_connPool; // 数据库
    int m_actor_model;               // 模型切换
};
#endif