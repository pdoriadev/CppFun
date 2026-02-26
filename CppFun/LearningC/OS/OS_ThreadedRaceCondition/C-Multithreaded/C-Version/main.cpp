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
////////////////////////
// User-defined headers
#include "BankAccount.hpp"
#include "AccountActions.hpp"
#include "Errorlog.hpp"
#include "ThreadLog.hpp"

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

  // Open file. Create the file if it is the first run.
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
		printErrorToErrorFile("Failed to open thread log file");
	}
	fprintf(logFile, "\n==========================================================");
	fprintf(logFile, "\n          File name: %s", logFileStrOut);
	fprintf(logFile, "\n                Run: %lu", tArgs->runCount);
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
void* runThreadedLoggingRaceCondition(void *args)
{
  // Cast void* to a ThreadArg*. Then dereference the pointer.
  ThreadArg threadArg = *(ThreadArg*)args;

	char logFileStrOut[30];
	// Trusting that we are being given a unique index.
	getThreadLogFileName(threadArg.threadIndex, logFileStrOut, 30);

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

	BankAccount account;
  createAccount(&account, 1000);

	const uint32_t RUNS_TARGET = 3;
  const uint32_t THREAD_COUNT = 100;
  pthread_t threads[THREAD_COUNT];

	for (uint32_t runCount = 0; runCount < RUNS_TARGET; runCount++)
	{
		fprintf(stdout, "\nSpinning up %lu threads for run %lu.", THREAD_COUNT, runCount);
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
		  (*argCopy).runCount = runCount;

		  if (pthread_create(&threads[i], NULL, runThreadedLoggingRaceCondition, (void*)argCopy) != 0)
      {
        fprintf(stdout, "FAILED TO CREATE PTHREAD. ThreadID: %lu", threads[i]);
        return 1;
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

    // print merged transaction logs for analysis
    const char BUFFER = 100;
    char mergeLogsStrOut [BUFFER];
	  if (mergeThreadLogFiles(THREAD_COUNT, mergeLogsStrOut, BUFFER) == false)
	  {
		  printErrorToErrorFile("Failed to merge thread log files.");
	  }

		
    // print final account balance
    // fprintf(stdout, "\nFinal Balance: %lld\n", account.balance);

	}


  return 0;
}
