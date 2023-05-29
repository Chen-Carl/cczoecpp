#ifndef __CCZOE_LOGFORMATTER_H__
#define __CCZOE_LOGFORMATTER_H__

#include <iostream>
#include <ctime>
#include <vector>
#include "Log/LogEvent.h"

namespace cczoe {
namespace logcpp {
class LogFormatter
{
public:
    class FormatItem
    {
    public:
        virtual ~FormatItem() { }
        virtual void format(std::ostream &os, std::shared_ptr<LogEvent> event) = 0;
    };

private:
    bool m_error = false;
    std::string m_pattern;
    std::vector<std::shared_ptr<FormatItem>> m_items;

    // pattern parser
    void init();

public:
    LogFormatter(const std::string &pattern);

    bool isError() const { return m_error; }
    std::string format(std::shared_ptr<LogEvent> event);
};

class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    MessageFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event) override
    {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem
{
public:
    LevelFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << LogLevel::toString(event->getLevel());
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem
{
public:
    ElapseFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem
{
public:
    NameFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        // format event logger's name
        os << event->getLogger()->getName();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadNameFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << event->getThreadName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadIdFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << event->getTid();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem
{
public:
    FiberIdFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << event->getFid();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem
{
private:
    std::string m_format;
public:
    DateTimeFormatItem(const std::string &str = "%Y-%m-%d %H:%M:%S") :
        m_format(str)
    { }

    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        struct tm tp;
        time_t time = event->getTime();
        localtime_r(&time, &tp);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tp);
        os << buf;
    }
};

class FilenameFormatItem : public LogFormatter::FormatItem
{
public:
    FilenameFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem
{
public:
    LineFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem
{
public:
    NewLineFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem
{
private:
    std::string m_string;

public:
    StringFormatItem(const std::string &str = "") :
        m_string(str)
    { }

    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << m_string;
    }
};

class TabFormatItem : public LogFormatter::FormatItem
{
public:
    TabFormatItem(const std::string &str = "") { }
    void format(std::ostream &os, std::shared_ptr<LogEvent> event)
    {
        os << "\t";
    }
};

}}

#endif