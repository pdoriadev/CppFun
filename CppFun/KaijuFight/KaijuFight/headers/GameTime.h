#ifndef GAMETIME_H
#define GAMETIME_H

#include <stdint.h> // uint64
#include <time.h> // clock_gettime, CLOCK_MONOTONIC_RAW
#include <assert.h>

typedef struct
{
    uint64_t startOfGameLoop;
    uint64_t elapsedSinceLastGameLoop;
    uint64_t lastSimUpdate;
    uint64_t uSecPerSimStep;
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
/// Linux Man: https://www.man7.org/linux/man-pages/man3/clock_gettime.3.html
///
/// CLOCK_MONOTONIC_RAW vs CLOCK_MONOTONIC vs CLOCK_BOOTTIME: https://tigerbeetle.com/blog/2021-08-30-three-clocks-are-better-than-one/
/// CLOCK_MONOTONIC_RAW is missing on windows QT, but not Linux. Need to learn CMAKE to pull in the proper libraries to run it.
///     CLOCK_MONOTONIC changes based on NTP. It hangs when the system suspends <--- maybe I want this though? I don't want enemies running
///         around when the player can't play. That'd be silly. It would make sense for a networked game - but I'm not making a networked game.
///
/// Additional links
///     https://stackoverflow.com/questions/5833094/get-a-timestamp-in-c-in-microseconds
static void getTime(timeResolution res, uint64_t &time)
{
    static timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t seconds = (uint64_t)ts.tv_sec;
    uint64_t nano = (uint64_t)ts.tv_nsec;

    // BUG - least significant digit of nanosecond is getting chopped off during unit conversion \:
    switch(res)
    {
    case NANO:
        time = seconds * 1000000000 + nano;
        break;
    case MICRO:
        time = seconds * 1000000    + nano / 1000;
        break;
    case MILLI:
        time = seconds * 1000       + nano / 1000000;
        break;
    case SEC:
        time = seconds              + nano / 1000000000;
        break;
    default:
        assert(false && "Not a valid time resolution.");
        break;
    }
}

#endif // GAMETIME_H
