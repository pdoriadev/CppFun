// blah.c

#include <stdio.h>
#include <time.h>

int main(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	fprintf(stdout, "\nTime: %d\n", (size_t)ts.tv_sec);
	fflush(stdout);

	return 0;
}
