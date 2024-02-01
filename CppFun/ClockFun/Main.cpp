/*
Bomb and alarm clock both use Countdown clock to count down until something happens.
A different message is output based on the if it's a bomb or alarm clock. 
*/
#include "Bomb.h"

int main()
{
	// countdown clock as a component of the bomb and the kitchen timer classes. 
	Bomb b;
	b.messageOnBoom = "BOOM BOOM BOOM";
	b.countdown(10);

	return 0;
}
