#include <sys/wait.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
int main() {
	pid_t pid;
    	// Fork a child process. Create a child process of this process.
	// Returns 0 to a child process. Returns child's actual id to the parent.
	pid = fork();

	// Error occurred
	if (pid < 0)
	{
        	std::cerr << "Fork Failed" << std::endl;
	        return 1;
    	}
	// Child process. This assumes the process has no other pids.
    	else if (pid == 0)
	{
        	execlp("/bin/ls", "ls", NULL);
    	}
	// Parent process
    	else
	{
		// Parent will wait for the child to complete.
        	// If this line isn't here, then the parent won't catch the child. The child will hang out,
		// wait allows the parent to wait until child completion. Without wait, the child process
		// will become a zombie.
		wait(NULL);
        	std::cout << "Child Complete" << std::endl;
    	}
	return 0;
}
