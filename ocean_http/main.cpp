#include "config.h"

int main(int argc, char *argv[])
{
    std::string user = "root";
    std::string passwd = "123456";
    std::string databasename = "oceanhttp";

    // 命令行解析
    config _config;
    _config.parse_arg(argc, argv);

    webserver _webserver;

    // 初始化
    _webserver.init(_config.PORT, user, passwd, databasename, _config.LOGWrite,
                    _config.OPT_LINGER, _config.TRIGMode, _config.sql_num, _config.thread_num,
                    _config.close_log, _config.actor_model);

    // 日志
    _webserver.log_write();

    // 数据库
    _webserver.sql_pool();

    // 线程池
    _webserver.thread_pool();

    // 触发模式
    _webserver.trig_mode();

    // 监听
    _webserver.eventListen();

    // 运行
    _webserver.eventLoop();

    return 0;
}