#include "config.h"

config::config(/* args */)
{
    PORT = 9006; //端口号 默认9006
    LOGWrite = 0; //日志写入方式，默认同步
    TRIGMode = 0; // 触发组合模式，默认listenfd LT + connfd LT
    LISTENTrigmode = 0; // listenfd触发模式，默认 LT 

    CONNTrigmode = 0; // connfd触发模式，默认 LT 
    OPT_LINGER = 0; //优雅关闭连接，默认不使用
    sql_num = 8; //数据库连接池 默认8
    thread_num = 8; //线程池 默认8

    close_log = 0; // 关闭日志，默认不关闭
    actor_model = 0; // 并发模型，默认proactor
}

config::~config()
{
}