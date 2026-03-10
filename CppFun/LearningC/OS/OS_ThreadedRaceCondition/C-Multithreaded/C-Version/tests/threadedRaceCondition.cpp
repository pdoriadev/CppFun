#include <stdio.h>
#include <stdint.h>
#include <threads.h>
#include "BankAccount.h"
ifdef __STDC_NO_THREADS__
#error I need threads!!!
#endif


int main(int argc, char* argv[])
{
	BankAccount account;
	// spin up n threads
	// wait for n threads

	// print final account balance
	// print merged transaction logs for analysis
}

// Thread function (takes in BankAccount object, threadID number)
// - threads operate on the same BankAccount object.
// - threads write to their own log file for each interaction
// - repeat the above in a for loop, then exit. 
