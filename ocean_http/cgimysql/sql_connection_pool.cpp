#include "sql_connection_pool.h"

sql_connection_pool::sql_connection_pool(/* args */)
{
    m_CurConn = 0;
    m_FreeConn = 0;
}

sql_connection_pool::~sql_connection_pool()
{
    DestroyPool();
}

// 当有请求时，从数据库连接池中 返回一个可用连接， 更新使用和空闲连接数
MYSQL *sql_connection_pool::GetConnection()
{
    MYSQL *_con = NULL;
    if (0 == connList.size())
    {
        return NULL;
    }

    reserve.wait();

    lock.lock();
    _con = connList.front();
    connList.pop_front();

    --m_FreeConn;
    ++m_CurConn;

    lock.unlock();
    return _con;
}

// 释放当前使用的连接
bool sql_connection_pool::ReleaseConnection(MYSQL *conn)
{
    if (NULL == conn)
    {
        return false;
    }

    lock.lock();

    connList.push_back(conn);
    ++m_FreeConn;
    --m_CurConn;

    lock.unlock();

    reserve.post();
    return true;
}

// 返回空闲连接数
int sql_connection_pool::GetFreeConn()
{
    return this->m_FreeConn;
}

void sql_connection_pool::DestroyPool()
{
    lock.lock();
    if (connList.size() > 0)
    {
        std::list<MYSQL *>::iterator it;
        for (it = connList.begin(); it != connList.end(); it++)
        {
            MYSQL *_con = *it;
            mysql_close(_con);
        }

        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }
    lock.unlock();
}

sql_connection_pool *sql_connection_pool::GetInstance()
{
    static sql_connection_pool conn_pool;
    return &conn_pool;
}

// 构造初始化
void sql_connection_pool::init(std::string Url, std::string User, std::string PassWord,
                               std::string DataBaseName, int Port, int MaxConn, int close_log)
{
    m_url = Url;
    m_port = Port;
    m_user = User;
    m_password = PassWord;
    m_database_name = DataBaseName;
    m_close_log = close_log;

    for (int i = 0; i < MaxConn; i++)
    {
        MYSQL *_con = NULL;
        _con = mysql_init(_con);

        if (_con == NULL)
        {
            LOG_ERROR("MySQL Error");
            exit(1);
        }
        _con = mysql_real_connect(_con, Url.c_str(), User.c_str(), PassWord.c_str(), DataBaseName.c_str(), Port, NULL, 0);

        if (_con == NULL)
        {
            LOG_ERROR("MySQL Error");
            exit(1);
        }

        connList.push_back(_con);
        ++m_FreeConn;
    }
    reserve = sem(m_FreeConn);
    m_MaxConn = m_FreeConn;
}

connectuinRAII::connectuinRAII(MYSQL **SQL, sql_connection_pool *conn_pool)
{
    *SQL =  conn_pool->GetConnection();

    connRAII = *SQL;
    poolRAII = conn_pool;
}

connectuinRAII::~connectuinRAII()
{
    poolRAII->ReleaseConnection(connRAII);
}
