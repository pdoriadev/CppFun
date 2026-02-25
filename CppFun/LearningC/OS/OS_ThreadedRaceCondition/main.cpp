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

// merges the contents of all the log files into one
bool mergeLogFiles(FILE *mergedFP, char *logFileNames[], uint16_t bufferLength)
{

  return true;
}

///////////////////////////////////////////////
// Given an empty string and an index value, provides the correct log file name.
bool getThreadLogFileName(uint32_t threadIndex, char logFileStr[], uint16_t bufferLength)
{
	if (bufferLength < 20)
	{
		char errorTxt[] = "Thread Log File Name String's Buffer is insufficient size"; 
		printErrorToErrorFileThenExit(errorTxt);
	}

	// Create Log File name for thread
  // append thread ID to end of log file name.
  snprintf(logFileStr, 5, "Log_");
  char threadIDString[bufferLength-8];
  snprintf(threadIDString, bufferLength-7, "%lu", threadIndex);
	strcat(logFileStr, threadIDString);
  strcat(logFileStr, ".txt");

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

	char logFileStr[30];
	getThreadLogFileName(tArgs->threadIndex, logFileStr, 30);

  // create/open file in write mode.
  FILE *logFile = fopen(logFileStr, "a");
	fprintf(logFile, "\n          File name: %s", logFileStr);
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

	char logFileStr[30];
	// Trusting that we are being given a unique index.
	getThreadLogFileName(threadArg.threadIndex, logFileStr, 30);

	// Opening with read permissions to see if file exists.
		// Shouldn't open new file if it doesn't exist.
	/*FILE *logFP = fopen(logFileStr, "r"); <--- THIS LINE CAUSES A SEGMENTATION FAULT
	if (logFP != NULL)
	{
		char errorStr[300] = "\nLog File already exists: ";
		strcat(errorStr, logFileStr);
		char threadValues[200];
		snprintf(threadValues, 199, "\nThreadArg Values\nBalance=%lli\n  Index=%lu", threadArg.account->balance, threadArg.threadIndex);
		strcat(errorStr, threadValues);
		printErrorToErrorFileThenExit(errorStr);
	}
	*/
	// fclose(logFP);

	// Opening new log file

  // Debug Log Statements
  /*
	logFP = fopen(logFileStr, "w");
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
		printErrorToErrorFileThenExit("Failed to reset error log file.");
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

  return 0;
}
