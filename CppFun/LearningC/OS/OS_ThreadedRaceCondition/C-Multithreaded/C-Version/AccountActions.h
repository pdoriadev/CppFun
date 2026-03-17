#ifndef ACCOUNT_ACTIONS_H
#define ACCOUNT_ACTIONS_H
// File io
#include <stdio.h>
// int types
#include <stdint.h>
// exit()
#include <unistd.h>
#include "BankAccount.h"

bool outputBalance(BankAccount*, FILE*);

typedef struct Action
{
  uint64_t iterations;
  int64_t amount;
  bool (*actionFunction)(BankAccount*, int64_t);
} Action;

bool initAction(Action*);

// Assumes the outputFP is open and writable!
// Does not close the outputFP!
bool outputAndDoBankingAction(BankAccount*, Action *, FILE*);

#endif
