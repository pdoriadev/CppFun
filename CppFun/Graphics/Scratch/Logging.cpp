    #include "Logging.h"
#include <assert.h>         // One of the LogType types https://en.cppreference.com/c/error/assert
#include <iostream>
#include <fstream>

namespace Logging
{
    std::fstream logFile;

    //-//////////////////////////////////
    //
    std::string getLogTypeString(LogType type)
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
                consoleLog(LogType::ERROR, errorMessage.c_str());
                return "NON-IMPLEMENTED_TYPE";
        }

        consoleLog(LogType::ASSERT, "Switch statement faailed to break or return.");
        return "FAILED";
    }

    //-//////////////////////////////////
    // LATER wrap functionality around an #ifdef for DEBUG vs RELEASE
    bool consoleLog(LogType type, std::string logMessage, bool flush)
    {
        std::string typeStr = getLogTypeString(type);
        logMessage.insert(0, typeStr + ": ");
        
        openLogFileIfClosed();
        outputToLogFile(type, logMessage, flush);

        switch(type)
        {
            case LogType::LOG:
                std::cout << logMessage;
                return true;
            case LogType::ERROR:
                std::cerr << logMessage;
                return true;
            case LogType::ASSERT:
                closeLogFileIfOpen();
                assert((logMessage.c_str()) && false);
                return true;
            default:
                std::string errorMessage = getLogTypeString(type) + " is not an implemented " + getLogTypeString(LogType::LogType) + ". May be invalid.";
                consoleLog(LogType::ERROR, errorMessage);
                return false;
        }

        // Called after #ifdef NOT IMPLEMENTED YET
        return false;
    }

    //-//////////////////////////////////
    // Returns true if opens a new log file.
    bool openLogFileIfClosed()
    {
        if (logFile.is_open()) return false;

        const std::string logFileName = "Log.txt";
        //std::cout << "Attempting to open log file: " << logFileName << std::endl;
        logFile.open(logFileName, std::fstream::out); // https://stackoverflow.com/questions/8835888/stdfstream-doesnt-create-file

        if (logFile.is_open() == false)
        {
            const std::string errorMessage = "Failed to open log file: " + logFileName;
            std::cerr << errorMessage << std::endl;
            assert((errorMessage, false));
        }

        return true;
    }

    //-//////////////////////////////////
    // Returns true if closes an open log file
    bool closeLogFileIfOpen()
    {
        if (logFile.is_open() == false) return false;

        consoleLog(LogType::LOG, "Closing log file");
        logFile.close();
        return true;
    }

    //-//////////////////////////////////
    // 
    bool outputToLogFile(LogType type, std::string logMessage, bool flush)
    {
        logFile << logMessage;
        if (flush) logFile << std::endl;
        return true;
    }
}
