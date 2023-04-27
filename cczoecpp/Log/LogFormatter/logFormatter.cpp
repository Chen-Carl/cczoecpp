#include <cctype>
#include "logFormatter.h"

namespace cczoe {
namespace logcpp {
LogFormatter::LogFormatter(const std::string &pattern) :
    m_pattern(pattern)
{
    init();
}

std::string LogFormatter::format(std::shared_ptr<LogEvent> event)
{
    std::stringstream ss;
    for (auto item : m_items)
    {
        item->format(ss, event);
    }
    return ss.str();
}
/*
    LogFormatter:
        Create a pattern to output the log information.
        The pattern is a string that contains certain charater.

        %d output a time
            The time format string is optional. For example, a valid format string is 
            {%Y-%m-%d %H:%M:%S}
        %T symbol Tab
        %t thread id
        %N thread name
        %F fiber id
        %p log level
        %c log name
        %f file name
        %l line number
        %m log content
        %n symbol \n
        %% %
*/
void LogFormatter::init()
{
    size_t i = 0;
    std::string str;        // temporary string to store string formatter
    while (i < m_pattern.size())
    {
        if (m_pattern[i] == '%')
        {
            std::string dateFmt;
            switch (m_pattern[i + 1])
            {
            case 'd':
                // date format begin with '{'
                if (i + 2 < m_pattern.size() && m_pattern[i + 2] == '{')
                {
                    size_t end = i + 3;
                    while (end < m_pattern.size() && m_pattern[end] != '}')
                    {
                        end++;
                    }
                    // now end == pattern.size() or end == '}', we assure that the content is not empty
                    if (m_pattern[end] == '}' && end != i + 3)
                    {
                        // fetch the content between '{' and '}'
                        dateFmt = m_pattern.substr(i + 3, end - i - 3);
                        m_items.push_back(std::make_shared<DateTimeFormatItem>(dateFmt));
                        dateFmt.clear();
                        i = end - 1;
                        break;
                    }
                }
                m_items.push_back(std::make_shared<DateTimeFormatItem>());
                break;
            case 'T':
                m_items.push_back(std::make_shared<TabFormatItem>());
                break;
            case 't':
                m_items.push_back(std::make_shared<ThreadIdFormatItem>());
                break;
            case 'N':
                m_items.push_back(std::make_shared<ThreadNameFormatItem>());
                break;
            case 'F':
                m_items.push_back(std::make_shared<FiberIdFormatItem>());
                break;
            case 'p':
                m_items.push_back(std::make_shared<LevelFormatItem>());
                break;
            case 'c':
                m_items.push_back(std::make_shared<NameFormatItem>());
                break;
            case 'f':
                m_items.push_back(std::make_shared<FilenameFormatItem>());
                break;
            case 'l':
                m_items.push_back(std::make_shared<LineFormatItem>());
                break;
            case 'm':
                m_items.push_back(std::make_shared<MessageFormatItem>());
                break;
            case 'n':
                m_items.push_back(std::make_shared<NewLineFormatItem>());
                break;
            default:
                break;
            }
            i += 2;
        }
        else
        {
            str.push_back(m_pattern[i]);
            while (m_pattern[i + 1] != '%')
            {
                i++;
                str.push_back(m_pattern[i]);
            }
            // now m_pattern[i + 1] == '%'
            m_items.push_back(std::make_shared<StringFormatItem>(str));
            str.clear();
            i++;
        }
    }
}

}}