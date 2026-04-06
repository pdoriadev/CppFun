#include <stdio.h>
// int types
#include <stdint.h>
// malloc
#include <stdlib.h>
#include <string.h>
// exit()
#include <unistd.h>
// threads
#include <pthread.h>
// clock_gettime
#include <time.h>
////////////////////////
// User-defined headers
#include "BankAccount.h"
#include "AccountActions.h"
#include "Errorlog.h"
#include "ThreadLog.h"

/*
///////////////////////////////
APPLICATION SUMMARY
//////////////////////////////
Simulates a race condition by having multiple threads operate on the same
	struct object.
Logs the results of each thread's operations to a separate log file.
Merges all log files into one log file by the end.

--- ADDITIONAL REQUIREMENTS  ---
Convert cpp/hpp files to c/h files. 					                       *CHECK*
Move header function bodies into c source files. 	                   *CHECK*
  - Only prototypes in headers. 	                                   *CHECK*
  - https://beej.us/guide/bgc/html/split/multifile-projects.html#multifile-projects
Create a make file for compilation/linking				                   *CHECK*

Write log files in separate directory.					                     *CHECK*
Rewrite without race condition.						                           **
- Operates on "n" bank accounts. One thread per bank account
- mutexes, etc.

Create sequential version.												                    **
Profile sequential.																                    **
Profile multi-threaded.														                    **

Look into static/extern use-caes									                                            **

*/

void nowInMicroseconds(uint64_t *useconds)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  *useconds = (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
}

bool testDepositsWithdrawls(ThreadArg *tArgs)
{
  // Open file. Create the file if it is the first run.
	char logFileStrOut[30];
	getThreadLogFileName(tArgs->threadIndex, logFileStrOut, 30);
  FILE *logFile;
  if (tArgs->runCount == 0)
	{
		logFile = fopen(logFileStrOut, "w");
	}
	else
	{
		logFile = fopen(logFileStrOut, "a");
	}

	if (logFile == NULL)
	{
		printErrorToErrorFile("Failed to open thread log file: ");
	  return false;
	}

  ////////////////////////////////////////////////////////
  // Write to file. Perform withdrawls/deposits.
  uint64_t expectedFinalBalance = tArgs->account->balance + (tArgs->deposits - tArgs->withdrawls)*1000;
  fprintf(logFile, "\n==========================================================");
  fprintf(logFile, "\n          File name: %s", logFileStrOut);
  fprintf(logFile, "\n                Run: %lu", tArgs->runCount);
  fprintf(logFile, "\n           ThreadID: %lu", tArgs->threadIndex);
  fprintf(logFile, "\n Deposit Iterations: %llu", tArgs->deposits);
  fprintf(logFile, "\nWithdraw Iterations: %llu", tArgs->withdrawls);
  if (rand() % 2 == 0)
  {
  	sleep(1);
  }

  Action depositAction = {.iterations=tArgs->deposits, .amount=1000, .actionFunction=deposit};
  Action withdrawAction = {.iterations=tArgs->withdrawls, .amount=-1000, .actionFunction=withdraw};
  outputAndDoBankingAction(tArgs->account, &depositAction, NULL); 
	outputAndDoBankingAction(tArgs->account, &withdrawAction, NULL);

  fprintf(logFile, "\n      Final Balance: %llu", tArgs->account->balance);
  fprintf(logFile, "\n   Expected Balance: %llu", expectedFinalBalance);

  fflush(logFile);
  fclose(logFile);
	return true;
}

// Thread function (takes in BankAccount object, threadIDPtr number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
void* runThreadedLoggingRaceCondition(void *args)
{
  // Cast void* to a ThreadArg*. Then dereference the pointer.
  ThreadArg threadArg = *(ThreadArg*)args;

  char logFileStrOut[30];
	// Trusting that we are being given a unique index.
  getThreadLogFileName(threadArg.threadIndex, logFileStrOut, 30);

  if (testDepositsWithdrawls(&threadArg) == false)
  {
		exit(EXIT_FAILURE);
  }

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

  const uint32_t RUNS_TARGET = 30;
  const uint32_t THREAD_COUNT = 32;
  pthread_t threads[THREAD_COUNT];
  char finalExpectedBalancesAllRuns[10000] = "";
  for (uint32_t runCount = 0; runCount < RUNS_TARGET; runCount++)
  {
		uint32_t totalDeposits = 0;
    uint32_t totalWithdrawls = 0;
    // malloc BankAccount
    BankAccount *accountPtr = (BankAccount*)(malloc(sizeof(BankAccount)));
    createAccount(accountPtr, 0);

    fprintf(stdout, "\nSpinning up %lu threads for run %lu.", THREAD_COUNT, runCount);
    fflush(stdout);

    // spin up n threads
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
      // allocates memory the size of a ThreadArg struct.
	  		// casts the void* returned by malloc to a ThreadArg*.
      ThreadArg *argCopy = (ThreadArg*) malloc(sizeof(ThreadArg));
			initThreadArg(argCopy);

      // copy the thread arg to avoid any multi-threading race condition.
        // The thread will make its own copy and free the malloc: https://beej.us/guide/bgc/html/split/multithreading.html
      argCopy->account = accountPtr;
      argCopy->threadIndex = i;
      argCopy->runCount = runCount;

      // seeding the rand_r() to get consistent random behavior between threads.
        // rand_r required an unsigned_int* seed.
      uint64_t nowOut;
      nowInMicroseconds(&nowOut);
      srand((uint64_t)i + nowOut);
      argCopy->deposits = ( rand() % 8112 ) + 1;
      argCopy->withdrawls = ( rand() % 4096 ) + 1;

      totalDeposits += argCopy->deposits;
      totalWithdrawls += argCopy->withdrawls;

      if (pthread_create(&threads[i], NULL, runThreadedLoggingRaceCondition, (void*)argCopy) != 0)
      {
        fprintf(stdout, "FAILED TO CREATE PTHREAD. ThreadID: %lu", threads[i]);
        exit(EXIT_FAILURE);
      }

      // fprintf(stdout, "\nCreated thread: %lu", i);
    }

    // wait for n threads
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
      // wait for i'th thread in threads list
      pthread_join(threads[i], NULL);
      // fprintf(stdout, "\nThread Exited: %lu", i);
    }

    // Final vs. Expected Balance Strings and Output
    uint64_t expectedBalance = totalDeposits*1000 - totalWithdrawls*1000;
    char finalExpectedBalanceStr[200] = "\n\n             Run: ";
    char intStr[30];
    snprintf(intStr, 29, "%lu", runCount);
    strcat(finalExpectedBalanceStr, intStr);
    strcat(finalExpectedBalanceStr, "\n   Final Balance: ");
    snprintf(intStr, 29, "%llu", accountPtr->balance);
    strcat(finalExpectedBalanceStr, intStr);
    strcat(finalExpectedBalanceStr, "\nExpected Balance: ");
    snprintf(intStr, 29, "%llu", expectedBalance);
    strcat(finalExpectedBalanceStr, intStr);

    fprintf(stdout, finalExpectedBalanceStr);
    strcat(finalExpectedBalancesAllRuns, finalExpectedBalanceStr);

    // Clean-up malloc
    memset(accountPtr, 0, sizeof(BankAccount)); // sets accountPtr block to consant bytes
    free(accountPtr); // frees allocated memory.
    accountPtr = NULL;

  }

  ////////////////////////////////////////////////////////
    // CREATE MERGED LOG FILE. PRINT TO CONSOLE.
  const char BUFFER = 100;
  char mergeLogsStrOut [BUFFER];
  if (mergeThreadLogFiles(THREAD_COUNT, mergeLogsStrOut, BUFFER) == false)
  {
    printErrorToErrorFile("Failed to merge thread log files.");
  }

  if (copyFileToDestinationFile(mergeLogsStrOut, stdout) == false)
  {
    printErrorToErrorFile("Failed to copy merged log file contents to console");
  }

  fprintf(stdout, finalExpectedBalancesAllRuns);

  ///////////////////////////////////////////////////////
    // OUTPUT ANY ERRORS TO CONSOLE
  fprintf(stdout, "\n====== ERRORS =======");
  if (copyFileToDestinationFile("error.txt", stdout) == false)
  {
    printErrorToErrorFile("Failed to output error file contents to stdout");
  }
	fprintf(stdout, "\n");

  return 0;
}
