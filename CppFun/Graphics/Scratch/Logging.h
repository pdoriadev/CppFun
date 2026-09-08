#ifndef LOGGING_H
#define LOGGING_H
#endif

#include <string>
#include <assert.h>         // One of the LogType types https://en.cppreference.com/c/error/assert

//-//////////////////////////////////
//
namespace Logging
{
    //-//////////////////////////////////
    //
    enum class LogType : int32_t
    {
        LogType = -100,
        INVALID = -1,
        LOG = 0,
        ERROR = 10,
        ASSERT = 20,
        COUNT = ASSERT + 10
    };
    
    //-//////////////////////////////////
    //
    std::string getLogTypeString(LogType type);

    //-//////////////////////////////////
    // NOTE: assigns default params at prototype level. Assigning in prototypeand implementation throws an error. 
    // 
    bool consoleLog(LogType type, std::string logMessage, bool flush = true);

    //-//////////////////////////////////
    // Returns true if opens a new log file.
    bool openLogFileIfClosed();

    //-//////////////////////////////////
    // Returns true if closes an open log file
    bool closeLogFileIfOpen();

    //-//////////////////////////////////
    // 
    bool outputToLogFile(LogType type, std::string logMessage, bool flush);
}
