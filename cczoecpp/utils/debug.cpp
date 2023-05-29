#include <execinfo.h>
#include "utils/debug.h"
#include "Log/Log.h"

namespace cczoe {
namespace debug {

std::shared_ptr<logcpp::Logger> g_logger = CCZOE_LOG_NAME("system");

void backtrace(std::vector<std::string> &bt, int size, int skip)
{
    void **array = (void**)malloc((sizeof(void*) * size));
    size_t s = ::backtrace(array, size);
    char **strings = backtrace_symbols(array, s);
    if (strings == NULL)
    {
        CCZOE_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for (size_t i = skip; i < s; i++)
    {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(array);
}

std::string backtraceToString(int size, int skip)
{
    std::vector<std::string> bt;
    backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); i++)
    {
        ss << "\t" << bt[i] << std::endl;
    }
    return ss.str();
}

}
}