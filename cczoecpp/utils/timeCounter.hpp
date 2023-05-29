#pragma once

#include <execinfo.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#include "Log/Log.h"

class TimeCounter
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime;

    void start() { m_startTime = std::chrono::high_resolution_clock::now(); }
    void stop() { m_endTime = std::chrono::high_resolution_clock::now(); }

    void backtrace(std::vector<std::string> &bt, int size, int skip)
    {
        void **array = (void**)malloc((sizeof(void*) * size));
        size_t s = ::backtrace(array, size);
        char **strings = backtrace_symbols(array, s);
        if (strings == NULL)
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << "backtrace_symbols error";
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

public:
    TimeCounter() { start(); }

    ~TimeCounter() { 
        stop(); 
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << backtraceToString(1, 0) << std::endl << "Time elapsed: " << getDuration() << " ms";
    }

    double getDuration() const { 
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_endTime - m_startTime).count(); 
    }
};