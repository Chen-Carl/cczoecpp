#include "utils/macro.h"
#include "utils/debug.h"
#include "log.h"
#include "macro.h"

void testAssert();

std::shared_ptr<cczoe::logcpp::Logger> g_logger = CCZOE_LOG_ROOT();

int main()
{
    testAssert();
    return 0;
}

void testAssert()
{
    CCZOE_LOG_INFO(g_logger) << cczoe::debug::backtraceToString(10);
    CCZOE_ASSERT(0);
}