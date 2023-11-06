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
#include "yaml-cpp/yaml.h"

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

    LogFormatter::LogFormatter(const std::string &pattern)
        : m_pattern(pattern)
    {
    }

    Logger::Logger(const std::string &name = "root")
    {
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        if (!appender->getLogFormatter())
        {
            appender->m_formatter = m_formatter;
        }
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

    void Logger::setLogFormatter(LogFormatter::ptr val)
    {
        m_formatter = val;
        for (auto &&i : m_appenders)
        {
            if (!i->m_hasFormatter)
            {
                i->m_formatter = m_formatter;
            }
        }
    }

    void Logger::setLogFormatter(const std::string val)
    {
        std::cout << "---" << val << std::endl;
        oceanserver::LogFormatter::ptr new_val(new oceanserver::LogFormatter(val));
        if (new_val->isError())
        {
            std::cout << "Logger setFormatter name=" << m_name
                      << "value=" << val << "invalid formatter"
                      << std::endl;
            return;
        }
        setLogFormatter(new_val);
    }

    LogFormatter::ptr Logger::getLogFormatter()
    {
        return m_formatter;
    }

    std::string Logger::toYamlString()
    {
        YAML::Node node;
        node["name"] = m_name;
        if (m_level != LogLevel::UNKNOWN)
        {
            node["level"] = LogLevel::ToString(m_level);
        }
        if (m_formatter)
        {
            node["formatter"] = m_formatter->getPattern();
        }
        for (auto &&i : m_appenders)
        {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }

        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = shared_from_this();
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

        for (size_t i = 0; i < m_pattern.size(); i++)
        {
            if (m_pattern[i] != '%')
            {
                vstr.append(1, m_pattern[i]);
                continue;
            }
            if ((i + 1) < m_pattern.size())
            {
                if (m_pattern[i + 1] == '%')
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

            while (n < m_pattern.size())
            {
                if (isspace(m_pattern[n])) // 判断是否是空格符号
                {
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i);
                        fmt_status = 1; // 解析格式
                        ++n;
                        fmt_begin = n;
                        continue;
                    }
                }

                if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
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
                str = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error:"
                          << m_pattern << "-" << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
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
            os << event->getLogger();
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
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S.%e") : m_format(format)
        {
            if (m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S.%e";
            }
        }
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadName();
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string &str)
            : m_string(str) {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                       const char *file, int32_t line, uint32_t elapse,
                       uint32_t thread_id, uint32_t fiber_id, uint64_t time,
                       const std::string &thread_name)
        : m_file(file),
          m_line(line),
          m_elapse(elapse),
          m_threadId(thread_id),
          m_fiberId(fiber_id),
          m_time(time),
          m_threadName(thread_name),
          m_logger(logger),
          m_level(level)

    {
    }

} // namespace name
