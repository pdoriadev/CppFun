#include "CountdownClock.h"
#include "CountdownClockReceiver.h"
#include "result.h"
#include <assert.h>

void CountdownClock::setExternalReceiver(CountdownClockReceiver r) { receiver = &r; }

result CountdownClock::countdown(unsigned int _count = 0) {
	startingCount = _count;
	remaining = _count;
	while (remaining > 0) {
		if (shouldReset) {
			remaining = startingCount;
		}
		
		receiver->updateInfo(countdownClockInfo(remaining, false));

		while (paused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
		remaining--;
	}

	if (remaining != 0) {
		assert(("Remaining count should equal 0", remaining != 0));
	}
	
	receiver->updateInfo(countdownClockInfo(remaining, true));

	return result(true, "Countdown complete");
}

result CountdownClock::resetCountdown() {
	if (cantReset)
		return result(false, "Cannot reset countdown");
	shouldReset = true;
	return result(true, "Resetting countdown");
}

result CountdownClock::pauseCountdown() {
	if (cantPause)
		return result(false, "Cannot pause countdown");
	paused = true;
	return result (true, "Paused countdown");
}

