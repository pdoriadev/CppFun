#include <stdio.h>
#include <stdint.h>
#include <threads.h>
#include "BankAccount.hpp"

bool printBalance(BankAccount *account)
{
	fprintf(stdout, "\nBalance: %d", account->balance);
	return true;
}

typedef struct Action
{
	uint64_t iterations = 0;
	int64_t amount = 0;
	bool (*actionFunction)(BankAccount*, int64_t);
} Action;

bool printAndDoBankingAction(BankAccount *account, Action *action)
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

	for (uint64_t i = 0; i < action->iterations; i++)
	{
		printBalance(account);
		(*action->actionFunction)(account, action->amount);
		printBalance(account);
	}

	return true;
}

int main (int argv, char* argc[])
{
	BankAccount account;

	Action depositAction = {.iterations=4, .amount=1000, .actionFunction=deposit};
	Action withdrawAction = {.iterations=1, .amount=-1000, .actionFunction=withdraw}

	printAndDoBankingAction(&account, &depositAction);
	printAndDoBankingAction(&account, &withdrawAction);

	fprintf(stdout, "\nFinal Amount: %d\n", account.balance);
	fflush(stdout);

  // spin up n threads
  // wait for n threads

  // print final account balance
  // print merged transaction logs for analysis

	return 0;
}

// write this up later.
bool executeThread(BankAccount *account) 
{
	return true;
}
// Thread function (takes in BankAccount object, threadID number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
// - repeat the above in a for loop, then exit.


