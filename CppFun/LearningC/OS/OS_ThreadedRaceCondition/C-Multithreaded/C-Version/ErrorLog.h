#ifndef ERROR_LOG_H
#define ERROR_LOG_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void printErrorToErrorFile(const char[]);

bool resetErrorLogFile();

#endif
