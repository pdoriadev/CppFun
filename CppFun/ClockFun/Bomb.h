#pragma once
#include <string>
#include "CountdownClock.h"

/*
Problem - I only want bomb to be able to interface with clock, but I also want things outside the bomb to know what's going on with the clock
	so they can react appropriately. Also don't want the clock to have to know each different kind of thing it's sending information to. 
Solution - Create CountdownClockReceiver. An intermediary that is given to the clock object that can be contained in a bomb, alarm clock, etc. 
	The clock object updates the receiver. The class that has the recieverThe clock objectThe clock can send information to the reciever without knowing if it's sending information
		to a bomb or an alarm clock, etc. 
*/

class Bomb
{
public:
	result countdown(unsigned int _countFrom);
	std::string messageOnBoom = "BOOM";
	
private:	
	CountdownClock clock;
	CountdownClockReceiver r;
	bool dead = false;
};

