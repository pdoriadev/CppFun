#include "ErrorLog.h"

void printErrorToErrorFile(const char errorStr[])
{
	static uint32_t errorCount;
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
