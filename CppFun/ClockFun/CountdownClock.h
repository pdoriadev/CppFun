#pragma once
#include "CountdownClockReceiver.h"
#include "result.h"
#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
#include <string>

class CountdownClock {
public:
	void setExternalReceiver(CountdownClockReceiver r);
	result countdown(unsigned int);
	result resetCountdown();
	result pauseCountdown();

private:
	CountdownClockReceiver * receiver = NULL;
	unsigned int startingCount = 0;
	unsigned int remaining = 10;
	bool paused = false;
	bool cantPause = false;
	bool shouldReset = false;
	bool cantReset = false;
	std::time_t t;
};