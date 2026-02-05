
//////////////////////////////////////////////////////////////////////
// ASSIGNMENT PROMPT
// Write a program  on that does the following:
	// uses fork()

	// display process ID, parent process ID;

	// Run program 3 times individually on their own machine

 	//  Record outputs in a table

 	//  Note any differences between runs and save your screenshots

//  fork(), pid, wait
#include <sys/wait.h>
// stat(), for getting a file size.
#include <sys/stat.h>
// i/o - printf, fprintf
#include <stdio.h>
// uint64_t
#include <stdint.h>
// exit() and exit macros
#include <stdlib.h>

//////////////////////////////////////////////////////////
// SETUP TABLE IF FILE IS EMPTY.
/////////////////////////////////////////////////////////
void setupTable(FILE * tableFile, const char * tableFilePath)
{
	const uint16_t  width = 15;
	struct stat statBuffer;
	stat(tableFilePath, &statBuffer);
	if ((uint64_t)statBuffer.st_size == 0)
	{
		// opening file in append mode
		tableFile = fopen(tableFilePath, "a");

		fprintf(tableFile, "%*s", width / 2, "Run #");
		fprintf(tableFile, "%*s", width, "Child ID, Parent ID");
		fprintf(tableFile, "%*s", width, "Order");
		fprintf(tableFile, "%*s", width, "Notes");
		fprintf(tableFile, "\n"); // new line for first entry
		fflush(tableFile);

		fclose(tableFile);
	}
}


// The best fun guide to fork(): https://beej.us/guide/bgipc/html/split/fork.html
// fork() linux manual reference: https://www.man7.org/linux/man-pages/man2/fork.2.html
// Additional Notes:
	// The child pid returned by the child is the pid that the child knows about.
	// The first child's pid will be 0. The second's will be 1, and so on.
	// pid_t is a signed integer type: https://man.archlinux.org/man/pid_t.3type.en

//////////////////////////////////////////////////////////
// MAIN PROGRAM - Process Lifetime, Appending to Table
/////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	//////////////////////////////////////////////////////////
	// Table File manipulation
	FILE * tableFile = NULL;
	const char * tableFilePath = "PID_Table.txt";
	const uint16_t width = 15;
	//////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// Child creation and process code
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
		// avoids flushing before/after other parent print statements. 
		fflush(stdout);
		exit(EXIT_SUCCESS);
	}

	////////////////////////////////////////////////////////
	// Parent Process Code Starts
	fprintf(stdout, "\nParent process waiting.");
	fflush(stdout);
	////////////////////////////////////////////////////////
	// Parent waits for Child
	int childExitStatus;
	waitpid(childID, &childExitStatus, 0);
	if (childExitStatus == EXIT_FAILURE)
	{
		fprintf(stderr, "\nError waiting for child process. Exit Status Code: %llu", (uint64_t)childExitStatus);
		exit(EXIT_FAILURE);
	}

	///////////////////////////////////////////////////////
	// Append PID data to table
	fprintf(stdout, "\n\nParent process resumes. \nID: %llu\nChild's Real ID: %llu", (uint64_t)childID, (uint64_t)getpid());

	tableFile = fopen(tableFilePath, "a"); \
	if (tableFile == NULL)
	{
		fprintf(stderr, "Failed to open table file.");
	}


	uint16_t runCount = 0;
	{
		char stringBuffer[1024];
		// for every time we have not reached the end of the file, up the run counter.
		for(; feof(tableFile) != EOF; runCount++)
		{
			//reads line from file
			fgets(stringBuffer, sizeof stringBuffer, tableFile);
		}
	}

	fprintf(tableFile, "\n%d", runCount);
	fprintf(tableFile, "%*llu,", width/2 - 1, (uint64_t)childID);
	fprintf(tableFile, "%*llu", width/2, (uint64_t)getpid());
	fprintf(tableFile, "\n"); // new line for next entry
	fflush(tableFile);
	fclose(tableFile);


 	fprintf(stdout, "\nParent Process exiting.\n");
	fflush(stdout);

	return 0;
}
