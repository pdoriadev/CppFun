#include <stdio.h>
// uints
#include <stdint.h>
// malloc
#include <stdlib.h>
#include <string.h>
// exit()
#include <unistd.h>
// threads
#include <pthread.h>
#include "BankAccount.hpp"
#include "AccountActions.hpp"
#include "Errorlog.hpp"

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
bool mergeThreadLogFiles(uint32_t fileCount)
{
  //////////////////////////////////////////
	// Create merge file name string
	char mergeFileStr[50] = "MergeFile_";
	char fileCountStr[20];
	snprintf(fileCountStr, 19, "%lu", fileCount);
	strcat(mergeFileStr, fileCountStr);
	strcat(mergeFileStr, "ThreadLogFiles.txt");

	// Try opening the merge file
	FILE *mergedFP = fopen(mergeFileStr, "w");
	if (mergedFP == NULL)
	{
    char errorMessage[200] = "Failed to open the merged log file: ";
    strcat(errorMessage, mergeFileStr);
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

  	// Write source file stream to buffer. Write buffer to destination file stream.
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

    ////////////////////////////////////////////////
    // RESET for next loop
		// resets buffer and other values for next write
		// memset(buffer, '\0', BUFFER_SIZE);
		//bytes_read = 0;
		//bytes_written = 0;
    fflush(threadLogFP);
    fclose(threadLogFP);
	}

  fflush(mergedFP);
  fclose(mergedFP);
  return true;
}

typedef struct ThreadArg
{
  BankAccount *account = NULL;
  uint32_t threadIndex = 0;
} ThreadArg;

bool testDepositsWithdrawls(ThreadArg *tArgs)
{
 	Action depositAction = {.iterations=1, .amount=1000, .actionFunction=deposit};
	Action withdrawAction = {.iterations=1, .amount=-1000, .actionFunction=withdraw};

  // seeding the rand_r() to get consistent random behavior between threads.
    // rand_r required an unsigned_int* seed.
	srand(tArgs->threadIndex);
  depositAction.iterations = ( rand() % 4 ) + 1;
	srand (tArgs->threadIndex + 1);
  withdrawAction.iterations = ( rand() % 3 ) + 1;

	char logFileStrOut[30];
	getThreadLogFileName(tArgs->threadIndex, logFileStrOut, 30);

  // create/open file in write mode.
  FILE *logFile = fopen(logFileStrOut, "a");
	fprintf(logFile, "\n          File name: %s", logFileStrOut);
  fprintf(logFile, "\n           ThreadID: %lu", tArgs->threadIndex);
	fprintf(logFile, "\n Deposit Iterations: %llu", depositAction.iterations);
	fprintf(logFile, "\nWithdraw Iterations: %llu", withdrawAction.iterations);

  outputAndDoBankingAction(tArgs->account, &depositAction, logFile);
  outputAndDoBankingAction(tArgs->account, &withdrawAction, logFile);

  fclose(logFile);
	return true;
}

// Thread function (takes in BankAccount object, threadIDPtr number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
void* executeThread(void *args)
{
  // Cast void* to a ThreadArg*. Then dereference the pointer.
  ThreadArg threadArg = *(ThreadArg*)args;

	char logFileStrOut[30];
	// Trusting that we are being given a unique index.
	getThreadLogFileName(threadArg.threadIndex, logFileStrOut, 30);

	// Opening with read permissions to see if file exists.
		// Shouldn't open new file if it doesn't exist.
	/*FILE *logFP = fopen(logFileStrOut, "r"); <--- THIS LINE CAUSES A SEGMENTATION FAULT
	if (logFP != NULL)
	{
		char errorStr[300] = "\nLog File already exists: ";
		strcat(errorStr, logFileStrOut);
		char threadValues[200];
		snprintf(threadValues, 199, "\nThreadArg Values\nBalance=%lli\n  Index=%lu", threadArg.account->balance, threadArg.threadIndex);
		strcat(errorStr, threadValues);
		printErrorToErrorFile(errorStr);
	}
	*/
	// fclose(logFP);

	// Opening new log file

  // Debug Log Statements
  /*
	logFP = fopen(logFileStrOut, "w");
	fprintf(logFP, "\nLog File is unique. Passed.");
	fprintf(logFP, "\nThreadArg Values");
  fprintf(logFP, "\nBalance=%lli", threadArg.account->balance);
  fprintf(logFP, "\n  Index=%lu", threadArg.threadIndex);
  fflush(logFP);
  fclose(logFP);
	*/
  testDepositsWithdrawls(&threadArg);

  //  https://www.man7.org/linux/man-pages/man3/pthread_exit.3.html
  pthread_exit(0);
}

int main (int argv, char* argc[])
{
	// Reset error log file
	if (resetErrorLogFile() == false)
	{
		printErrorToErrorFile("Failed to reset error log file.");
	}

  // Command Line Args
	BankAccount account;
  createAccount(&account, 0);

  const uint32_t THREAD_COUNT = 4;
  pthread_t threads[THREAD_COUNT];

  fprintf(stdout, "\nSpinning up %lu threads", THREAD_COUNT);
	fflush(stdout);
  // spin up n threads
  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {
		// allocates memory the size of a ThreadArg struct.
			// casts the void* returned by malloc to a ThreadArg*.
    ThreadArg *argCopy = (ThreadArg*) malloc(sizeof(ThreadArg));

    // copy the thread arg to avoid any multi-threading race condition.
      // The thread will make its own copy and free the malloc: https://beej.us/guide/bgc/html/split/multithreading.html
    (*argCopy).account = &account;
		(*argCopy).threadIndex = i;

		// Debugging out statements
		fprintf(stdout, "\nPrethread Creation");
		fprintf(stdout, "\nAccount: %lli", argCopy->account->balance);
		fprintf(stdout, "\n  Index: %lu", argCopy->threadIndex);
		fflush(stdout);

		if (pthread_create(&threads[i], NULL, executeThread, (void*)argCopy) != 0)
    {
      fprintf(stdout, "FAILED TO CREATE PTHREAD. ThreadID: %lu", threads[i]);
      return 1;
    }

		fprintf(stdout, "\n Created thread: %lu", i);
  }

  // wait for n threads
  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {
    // wait for i'th thread in threads list
    pthread_join(threads[i], NULL);
		fprintf(stdout, "\nThread Exited: %lu", i);
  }

  // print final account balance
  fprintf(stdout, "\nFinal Balance: %lld\n", account.balance);

  // print merged transaction logs for analysis
	if (mergeThreadLogFiles(THREAD_COUNT) == false)
	{
		printErrorToErrorFile("Failed to merge thread log files.");
	}

  return 0;
}
