/*
 * @Author: OCEAN.GZY
 * @Date: 2023-11-26 10:55:10
 * @LastEditors: OCEAN.GZY
 * @LastEditTime: 2023-11-26 10:55:10
 * @FilePath: /ocean_http/log/log.h
 * @Description: 注释信息
 */
#ifndef __OCEANHTTP_LOG_H__
#define __OCEANHTTP_LOG_H__

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "log/block_queue.h"

class log
{
private:
    log(/* args */);
    virtual ~log();
    void *async_write_log();

private:
    char dir_name[128]; // 路径名
    char log_name[128]; // log文件名
    int m_split_lines;  // 日志最大行数
    int m_log_buf_size; // 日志缓冲区大小
    long long m_count;  // 日志行数记录
    int m_today;        // 记录当前日期
    FILE *m_fp;         // 打开log的文件指针
    char *m_buf;

    block_queue<std::string> *m_log_queue; // 阻塞队列
    bool m_is_async;                       // 是否同步标志位
    locker m_mutex;
    int m_close_log; // 关闭日志

public:
    static log *get_instance();
    static void *flusg_log_thread(void *args);

    // 可选择的参数：日志文件、 日志缓冲区大小、 最大行数、最长日志条队列
    bool init(const char *file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);
    void write_log(int level, const char *format, ...);
    void flush(void);
};

#define LOG_DEBUG(format, ...)                                    \
    if (0 == m_close_log)                                         \
    {                                                             \
        log::get_instance()->write_log(0, format, ##__VA_ARGS__); \
        log::get_instance()->flush();                             \
    }
#define LOG_INFO(format, ...)                                     \
    if (0 == m_close_log)                                         \
    {                                                             \
        log::get_instance()->write_log(1, format, ##__VA_ARGS__); \
        log::get_instance()->flush();                             \
    }
#define LOG_WARN(format, ...)                                     \
    if (0 == m_close_log)                                         \
    {                                                             \
        log::get_instance()->write_log(2, format, ##__VA_ARGS__); \
        log::get_instance()->flush();                             \
    }
#define LOG_ERROR(format, ...)                                    \
    if (0 == m_close_log)                                         \
    {                                                             \
        log::get_instance()->write_log(3, format, ##__VA_ARGS__); \
        log::get_instance()->flush();                             \
    }

#endif