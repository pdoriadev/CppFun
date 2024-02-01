#include "Bomb.h"
#include "CountdownClock.h"

result Bomb::countdown(unsigned int _countFrom = 5) {
	if (dead) {
		return result(false, "The bomb is dead. No more boom boom.");
	}

	clock.setExternalReceiver(r);
	clock.countdown(_countFrom); // problem, even if the clock sends the info, it is still holding up the entire line. main() or bomb can't do anything. hmm. 
								// separate threads?? should the receiver tell the receiver's entity to do something? Make make an interface that accomplishes that?
	if (r.finished) {
		std::cout << "fssss ";
		for (int i = 0; i < 5; i++) {
			std::cout << ". ";
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		std::cout << messageOnBoom << '\n';
	}

	return result(true, "The bomb has boomed");
}