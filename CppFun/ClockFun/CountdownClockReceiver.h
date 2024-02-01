#pragma once

/*
Problem - want things external to countdown clock receiver to get info from it without being able to edit the clock or the entity that relies on the clock. 

Solution - Create a receiver class that the clock can relay information to. Create an info class that can be used to pass information to other entities. 
*/
struct countdownClockInfo {
	unsigned int remaining = 0; 
	bool finished;
	countdownClockInfo(unsigned int _remaining, bool _finished) {
		remaining = _remaining;
		finished = _finished;
	}
};

class CountdownClockReceiver {
public:
	void updateInfo(countdownClockInfo);
	unsigned int remaining = 0;
	bool finished;
};

