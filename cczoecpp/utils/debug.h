#ifndef __ZOE_DEBUG_H__
#define __ZOE_DEBUG_H__

#include <vector>
#include <string>

namespace cczoe {
namespace debug {

void backtrace(std::vector<std::string> &bt, int size, int skip);
std::string backtraceToString(int size, int skip = 2);

}
}
#endif