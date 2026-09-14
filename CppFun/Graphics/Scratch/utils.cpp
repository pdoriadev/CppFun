#include "utils.h"
#include "Logging.h"

const bool IsNullPtr(void* pointer, std::string typeStr)
{
    if (pointer == NULL)
    {
        Logging::consoleLog(Logging::LogType::ERROR, 
            ("Pointer of type " + typeStr + "is null"));
        return true;
    }

    return false;
}