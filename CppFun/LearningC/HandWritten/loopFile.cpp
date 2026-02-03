#include <cstdio>
#include <time.h>
#include <cstdint>
#include <string>


void nowInMicroseconds(uint64_t &useconds)
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	useconds = (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
}

int main(int argc, char* argv[])
{
	uint64_t timeInMicroseconds;
	nowInMicroseconds(timeInMicroseconds);
	printf("Program Start: %lu\n\n", timeInMicroseconds);

	uint64_t loops = 1000;
	/*
	if (argc > 0)
	{
		loops = (uint64_t)std::stoull(argv[0]);
	}
	*/
	for(int i = 0; i < loops; i++ ) { }

	nowInMicroseconds(timeInMicroseconds);
	printf("Program End: %llu\n\n", timeInMicroseconds);

	return 0;
}
