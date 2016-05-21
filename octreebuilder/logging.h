#pragma once

#include <iostream>
#include <stdexcept>
#include <sstream>


#define LOG_TO_STREAM(level, stream, msg)   \
    {                                       \
        std::stringstream ss;               \
        ss << "[" << #level << "] " << msg; \
        stream << ss.str();    \
    }


#define LOG_INFO(msg) LOG_TO_STREAM(INFO, std::cout, msg)
#define LOG_WARN(msg) LOG_TO_STREAM(WARN, std::cout, msg)
#define LOG_ERROR(msg) LOG_TO_STREAM(ERROR, std::cerr, msg)