#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

bool copyFileToDestinationFile(const char logFileNameStr[], FILE *destinationFP)
{
	FILE *sourceLogFP = fopen(logFileNameStr, "r");
	if (sourceLogFP == NULL)
	{
		return false;
	}

  // OUTPUT SOURCE FILE CONTENTS TO CONSOLE
	uint16_t BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	size_t bytesRead;
	size_t bytesWritten;
	while ((bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, sourceLogFP)) > 0)
	{
		if ((bytesWritten = fwrite(buffer, sizeof(char), bytesRead, destinationFP)) != bytesRead)
		{
			fflush(sourceLogFP);
			fclose(sourceLogFP);
			return false;
		}
	}

	// ERROR - did not reach end of file
  if (feof(sourceLogFP) == 0)
  {
	  char errorMessage[200] = "\nFailed to read to end of file: ";
    strcat(errorMessage, logFileNameStr);
  	printErrorToErrorFile(errorMessage);

    fflush(sourceLogFP);
    fclose(sourceLogFP);

    exit(EXIT_FAILURE);
  }

  // ERROR - a file error occurred. BUT WHAT THOUGH?
  if (ferror(sourceLogFP) != 0)
  {
    char errorMessage[200] = "\nFile error occurred in ";
    strcat(errorMessage, logFileNameStr);
    printErrorToErrorFile(errorMessage);

    fflush(sourceLogFP);
    fclose(sourceLogFP);

    exit(EXIT_FAILURE);
  }

	return true;
}

///////////////////////////////////////////////
// Given an empty string and an index value, provides the correct log file name.
bool getThreadLogFileName(uint32_t threadIndex, char logFileStrOut[], uint16_t bufferLength)
{
  if (bufferLength < 20)
  {
    char errorTxt[] = "Thread Log File Name String's Buffer is insufficient size";
    printErrorToErrorFile(errorTxt);
    exit(EXIT_FAILURE);
  }

  // Create Log File name for thread
  // append thread ID to end of log file name.
  snprintf(logFileStrOut, 5, "Log_");
  char threadIDString[bufferLength-9];
  snprintf(threadIDString, bufferLength-7, "%lu", threadIndex);
  strcat(logFileStrOut, threadIDString);
  strcat(logFileStrOut, ".txt");

  return true;
}

////////////////////////////////////////////////////
// merges the contents of all the log files into one
bool mergeThreadLogFiles(const uint32_t fileCount, char mergedFileStrOut[], const uint32_t BUFFER)
{
	if (BUFFER < 40)
	{
    char errorTxt[100] = "Merged Log File Name String's Buffer is insufficient size: ";
		char bufferStr[20];
		snprintf(bufferStr, 19, "%lu", BUFFER);
		strcat(errorTxt, bufferStr);
    printErrorToErrorFile(errorTxt);
    exit(EXIT_FAILURE);
	}

  //////////////////////////////////////////
  // Create merge file name string
  sprintf(mergedFileStrOut, "MergeFile_");
  char fileCountStr[20];
  snprintf(fileCountStr, 19, "%lu", fileCount);
  strcat(mergedFileStrOut, fileCountStr);
  strcat(mergedFileStrOut, "ThreadLogFiles.txt");

  // Try opening the merge file
  FILE *mergedFP = fopen(mergedFileStrOut, "w");
  if (mergedFP == NULL)
  {
    char errorMessage[200] = "Failed to open the merged log file: ";
    strcat(errorMessage, mergedFileStrOut);
    printErrorToErrorFile(errorMessage);
    exit(EXIT_FAILURE);
  }

  /////////////////////////////////////////
  // Setup for file copying (i.e. fread/fwrite)
  static const uint16_t BUFFER_SIZE = 1024;
  char buffer[1024];
  size_t bytes_read;
  size_t bytes_written;
  char logFileStrOut[40];

  /////////////////////////////////////////
  // COPY EACH LOG FILE
  // Write buffer to destination file stream.
    // Get the number of bytes written as a return value
    // If the number of bytes written is less than bytes read, then error?
  for(uint16_t i = 0; i < fileCount; i++)
  {
    // Open thread log file
    getThreadLogFileName(i, logFileStrOut, 40);
    FILE *threadLogFP = fopen(logFileStrOut, "r");
    if (threadLogFP == NULL)
    {
      char errorMessage[200] = "Failed to open thread log file: ";
      strcat(errorMessage, logFileStrOut);
      printErrorToErrorFile(errorMessage);

      fflush(mergedFP);
      fclose(mergedFP);

      exit(EXIT_FAILURE);
    }

    // Write source file stream to buffer. Write buffer to destination file stre>
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, threadLogFP)) > 0)
    {
      if ((bytes_written = fwrite(buffer, sizeof(char), bytes_read, mergedFP)) != bytes_read)
      {
        printErrorToErrorFile("\nFailed to write to merged log file.");
        exit(EXIT_FAILURE);
      }
    }

    // ERROR - did not reach end of file
    if (feof(threadLogFP) == 0)
    {
      char errorMessage[200] = "\nFailed to read to end of file: ";
      strcat(errorMessage, logFileStrOut);
      printErrorToErrorFile(errorMessage);

      fflush(threadLogFP);
      fclose(threadLogFP);
      fflush(mergedFP);
			fclose(mergedFP);

      exit(EXIT_FAILURE);
    }

    // ERROR - a file error occurred. BUT WHAT THOUGH?
    if (ferror(threadLogFP) != 0)
    {
      char errorMessage[200] = "\nFile error occurred in ";
      strcat(errorMessage, logFileStrOut);
      printErrorToErrorFile(errorMessage);

      fflush(threadLogFP);
      fclose(threadLogFP);
      fflush(mergedFP);
      fclose(mergedFP);

      exit(EXIT_FAILURE);
    }

    fflush(threadLogFP);
    fclose(threadLogFP);
  }

  fflush(mergedFP);
  fclose(mergedFP);
  return true;
}
