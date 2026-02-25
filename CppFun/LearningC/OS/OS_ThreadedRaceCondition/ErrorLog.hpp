#pragma once
#include <stdio.h>
#include <string.h>


void printErrorToErrorFileThenExit(const char errorStr[])
{
	FILE *fp = fopen("error.txt", "w+");
	fprintf(fp, "%s", errorStr);

	fflush(fp);
	fclose(fp);

	exit(1);
}
