#include <iostream>
#include "../cczoecpp/Log/log.h"

using namespace cczoe;
using namespace cczoe::logcpp;

void testBasicLogic();
void testStreamLog();
void testFormatLog();

int main()
{
    testBasicLogic();
    testStreamLog();
    testFormatLog();
    return 0;
}

void testBasicLogic()
{
    std::cout << "------ test1: test basic logic ------" << std::endl;
    LogLevel::Level debug = LogLevel::DEBUG;
    std::shared_ptr<Logger> logger(new Logger());
    std::shared_ptr<LogEvent> event(new LogEvent("", "", 0, 0, 0, 0, 0, logger, debug));
    std::shared_ptr<LogAppender> stdAppender(new StdoutLogAppender);
    std::shared_ptr<LogAppender> fileAppender(new FileLogAppender("../tests/testLogs/fileout.txt"));
    std::shared_ptr<LogFormatter> fmtter(new LogFormatter("[%d{%H:%M:%S}]%T%M%T%F%n"));
    stdAppender->setFormatter(fmtter);
    fileAppender->setFormatter(fmtter);
    logger->addAppender(stdAppender);
    logger->addAppender(fileAppender);
    // test logger->log
    logger->log(debug, event);
    // test ~LogEventWrap()
    std::shared_ptr<LogEventWrap> wrapper(new LogEventWrap(event));
}

void testStreamLog()
{
    std::cout << "------ test2: test stream log output ------" << std::endl;
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "test log output: " << "success!";
}

void testFormatLog()
{
    std::cout << "------ test3: test format log output ------" << std::endl;
    CCZOE_LOG_FMT_DEBUG(CCZOE_LOG_ROOT(), "%s %s %s", "test", "format", "log output");
}
