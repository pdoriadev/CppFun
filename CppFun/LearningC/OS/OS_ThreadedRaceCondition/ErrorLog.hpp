#pragma once
#include <stdio.h>
#include <string.h>

void printErrorToErrorFileThenExit(char errorStr[])
{
	FILE *fp = fopen("error.txt", "w+");
	fprintf(fp, errorStr);

	fflush(fp);
	fclose(fp);

	exit(1);
}
