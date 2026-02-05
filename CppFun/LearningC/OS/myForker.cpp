
//////////////////////////////////////////////////////////////////////
// ASSIGNMENT PROMPT
// Write a program  on that does the following:
	// uses fork()

	// display process ID, parent process ID;

	// Run program 3 times individually on their own machine

 	//  Record outputs in a table

 	//  Note any differences between runs and save your screenshots

// fork - creates a new process by duplicating the current process.
#include <sys/types.h> 
//  wait 
#include <sys/wait.h>
// stat(), for getting a file size.
#include <sys/stat.h>
// i/o - printf, fprintf
#include <stdio.h>
// uint64_t
#include <stdint.h>
// exit() and exit macros
#include <stdlib.h>

// The best fun guide to fork(): https://beej.us/guide/bgipc/html/split/fork.html
// https://www.man7.org/linux/man-pages/man2/fork.2.html
	// the child pid returned by the child is the pid that the child knows about.
	// The first child's pid will be 0. The second's will be 1, and so on.
	// pid_t is a signed integer type: https://man.archlinux.org/man/pid_t.3type.en
int main(int argc, char* argv[])
{
	//////////////////////////////////////////////////////////
	// Table File manipulation
	const char * tableFilePath = "PID_Table.txt";
	FILE * tableFile = NULL;
	const uint16_t width = 15;
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// SETUP TABLE IF FILE IS EMPTY.
	/////////////////////////////////////////////////////////
	struct stat statBuffer;
	stat(tableFilePath, &statBuffer);
	if ((uint64_t)statBuffer.st_size == 0)
	{
		// opening file in append mode
		tableFile = fopen(tableFilePath, "a");

		fprintf(tableFile, "%*s", width, "Child ID");
		fprintf(tableFile, "%*s", width, "Real Child ID");
		fprintf(tableFile, "%*s", width, "Parent ID");
		fprintf(tableFile, "\n"); // new line for first entry
		fflush(tableFile);

		fclose(tableFile);
	}
	//////////////////////////////////////////////////////////
	// FINISHED TABLE SETUP
	/////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// MAIN PROGRAM - Process Lifetime, Appending to Table
	/////////////////////////////////////////////////////////
	pid_t childID = fork();
	// Error in child creation
	if (childID < 0)
	{
		fprintf(stderr, "Error in child creation. ChildID = %d", childID);
		exit(EXIT_FAILURE);
	}
	// Child process code
	else if (childID == 0)
	{
		fprintf(stdout, "\n\nChild process running. \nChild ID: %llu\nParent's ID: %llu", (uint64_t)childID, (uint64_t)getppid());

		// Append to table
		tableFile = fopen(tableFilePath, "a");
		// width -1 for the comma
		fprintf(tableFile, "%*llu,", width-1, (uint64_t)childID);
		fflush(tableFile);
		fclose(tableFile);

		fprintf(stdout, "\nChild process exiting.");
		fflush(stdout);
		exit(EXIT_SUCCESS);
	}
	// Parent process coe
	else
	{
		fprintf(stdout, "\nParent process waiting.");
		fflush(stdout);

		// Wait for Child
		int childExitStatus;
		waitpid(childID, &childExitStatus, 0);
		if (childExitStatus == EXIT_FAILURE)
		{
			fprintf(stderr, "\nError waiting for child process. Exit Status Code: %llu", (uint64_t)childExitStatus);
			exit(EXIT_FAILURE);
		}

		fprintf(stdout, "\n\nParent process resumes. \nID: %llu\nChild's Real ID: %llu", (uint64_t)childID, (uint64_t)getpid());

		// Append to table
		tableFile = fopen(tableFilePath, "a"); \
		// width -1 for the comma
		fprintf(tableFile, "%*llu,", width-1, (uint64_t)childID);
		fprintf(tableFile, "%*llu", width, (uint64_t)getpid());
		fprintf(tableFile, "\n"); // new line for next entry
		fflush(tableFile);
		fclose(tableFile);
	}

	fprintf(stdout, "\nParent Process exiting.\n");
	fflush(stdout);

	return 0;
}
