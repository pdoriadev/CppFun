#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint32_t errorCount = 0;

void printErrorToErrorFile(const char errorStr[])
{
	errorCount += 1;

	FILE *fp = fopen("error.txt", "w");
	fprintf(fp, "%s", errorStr);

	fflush(fp);
	fclose(fp);

	char consoleMessageStr[500] = "\nExited with Error\n";
	snprintf(consoleMessageStr,470, errorStr);
	perror(consoleMessageStr);
}

bool resetErrorLogFile()
{
	FILE *fp = fopen("error.txt", "w");
	fclose(fp);
	return true;
}

