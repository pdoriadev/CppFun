#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printErrorToErrorFileThenExit(const char errorStr[])
{
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

