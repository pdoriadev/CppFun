#include <iostream>
#include <cstdint>
#include <thread>
#include <vector>
#include <ctime>

class BankAccount
{
public:
	uint32_t balance = 0;

  BankAccount(uint32_t initialBalance = 1000)
  {
    balance = initialBalance;
  }

  bool deposit(uint32_t amount)
  {
    balance += amount;

    return true;
  }
  bool withdraw(uint32_t amount)
  {
    balance -= amount;

    return true;
  }
};


void runThread(uint16_t threadIndex, BankAccount &accountRef)
{
  // Compute # of Deposit/Withdrawl iterations
  std::srand(std::time(nullptr));
  uint8_t deposits = rand() % 6;
  uint8_t withdrawls = rand() % 3;

  for (uint8_t i = 0; i < deposits; i++)
  {
  	accountRef.deposit(1000);
  }
  for (uint8_t i = 0; i < withdrawls; i++)
  {
    accountRef.withdraw(250);
  }

	return;
}


int main(void)
{
  BankAccount account(1000);
  /////////////////////////////////////////////
	// Single-threaded operations
  ////////////////////////////////////////////
  account.deposit(500);
  account.withdraw(100);
  std::cout << account.balance << std::endl;

	//////////////////////////////////////////////
	// Multi-threaded operations
  /////////////////////////////////////////////
	const uint16_t THREAD_COUNT = 4;
	std::vector<std::thread> threads(4);

	// Spin up threads
	for (uint16_t i = 0; i < THREAD_COUNT; i++)
	{
	  threads[i] = std::thread(runThread, i, std::ref(account));
	}

	// Join threads
	for (uint16_t i = 0; i < THREAD_COUNT; i++)
	{
		threads[i].join();
	}

	std::cout << "Final Balance: " << account.balance << std::endl;

  return 0;
}
