#include "Logging.h"
#include <assert.h>         // One of the LogType types https://en.cppreference.com/c/error/assert
#include <iostream>

namespace Logging
{
    std::string getLogTypeString(Logging::LogType type)
    {
        switch(type)
        {
            case LogType::LogType:
                return "LogType";
            case LogType::INVALID:
                return "INVALID";
            case LogType::LOG:
                return "LOG";
            case LogType::ERROR:
                return "ERROR";
            case LogType::ASSERT:
                return "ASSERT";
            case LogType::COUNT:
                return "COUNT";
            default:
                const std::string errorMessage = "Type for " + getLogTypeString(LogType::ERROR) + " is not implemented. May be invalid: " + getLogTypeString(type);
                ConsoleLog(Logging::LogType::ERROR, errorMessage.c_str());
                return "NON-IMPLEMENTED_TYPE";
        }

        Logging::ConsoleLog(LogType::ASSERT, "Switch statement faailed to break or return.");
        return "FAILED";
    }

    // LATER wrap functionality around an #ifdef for DEBUG vs RELEASE
    bool ConsoleLog(Logging::LogType type, const char* logMessage, bool flush)
    {
        switch(type)
        {
            case LogType::LOG:
                std::cout << logMessage;
                return true;
            case LogType::ERROR:
                std::cerr << logMessage;
                return true;
            case LogType::ASSERT:
                assert(0 && logMessage);
                return true;
            default:
                std::string errorMessage = getLogTypeString(type) + " is not an implemented " + getLogTypeString(LogType::LogType) + ". May be invalid.";
                ConsoleLog(LogType::ERROR, errorMessage.c_str());
                return false;
        }

        // Called after #ifdef NOT IMPLEMENTED YET
        return false;
    }
}
