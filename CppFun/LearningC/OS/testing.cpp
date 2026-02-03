// i/o
#include <stdio.h>
// stat function to get a stat struct. stat struct has file size.
#include <sys/stat.h>
// uint64
#include <stdint.h>

void getFileSize()
{
	FILE *fp = fopen("testing.txt", "w+");
	struct stat statBuffer;
	stat("testing.txt", &statBuffer);

	// st_size is a signed integer off_t type.https://www.man7.org/linux/man-pages/man3/off_t.3type.html
	printf("File size: %llu", (uint64_t)statBuffer.st_size);
/*	if(statBuffer.st_size == 0)
	{
		printf("\nFile is empty.");
	}
*/
	printf("\n");

	fclose(fp);

}


// For waiting for processess. 
#include <sys/wait.h>
// exit() functionality and macros.
#include <stdlib.h>

void parentChildProcessTest()
{
	printf("\nParent process START\n");

	// Create Child Process
	pid_t childPID = fork();
	switch(childPID)
	{
	case -1: // error in child  creation
		fprintf(stderr, "\nFailed to have a child.\n");
		exit(EXIT_FAILURE);
	case 0: // child process
		printf("\n");
		printf("\nI am a child! A sweet beautiful child! I want to sing-THE WORLD IS BEAUTIFUL LIKE ME!");
		printf("\nMy name is: %llu.", (uint64_t)childPID);
		printf("\nMy parent's name is: %llu.\n", (uint64_t)getppid());
		fflush(stdout);
		return;
	default: // parent process
		printf("\n");
		printf("\nI am the parent. I have a silly child.");
		printf("\nMy child's real name is: %llu.", (uint64_t)childPID);
		printf("\nMy name is: %llu.", (uint64_t)getpid());
		printf("\nI wait for the child, now.");
		fflush(stdout);
		// https://www.man7.org/linux/man-pages/man2/waitpid.2.html
			// "The wait() system call suspends execution of the calling thread
	       		// until one of its children terminates.  The call wait(&wstatus) is
	       		// equivalent to: waitpid(-1, &wstatus, 0);"
		// pidOfProcessIWantToWaitFor, process'CurrentState, 0 = noOptionsChosen
		//waitPID = wait(NULL);
		pid_t waitPID;
		int childProcessStatus;
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

int main(int argc,char* argv[])
{
	// write the function call for  whichever funtionality you want to test.
	parentChildProcessTest();

	return 0;
}
