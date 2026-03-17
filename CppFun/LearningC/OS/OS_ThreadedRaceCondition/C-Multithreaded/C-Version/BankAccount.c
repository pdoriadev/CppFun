#include "BankAccount.h"

typedef struct BankAccount
{
	int64_t balance;
} BankAccount;

bool createAccount(BankAccount *account, const int64_t initialValue)
{
	if (account == NULL)
	{
		return false;
	}
	account->balance = initialValue;
	return true;
}

bool deposit(BankAccount *account, int64_t amount)
{
	if (account == NULL)
	{
		return false;
	}

  if (amount < 0)
	{
		// deposit cannot be a negative amount
		return false;
	}

	if (account->balance + amount > LLONG_MAX)
	{
		// cannot exceed more money than is possible
		amount = LLONG_MAX - amount;
	}

	account->balance += amount;
	return true;
}

bool withdraw(BankAccount *account, int64_t amount)
{
	if (account == NULL)
	{
		return false;
	}

	if (amount > 0)
	{
		// withdrawl cannot be a positive amount
		return false;
	}

	if (account->balance - amount < LLONG_MIN)
	{
		amount = LLONG_MIN - amount;
	}

	account->balance += amount;
	return true;
}


typedef struct ThreadArg
{
  BankAccount *account;
  uint32_t threadIndex;
  uint32_t runCount;
  uint32_t deposits;
  uint32_t withdrawls;
} ThreadArg;

bool initThreadArg(ThreadArg *arg)
{
	arg->account = NULL;
	arg->threadIndex = 0;
	arg->runCount = 0;
	arg->deposits = 0;
	arg->withdrawls = 0;

	return true;
}

