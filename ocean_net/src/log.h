/*
 * @Author: OCEAN.GZY
 * @Date: 2023-11-04 16:25:39
 * @LastEditors: OCEAN.GZY
 * @LastEditTime: 2023-11-05 20:52:49
 * @FilePath: /ocean_net/src/log.h
 * @Description: 注释信息
 */
#ifndef __OCEANSERVER_LOG_H__
#define __OCEANSERVER_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

namespace oceanserver
{

    class Logger;
    class LoggerManager;

    // 日志级别
    class LogLevel
    {
    public:
        enum Level
        { // 日志级别
            UNKNOWN = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5,
        };

        static const char *ToString(LogLevel::Level level);
    };

    // 日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                 const char *file, int32_t line, uint32_t elapse,
                 uint32_t thread_id, uint32_t fiber_id, uint64_t time,
                 const std::string &thread_name);
        ~LogEvent();

        const char *getFile() const { return m_file; }
        int32_t getLine() const { return m_line; }
        uint32_t getElapse() const { return m_elapse; }
        uint32_t getThreadId() const { return m_threadId; }
        uint32_t getFiberId() const { return m_fiberId; }
        uint64_t getTime() const { return m_time; }
        std::string &getThreadName() { return m_threadName; }
        std::stringstream &getSS() { return m_ss; }
        std::shared_ptr<Logger> getLogger() { return m_logger; }
        const std::string getContent() const { return m_content; }
        LogLevel::Level getLevel() { return m_level; }

    private:
        const char *m_file = nullptr;     // 文件名
        int32_t m_line = 0;               // 行号
        uint32_t m_elapse = 0;            // 程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;          // 线程id
        uint32_t m_fiberId = 0;           // 协程id
        uint64_t m_time;                  // 日志时间戳
        std::string m_threadName;         // 线程名称
        std::stringstream m_ss;           // 日志内容流
        std::shared_ptr<Logger> m_logger; // 日志器
        LogLevel::Level m_level;          // 日志等级
        std::string m_content;            // 日志内容
    };

    // 日志格式器
    class LogFormatter
    {
    public:
        LogFormatter(const std::string &pattern);
        ~LogFormatter();

        typedef std::shared_ptr<LogFormatter> ptr;
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    public:
        void init();

        bool isError() const { return m_error; }

        const std::string getPattern() const { return m_pattern; }

        class FormatItem // 格式器的item
        {
        public:
            virtual ~FormatItem();
            virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

            typedef std::shared_ptr<FormatItem> ptr;
        };

    private:
        std::string m_pattern;                // 日志格式模版
        std::vector<FormatItem::ptr> m_items; // 日志格式解析后格式
        bool m_error = false;                 // 是否有错误
    };

    // 日志输出
    class LogAppender
    {
        friend class Logger;

    public:
        typedef std::shared_ptr<LogAppender> ptr; // 智能指针

        virtual ~LogAppender(){};

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0; // 纯虚函数，子类必须实现

        virtual std::string toYamlString() = 0;

        void setLogFormatter(LogFormatter::ptr val) { m_formatter = val; };
        LogFormatter::ptr getLogFormatter() const { return m_formatter; };

        LogLevel::Level getLevel() const { return m_level; }
        void setLevel(LogLevel::Level val) { m_level = val; }

    protected:
        LogLevel::Level m_level = LogLevel::DEBUG; // 日志级别
        LogFormatter::ptr m_formatter;             // 日志格式器
        bool m_hasFormatter = false;               // 是否有自己的日志格式器
    };

    // 日志器
    class Logger : public std::enable_shared_from_this<Logger>
    {
    public:
        Logger(const std::string &name = "root");
        ~Logger();

        typedef std::shared_ptr<Logger> ptr;
        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender); // 增加日志目标
        void delAppender(LogAppender::ptr appender); // 删除日志目标
        void clearAppenders();                       // 清空日志目标

        LogLevel::Level getLogLevel() { return m_level; }        // 获取日志级别
        void setLogLevel(LogLevel::Level val) { m_level = val; } // 设置日志级别

        const std::string &getName() const { return m_name; } // 返回日志名称

        void setLogFormatter(LogFormatter::ptr val); // 设置日志格式器
        void setLogFormatter(const std::string val); // 设置日志格式模版

        LogFormatter::ptr getLogFormatter();

        std::string toYamlString();

    private:
        std::string m_name;                      // 日志名称
        LogLevel::Level m_level;                 // 日志级别
        std::list<LogAppender::ptr> m_appenders; // Appender列表

        LogFormatter::ptr m_formatter; // 日志格式器
        Logger::ptr m_root;            // 主日志器
    };

    // 输出到控制台的Appender
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;
    };

    // 输出到文件的Appender
    class FileLogAppender : public LogAppender
    {
    public:
        FileLogAppender(const std::string filename);

        typedef std::shared_ptr<FileLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;

        bool reopen();

    private:
        std::string m_filename;     // 文件路径
        std::ofstream m_filestream; // 文件流
        uint64_t m_lastTime = 0;    // 上次重新打开时间
    };

    class LoggerManager
    {
    public:
        LoggerManager();
        Logger::ptr getLogger(const std::string &name);
        void init();

        Logger::ptr getRoot() const { return m_root; }
        std::string toYamlString();

    private:
        std::map<std::string, Logger::ptr> m_loggers;
        Logger::ptr m_root; // 主日志器
    };

    // 日志器管理类 单例模式
    // typedef oceanserver::Singleton<LoggerManager> LoggerMgr;

}

#endif