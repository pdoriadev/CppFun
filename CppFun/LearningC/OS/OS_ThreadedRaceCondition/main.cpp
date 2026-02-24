#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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


bool testDepositsWithdrawls(BankAccount *account, uint32_t* threadID)
{
 	Action depositAction = {.iterations=1, .amount=1000, .actionFunction=deposit};
	Action withdrawAction = {.iterations=1, .amount=-1000, .actionFunction=withdraw};

  // seeding the rand_r() to get consistent random behavior between threads.
    // rand_r required an unsigned_int* seed.
  depositAction.iterations = ( rand_r((unsigned int*)threadID) % 4 ) + 1;
  withdrawAction.iterations = (  rand_r((unsigned int*)threadID) % 3 ) + 1;

  // Create Log File name for thread
  // append thread ID to end of log file name.
    // NOTE: Later found out I can call pthread_self to get the calling thread's ID.
  char logFileStr[20] = "Log_";
  char threadIDString[10];
  snprintf(threadIDString, 10, "%d", threadID);
  strncat(logFileStr, threadIDString, 11);
  strncat(logFileStr, ".txt", 5);

  // create/open file in write mode.
  FILE *logFile = fopen(logFileStr, "w+");

  // Deposits
  for (uint64_t i = 0; i < depositAction.iterations; i++)
  {
    outputAndDoBankingAction(account, &depositAction, logFile);
  }

  // Withdrawls
  for (uint64_t i = 0; i < withdrawAction.iterations; i++)
  {
    outputAndDoBankingAction(account, &withdrawAction, logFile);
  }

  fclose(logFile);
	return true;
}

typedef struct ThreadArg
{
  BankAccount *account;
  uint32_t* threadID;
} ThreadArg;

// Thread function (takes in BankAccount object, threadID number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
void* executeThread(void *args)
{
  // Cast void* to a ThreadArg*. Then dereference the pointer.
  ThreadArg threadArg = *(ThreadArg*)args;
  //free(args);
  testDepositsWithdrawls(threadArg.account, threadArg.threadID);

  pthread_exit(0); // https://www.man7.org/linux/man-pages/man3/pthread_exit.3.html
}

int main (int argv, char* argc[])
{
  // Command Line Args
	BankAccount account;
  createAccount(&account, 0);

  const uint32_t THREAD_COUNT = 4;
  pthread_t threads[THREAD_COUNT];

  // spin up n threads
  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {

		// allocates memory the size of a ThreadArg struct.
			// casts the void* returned by malloc to a ThreadArg*.
    ThreadArg *argCopy = (ThreadArg*) malloc(sizeof(ThreadArg));
    // copy the thread arg to avoid any multi-threading race condition.
      // The thread will make its own copy and free the malloc: https://beej.us/guide/bgc/html/split/multithreading.html
    argCopy->account  = &account;
		argCopy->threadID = (uint32_t*)&threads[i];

    if (pthread_create(&threads[i], NULL, executeThread, &argCopy) != 0)
    {
      fprintf(stdout, "FAILED TO CREATE P_THREAD. ThreadID: %d", threads[i]);
      return 1;
    }
		fprintf(stdout, "\n Started thread :%d", (uint32_t)threads[i]);
		sleep(1);
  }

  // wait for n threads
  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {
    // wait for i'th thread in threads list
    pthread_join(threads[i], NULL);
		fprintf(stdout, "\nThread complete: %d", (uint32_t)threads[i]);
  }

  // print final account balance
  fprintf(stdout, "\nFinal Balance: %d\n", account.balance);

  // print merged transaction logs for analysis

  return 0;
}
