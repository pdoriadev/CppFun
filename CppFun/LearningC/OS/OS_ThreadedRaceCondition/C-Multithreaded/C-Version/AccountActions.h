#ifndef ACCOUNT_ACTIONS_H
#define ACCOUNT_ACTIONS_H
#include <stdio.h>
	// File io
#include <stdint.h>
	// int types
#include <unistd.h>
	// exit()
#include "BankAccount.h"
#include <pthread.h>
	// mutex 

bool outputBalance(BankAccount*, FILE*);

typedef struct Action
{
  uint64_t iterations;
  int64_t amount;
  bool (*actionFunction)(BankAccount*, int64_t, pthread_mutex_t*);
} Action;

bool initAction(Action*);

// Assumes the outputFP is open and writable!
// Does not close the outputFP!
bool outputAndDoBankingAction(BankAccount*, Action *, pthread_mutex_t * mtx, FILE*);

#endif
