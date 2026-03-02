#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H
#include <stdint.h>
#include <limits.h>
#include <stddef.h>

typedef struct BankAccount
{
	int64_t balance;
} BankAccount;

bool createAccount(BankAccount*, const int64_t);

bool deposit(BankAccount*, int64_t);

bool withdraw(BankAccount*, int64_t);

typedef struct ThreadArg
{
  BankAccount *account;
  uint32_t threadIndex;
  uint32_t runCount;
  uint32_t deposits;
  uint32_t withdrawls;
} ThreadArg;

bool initThreadArg(ThreadArg*);

#endif
