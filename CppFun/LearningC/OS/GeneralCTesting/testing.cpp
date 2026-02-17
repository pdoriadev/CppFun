// i/o
#include <stdio.h>
// stat function to get a stat struct. stat struct has file size.
#include <sys/stat.h>
// uint64
#include <stdint.h>

void printFileSizeToFile()
{
	FILE *fp = fopen("testing.txt", "w+");

	struct stat statBuffer;
	stat("testing.txt", &statBuffer);

	// st_size is a signed integer off_t type.https://www.man7.org/linux/man-pages/man3/off_t.3type.html
	printf("File size: %llu", (uint64_t)statBuffer.st_size);

	// Close out function
	printf("\n");
	fclose(fp);
}


// For waiting for processess. 
#include <sys/wait.h>
// exit() and exit() macros.
#include <stdlib.h>

// The best guide for beginner fork(): https://beej.us/guide/bgipc/html/split/fork.html
void parentChildProcessTest()
{
	printf("\nParent process START\n");
	// Even though this is declared before, the parent and child have a diferent address
	// space. Therefore, this variable is located in a completely different address
	// space for each process. Each process has its own COPY of each variable.
	int childProcessStatus;

	// Create a Child Process
	// fork() does not duplicate a program. It duplicates a process.
	// fork() creates a duplicate *process* based on the calling process' current state.
	// Once the duplicate process is created, fork() returns a pid. The parent/calling 
	// process receives the child's real Process Identification number (PID). The child
	// receives a different PID. The child's PID is how many children the parent 
	// process has had. The first child receives 0. The second receives 1, etc.
	pid_t childPID = fork();

	// error in child  creation
	if (childPID < 0)
	{
		fprintf(stderr, "\nFailed to have a child.\n");
		exit(EXIT_FAILURE);
	}
	// I am a child process
	else if (childPID == 0)
	{
		printf("\n");
		printf("\nI am a child! A sweet beautiful child! I want to sing-THE WORLD IS BEAUTIFUL LIKE ME!");
		printf("\nMy pid is: %llu.", (uint64_t)childPID);
		printf("\nMy parent's pid is: %llu.\n", (uint64_t)getppid());
		fflush(stdout);
		// demonstrates childProcessStatus is a copied variable from the parent instead of a shared one.
		childProcessStatus = 0;
		exit(childProcessStatus);
	}
	// I am a parent process
	else
	{
		printf("\n");
		printf("\nI am the parent. I have a silly child.");
		printf("\nMy child's real pid is: %llu.", (uint64_t)childPID);
		printf("\nMy pid is: %llu.", (uint64_t)getpid());
		printf("\nI wait for my child to die.");
		fflush(stdout);

		// https://www.man7.org/linux/man-pages/man2/waitpid.2.html
			// "The wait() system call suspends execution of the calling thread
	       		// until one of its children terminates.  The call wait(&wstatus) is
	       		// equivalent to: waitpid(-1, &wstatus, 0);"
			//
			// I think this would work the same as wait(&realAddress): waitPID = wait(NULL);

		// Waits for child process to exit. 
		// pidOfProcessIWantToWaitFor, process'CurrentState, 0 = noOptionsChosen
		pid_t waitPID;
		waitPID = waitpid(childPID, &childProcessStatus, 0);
		if (waitPID == -1)
		{
			fprintf(stderr, "\nWait Call Failed.\n");
			exit(EXIT_FAILURE);
		}

		printf("\nParent: The waiting is done. My child's exit status is: %d:", WEXITSTATUS(childProcessStatus));
		printf("\nFinally, I can go home and sleep.\n");
	}

	fprintf(stdout, "\nProcess Exit. ID: %llu", getpid());
	fprintf(stdout, "\n\n");
}

// argc is a value. Passing argv, a pointer to an array of chars - or put another way, an array of strings.
void commandLineStringProcessingTest(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		fprintf(stdout, "\narg %d: %s", i, argv[i]);
	}
	fprintf(stdout, "\n");
}

void commandLineIntProcessingTest_Sum(int argc, char* argv[])
{
	uint64_t sum = 0;
	for (uint16_t i = 0; i < argc; i++)
	{
		sum += (uint64_t)atoi(argv[i]);
	}
	fprintf(stdout, "Sum = %d\n", sum);
}

int main(int argc,char* argv[])
{
	// write the function call for  whichever funtionality you want to test.
	commandLineIntProcessingTest_Sum(argc, argv);
	return 0;
}
