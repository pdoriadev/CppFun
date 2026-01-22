#ifndef GAMETIME_H
#define GAMETIME_H

#include <stdint.h> // uint64
#include <time.h> // clock_gettime, CLOCK_MONOTONIC_RAW
#include <ctime>
#include <sys/time.h>
#include <assert.h>
#include <chrono>

typedef struct
{
    uint64_t lastTime;
    uint64_t lastSimulationUpdate;
    uint64_t currentTime;
    uint64_t elapsed;
    uint64_t uSecPerStep;
} timeData ;

enum timeResolution
{
    NANO,
    MICRO,
    MILLI,
    SEC,
    NUMBER_OF_RESOLUTIONS
};

////////
/// Based on: https://stackoverflow.com/questions/5833094/get-a-timestamp-in-c-in-microseconds
static void getTime(timeResolution res, uint64_t &time)
{
    static timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    switch(res)
    {
    case NANO:
        time = (uint64_t)(ts.tv_sec * 1000000000 + ts.tv_nsec);
        break;
    case MICRO:
        time = (uint64_t)(ts.tv_sec * 1000000 + ts.tv_nsec * 1000);
        break;
    case MILLI:
        time = (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
        break;
    case SEC:
        time = (uint64_t)ts.tv_sec + ts.tv_nsec / 1000000000;
        break;
    default:
        assert(false && "Not a valid time resolution.");
        break;
    }
}

#endif // GAMETIME_H
