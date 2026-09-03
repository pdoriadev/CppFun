#ifndef LOGGING_H
#define LOGGING_H
#endif

#include <string>
#include <assert.h>         // One of the LogType types https://en.cppreference.com/c/error/assert

namespace Logging
{

    enum class LogType : int32_t
    {
        LogType = -100,
        INVALID = -1,
        LOG = 0,
        ERROR = 10,
        ASSERT = 20,
        COUNT = ASSERT + 10
    };
    
    std::string getLogTypeString(Logging::LogType type);
    // assign default params at prototype level. Doing so at both prototype and
    //  implementation level throws an error. 
    bool ConsoleLog(Logging::LogType type, const char* logMessage, bool flush = true);
}
