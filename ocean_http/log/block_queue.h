/*************************************************************
 *循环数组实现的阻塞队列，m_back = (m_back + 1) % m_max_size;
 *线程安全，每个操作前都要先加互斥锁，操作完后，再解锁
 **************************************************************/

#ifndef __OCEANHTTP_BLOCK_QUEUE_H__
#define __OCEANHTTP_BLOCK_QUEUE_H__

#include "lock/locker.h"
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

template <class T>
class block_queue
{
private:
    locker m_mutex;
    cond m_cond;

    T *m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_back;

public:
    block_queue(int max_size = 1000);
    ~block_queue();
    void clear();
    bool full();  // 判断队列是否满
    bool empty(); // 判断队列是否为空
    bool front(T &value); // 返回队首元素
    bool back(T &value);  // 返回队尾元素
    int size();
    int max_size();
    bool push(const T &item);          // 往队列添加元素， 需要将所有使用队列的线程唤醒； 当有元素push进队列，相当于生产者生产力一个元素； 若当前没有线程等待条件变量，则唤醒无意义
    bool pop(T &item);                 // pop时，如果当前队列没有元素，将会等待条件变量
    bool pop(T &item, int ms_timeout); // pop 增加超时处理
};

#endif