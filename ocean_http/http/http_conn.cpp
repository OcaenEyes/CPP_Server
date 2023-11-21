#include "http_conn.h"

// 定义http响应的 状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file.\n";

locker m_lock;
std::map<std::string, std::string> users;

// 对文件描述符 设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

// 将内核事件表 注册读事件， ET模式， 选择开启EPOLLONESHOT
void addfd(int epollfd, int fd, bool one_shot, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
    {
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    }
    else
    {
        event.events = EPOLLIN | EPOLLHUP;
    }

    if (one_shot)
    {
        event.events |= EPOLLONESHOT; // 按位或运算
    }

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

// 从内核时间表 删除描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

// 将事件重置为EPOLLONESHOT
void modfd(int epollfd, int fd, int ev, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
    {
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    }
    else
    {
        event.events = ev | EPOLLONESHOT | EPOLLHUP;
    }

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd =-1;

http_conn::http_conn(/* args */)
{
}

http_conn::~http_conn()
{
}

void http_conn::init(int sockfd, const sockaddr_in &addr, char *, int, int, std::string user, std::string passwd, std::string sqlname)
{
}


// 关闭连接， 关闭一个连接，客户总量减1
void http_conn::close_conn(bool real_close)
{
    if(real_close && (m_sokcfd!= -1)){
        
    }
}

void http_conn::process()
{
}

bool http_conn::read_once()
{
}

bool http_conn::write()
{
}

sockaddr_in *http_conn::get_address()
{
    return &m_address;
}

void http_conn::initmysql_result(sql_connection_pool *connPool)
{
}

void http_conn::init()
{
}

http_conn::HTTP_CODE http_conn::process_read()
{
}

bool http_conn::process_write(HTTP_CODE ret)
{
}

http_conn::HTTP_CODE http_conn::parse_request_line(char *text)
{
}

http_conn::HTTP_CODE http_conn::parse_headers(char *text)
{
}

http_conn::HTTP_CODE http_conn::parse_content(char *text)
{
}

http_conn::HTTP_CODE http_conn::do_request()
{
}

char *http_conn::get_line()
{
    return m_read_buf + m_start_line;
}

http_conn::LINE_STATUS http_conn::parse_line()
{
}

void http_conn::unmap()
{
}

bool http_conn::add_response(const char *format, ...)
{
}

bool http_conn::add_content(const char *content)
{
}

bool http_conn::add_status_line(int status, const char *title)
{
}

bool http_conn::add_headers(int content_length)
{
}

bool http_conn::add_content_type()
{
}

bool http_conn::add_content_length(int content_length)
{
}

bool http_conn::add_linger()
{
}

bool http_conn::add_blank_line()
{
}