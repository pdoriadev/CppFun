#ifndef THREAD_LOG_H
#define THREAD_LOG_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ErrorLog.h"

bool copyFileToDestinationFile(const char[], FILE*);

///////////////////////////////////////////////
// Given an empty string and an index value, provides the correct log file name.
bool getThreadLogFileName(uint32_t, char[], uint16_t);

////////////////////////////////////////////////////
// merges the contents of all the log files into one
bool mergeThreadLogFiles(const uint32_t, char[], const uint32_t);

#endif
