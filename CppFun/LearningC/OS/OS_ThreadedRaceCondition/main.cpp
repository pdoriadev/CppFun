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

bool outputBalance(BankAccount *account, FILE* outputFile)
{
	fprintf(outputFile, "\nBalance: $%d", account->balance);
	return true;
}

typedef struct Action
{
	uint64_t iterations = 0;
	int64_t amount = 0;
	bool (*actionFunction)(BankAccount*, int64_t);
} Action;

bool outputAndDoBankingAction(BankAccount *account, Action *action, FILE *outputFile)
{
	if (action->actionFunction == NULL)
	{
		// void ptr points to NULL
		return false;
	}

	if (action->actionFunction != deposit && action->actionFunction != withdraw)
	{
		// must point to a valid action function
		return false;
	}

  if (outputFile == NULL)
  {
    fprintf(stdout, "Output File is NULL");
    fflush(stdout);
    exit(1);
  }

	for (uint64_t i = 0; i < action->iterations; i++)
	{
		if ((*action->actionFunction)(account, action->amount) == false)
		{
			return false;
		}
		outputBalance(account, outputFile);
	}

	return true;
}


bool testDepositsWithdrawls(BankAccount *account)
{
 	Action depositAction = {.iterations=1, .amount=1000, .actionFunction=deposit};
	Action withdrawAction = {.iterations=1, .amount=-1000, .actionFunction=withdraw};

  // seeding the rand_r() to get consistent random behavior between threads.
    // rand_r required an unsigned_int* seed.
	srand((uint32_t)pthread_self());
  depositAction.iterations = ( rand() % 4 ) + 1;
	srand ((uint32_t)pthread_self() + 1);
  withdrawAction.iterations = ( rand() % 3 ) + 1;

  // Create Log File name for thread
  // append thread ID to end of log file name.
    // NOTE: Later found out I can call pthread_self to get the calling thread's ID.
  char logFileStr[30] = "Log_";
  char threadIDString[30];
  snprintf(threadIDString, 20, "%lu", pthread_self());
  // const char LENGTH_CHECK[30] = { *threadIDString };
	// size_t length = strlen(LENGTH_CHECK);
	strcat(logFileStr, threadIDString);
  strcat(logFileStr, ".txt");

  // create/open file in write mode.
  FILE *logFile = fopen(logFileStr, "w+");
	fprintf(logFile, "\n          File name: %s", logFileStr);
	fprintf(logFile, "\n    ThreadID string: %s", threadIDString);
	//fprintf(logFile, "\nThreadID number: %lu", *threadIDPtr);
  fprintf(logFile, "\n  pthread_self() id: %lu", (uint32_t)pthread_self());
	fprintf(logFile, "\n Deposit Iterations: %llu", depositAction.iterations);
	fprintf(logFile, "\nWithdraw Iterations: %llu", withdrawAction.iterations);

  outputAndDoBankingAction(account, &depositAction, logFile);
  outputAndDoBankingAction(account, &withdrawAction, logFile);

  fclose(logFile);
	return true;
}

typedef struct ThreadArg
{
  BankAccount *account = NULL;
  uint32_t* threadIDPtr = NULL;
  char **logFileNamePtr = NULL;
} ThreadArg;

// Thread function (takes in BankAccount object, threadIDPtr number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
void* executeThread(void *args)
{
  // Cast void* to a ThreadArg*. Then dereference the pointer.
  ThreadArg threadArg = *(ThreadArg*)args;
  testDepositsWithdrawls(threadArg.account);

  //  https://www.man7.org/linux/man-pages/man3/pthread_exit.3.html
  pthread_exit(0);
}

// writes a log file name to an empty string
bool createLogFileName(char logFileName[], uint16_t bufferLength)
{

  return true;
}

// merges the contents of all the log files into one
bool mergeLogFiles(FILE *mergedFP, char *logFileNames[], uint16_t bufferLength)
{

  return true;
}

int main (int argv, char* argc[])
{
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
		(*argCopy).threadIDPtr = (uint32_t*)&threads[i];

		if (pthread_create(&threads[i], NULL, executeThread, &argCopy) != 0)
    {
      fprintf(stdout, "FAILED TO CREATE PTHREAD. ThreadID: %lu", threads[i]);
      return 1;
    }

		fprintf(stdout, "\n Started thread: %lu", (uint32_t)threads[i]);
		// sleep(1);
  }

  // wait for n threads
  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {
    // wait for i'th thread in threads list
    pthread_join(threads[i], NULL);
		fprintf(stdout, "\nThread complete: %lu", (uint32_t)threads[i]);
  }

  // print final account balance
  fprintf(stdout, "\nFinal Balance: %llu\n", account.balance);

  // print merged transaction logs for analysis

  return 0;
}
