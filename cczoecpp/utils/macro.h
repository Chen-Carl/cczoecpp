#ifndef __ZOE_MACRO_H__
#define __ZOE_MACRO_H__

#include <assert.h>
#include "debug.h"

#define CCZOE_ASSERT(x) \
    if (!(x)) \
    { \
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "ASSERTION: " #x \
            << " failed\nbacktrace:\n" \
            << cczoe::debug::backtraceToString(100, 2); \
        assert(x); \
    }

#define CCZOE_ASSERT2(x, info) \
    if (!(x)) \
    { \
        CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "ASSERTION: " #x \
            << " failed\n" << info << "\nbacktrace:\n" \
            << cczoe::debug::backtraceToString(100, 2); \
        assert(x); \
    }

#endif