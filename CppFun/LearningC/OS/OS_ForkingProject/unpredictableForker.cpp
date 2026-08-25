// i/o
#include <stdio.h>
// pid
#include <wait.h>
// uint types
#include <stdint.h>
// sleep
#include <unistd.h>
// standard library for exit() and exit macros
#include <stdlib.h>
int main(int argc, char* argv[])
{
	pid_t childPID = fork();
	if (childPID < 0)
	{
		fprintf(stdout, "Invalid childPID: %d", childPID);
		exit(1);
	}

	if (childPID != 0) // parent
	{
		int exitStatus;
		fprintf(stdout, "\nParent is waiting for child.");
		fflush(stdout);
		waitpid(childPID, &exitStatus, 0);
		if (exitStatus == EXIT_FAILURE)
		{
			fprintf(stdout, "\nExited with failure code: %d", exitStatus);
			fflush(stdout);
			exit(exitStatus);
		}
	}
	else // child
	{
		fprintf(stdout, "\nChild is sleeping");
		fflush(stdout);
		uint8_t sleepTime = 10;
		if (argc > 1)
		{
			// clamps sleep time to range 0-10
			sleepTime = (uint8_t)(atoi(argv[1]) % 11);
		}
		sleep(sleepTime);
	}
	fprintf(stdout, "\nMy id: %llu, Parent's id: %llu\n", (uint64_t)getpid(), (uint64_t)getppid());
	return 0;
}
