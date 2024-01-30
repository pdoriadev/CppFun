#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
int main()
{
	std::time_t timeInSeconds = std::time(nullptr);
	int countDown = 10;
	std::cout << countDown << '\n';
	while (true) {
		if (std::time(nullptr) > timeInSeconds)
		{
			timeInSeconds = std::time(nullptr);
			countDown--;
			std::cout << countDown << '\n';
		}

		if (countDown == 0)
			break;
	}
	
	for (int i = 0; i < 100; i++) {
		std::cout << "Welcome to the End" << '\n';
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	return 0;
}