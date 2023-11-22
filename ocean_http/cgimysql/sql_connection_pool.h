#ifndef __OCEANHTTP_MYSQL_CONNECTION_POOL_H__
#define __OCEANHTTP_MYSQL_CONNECTION_POOL_H__

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "lock/locker.h"
#include "log/log.h"

class sql_connection_pool
{
private:
    /* data */
    sql_connection_pool(/* args */);
    ~sql_connection_pool();

    int m_MaxConn;  // 最大连接数
    int m_CurConn;  // 当前已使用的连接数
    int m_FreeConn; // 当前空闲的连接数

    locker lock;
    std::list<MYSQL *> connList; // 连接池
    sem reserve;

public:
    std::string m_url;  // 主机地址
    std::string m_port; // 数据库端口号
    std::string m_user;
    std::string m_password;
    std::string m_database_name;
    int m_close_log; // 日志开关

public:
    MYSQL *GetConnection();              // 获取数据库连接
    bool ReleaseConnection(MYSQL *conn); // 释放连接
    int GetFreeConn();                   // 获取连接
    void DestroyPool();                  // 销毁所有连接

    // 单例模式
    static sql_connection_pool *GetInstance();

    void init(std::string Url, std::string User, std::string PassWord,
              std::string DataBaseName, int Port, int MaxConn, int close_log);
};


class connectionRAII
{
private:
    MYSQL *connRAII;
    sql_connection_pool *poolRAII;

public:
    connectionRAII(MYSQL **SQL, sql_connection_pool *conn_pool);
    ~connectionRAII();
};

#endif