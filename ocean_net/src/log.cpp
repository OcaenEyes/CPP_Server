/*
 * @Author: OCEAN.GZY
 * @Date: 2023-11-04 17:28:07
 * @LastEditors: OCEAN.GZY
 * @LastEditTime: 2023-11-05 09:36:04
 * @FilePath: /ocean_net/src/log.cpp
 * @Description: 注释信息
 */
#include "log.h"
#include <map>
#include <functional>

namespace oceanserver
{
    const char *LogLevel::ToString(LogLevel::Level level)
    {
        switch (level)
        {
#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX

        default:
            return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    LogFormatter::LogFormatter(const std::string &patten)
        : m_patten(patten)
    {
    }

    Logger::Logger(const std::string &name = "root")
    {
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto i = m_appenders.begin(); i != m_appenders.end(); i++)
        {
            if (*i == appender)
            {
                m_appenders.erase(i);
                break;
            }
        }
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = std::shared_from_this();
            for (auto &i : m_appenders)
            {
                i->log(self, level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }

    FileLogAppender::FileLogAppender(const std::string filename)
    {
        m_filename = filename;
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    bool FileLogAppender::reopen()
    {
        if (m_filestream)
        {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !!m_filestream; // 双!!转换类型
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter->format(logger, level, event);
        }
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &&i : m_items)
        {
            i->format(ss, logger, level, event);
        }

        return std::string();
    }

    void LogFormatter::init()
    {
        // str， format, type
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string vstr;

        for (size_t i = 0; i < m_patten.size(); i++)
        {
            if (m_patten[i] != '%')
            {
                vstr.append(1, m_patten[i]);
                continue;
            }
            if ((i + 1) < m_patten.size())
            {
                if (m_patten[i + 1] == '%')
                {
                    vstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;

            while (n < m_patten.size())
            {
                if (isspace(m_patten[n])) // 判断是否是空格符号
                {
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_patten[n] == '{')
                    {
                        str = m_patten.substr(i + 1, n - i);
                        fmt_status = 1; // 解析格式
                        ++n;
                        fmt_begin = n;
                        continue;
                    }
                }

                if (fmt_status == 1)
                {
                    if (m_patten[n] == '}')
                    {
                        fmt = m_patten.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 2;
                        break;
                    }
                }
            }

            if (fmt_status == 0)
            {
                if (!vstr.empty())
                {
                    vec.push_back(std::make_tuple(vstr, "", 0));
                }
                str = m_patten.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
            else if (fmt_status == 1)
            {
                std::cout << "patten parse error:"
                          << m_patten << "-" << m_patten.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<patten_error>>", fmt, 0));
            }
            else if (fmt_status == 2)
            {
                if (!vstr.empty())
                {
                    vec.push_back(std::make_tuple(vstr, "", 0));
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }

        if (!vstr.empty())
        {
            vec.push_back(std::make_tuple(vstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_format_items = {
#define XX(str, C)                                                               \
    {                                                                            \
        #str, [](const std::string &fmt) { return FormatItem::ptr(new C(fmt)); } \
    }

            XX(m, MessageFormatItem),    // m:消息
            XX(p, LevelFormatItem),      // p:日志级别
            XX(r, ElapseFormatItem),     // r:累计毫秒数
            XX(c, NameFormatItem),       // c:日志名称
            XX(t, ThreadIdFormatItem),   // t:线程id
            XX(n, NewLineFormatItem),    // n:换行
            XX(d, DateTimeFormatItem),   // d:时间
            XX(f, FilenameFormatItem),   // f:文件名
            XX(l, LineFormatItem),       // l:行号
            XX(T, TabFormatItem),        // T:Tab
            XX(F, FiberIdFormatItem),    // F:协程id
            XX(N, ThreadNameFormatItem), // N:线程名称
#undef XX
        };
        // %m -- 消息体
        // %p -- level
        // %r -- 启动后的时间
        // %c -- 日志名称
        // %t -- 线程id
        // %n -- 回车换行
        // %d -- 日志时间
        // %f -- 文件名
        // %l -- 行号
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberId();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr)
        {
            os << std::endl;
        }
    };

class DateTimeFormatItem:public LogFormatter::FormatItem{
    public:
        DateTimeFormatItem(const std::string &str = "%Y-%m-%d %H:%M:%S.%e"):m_for
        {
            format_str=str;
        }
        void format(std::ostream &os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event   ){
            
        }
};


} // namespace name
