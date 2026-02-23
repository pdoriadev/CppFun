#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "BankAccount.hpp"

bool outputBalance(BankAccount *account, FILE* outputFile)
{
	fprintf(outputFile, "\nBalance: %d", account->balance);
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
    outputFile = stdout;
  }

	for (uint64_t i = 0; i < action->iterations; i++)
	{
		outputBalance(account, outputFile);
		if ((*action->actionFunction)(account, action->amount) == false)
		{
			return false;
		}
		outputBalance(account, outputFile);
	}

	return true;
}

// write this up later.
bool testDepositsWithdrawls(BankAccount *account, Action deposit, Action withdraw, thrd_t threadID)
{
 	Action depositAction = {.iterations=1, .amount=1000, .actionFunction=deposit};
	Action withdrawAction = {.iterations=1, .amount=-1000, .actionFunction=withdraw};

  // seeding the rand_r() to get consistent random behavior between threads.
  deposit.iterations = (rand_r(threadID) % 4) + 1;
  withdraw.iterations = (rand_r(threadID) % 3) + 1;

  // Create Log File name for thread
  // append thread ID to end of log file name.
  char logFileString[20] = "Log_";
  char threadIDString[10];
  snprintf(threadIDString, 10, %d, threadID);
  strncat(logFileString, threadIDString, 10);
  strncat(logFileString, ".txt", 4);

  // create/open file in write mode.
  FILE *logFile = fopen(outputNameString, w+);
  // pass file pointer to printAndDo functions.

  for (uint64_t i = 0; i < deposit.iterations; i++)
  {
    outputAndDoBankingAction(account, deposit, logFile);
  }

  for (uint64_t i = 0; i < withdraw.iterations; i++)
  {
    outputAndDoBankingAction(account, withdraw, logFile);
  }

  fclose(logFile);
	return true;
}

typedef struct ThreadArg
{
  BankAccount *account;
  thrd_t threadID
} ThreadArg;


// Thread function (takes in BankAccount object, threadID number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
int executeThread(void *args)
{
  // Cast args back to a usable type
  ThreadArg threadArg = *(ThreadArg*)arg;
  testDepositsWithdrawls(threadArg.account, depositAction, threadArg.threadID);

  // thread exit
}

int main (int argv, char* argc[])
{
  // Command Line Args
	BankAccount account;

  const uint32_t THREAD_COUNT = 4;
  thrd_t threads[THREAD_COUNT];

  // spin up n threads
  for (uint8_t i = 0; i < THREAD_COUNT; i++)
  {
    // copy the thread arg to avoid any multi-threading race condition.
      // The thread will make its own copy and free the malloc: https://beej.us/guide/bgc/html/split/multithreading.html
    ThreadArg *threadSpecificArg = malloc(sizeof *threadSpecificArg);
    threadSpecificArg = {.account=account, .threadID = threads[i]};

    if (pthread_create(threads[i], NULL, executeThread, threadSpecificArg) != 0)
    {
      fprintf(stdout, "FAILED TO CREATE P_THREAD. ThreadID: %d", threads[i]);
      return 1;
    }
  }

  // wait for n threads
  for (uint8_t i = 0; i < THREAD_COUNT; i++)
  {
    // wait for i'th thread in threads list
    pthread_join(threads[i], NULL);
  }

  // print final account balance
  outputBalance(stdout, "\nFinal Balance: %d\n", account->balance);

  // print merged transaction logs for analysis

  return 0;
}


