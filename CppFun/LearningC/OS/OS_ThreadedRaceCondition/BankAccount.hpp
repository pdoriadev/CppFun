#include <stdint.h>
#include <limits.h>

typedef struct BankAccount
{
	uint64_t balance = 0;
} BankAccount;

bool deposit(BankAccount *account, uint64_t amount)
{
	if (account.balance + amount > ULLONG_MAX)
	{
		return false;
	}
	&account.balance += amount;
	return true;
}

bool withdraw(BankAccount *account, uint64_t amount)
{
	if (account.balance - amount < 0)
	{
		return false;
	}
	&account.balance -= amount;
	return true;
}


