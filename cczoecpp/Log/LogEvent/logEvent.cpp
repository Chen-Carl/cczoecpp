#include <stdarg.h>
#include "logEvent.h"

namespace cczoe {
namespace logcpp {

LogEvent::LogEvent(const std::string threadName, const char *file, int32_t line, uint32_t elapse, uint32_t tid, uint32_t fid, uint64_t time, std::shared_ptr<Logger> logger, LogLevel::Level level):
    m_threadName(threadName),
    m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_tid(tid),
    m_fid(fid),
    m_time(time),
    m_logger(logger),
    m_level(level)
{ }

void LogEvent::format(const char *fmt, ...)
{
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char *fmt, va_list al)
{
    char *buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != -1)
    {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

LogEventWrap::LogEventWrap(std::shared_ptr<LogEvent> event) :
    m_event(event)
{ }

LogEventWrap::~LogEventWrap()
{
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

}
}