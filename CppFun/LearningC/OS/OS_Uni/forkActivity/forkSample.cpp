#include <sys/wait.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>

int main() {
    pid_t pid;

    // Fork a child process
    pid = fork();
    if (pid == 0) { // child code
	std::cout << "Child's Parent process ID (PPID): "  << getppid() << std::endl;
	sleep(15);
	std::cout << "child's Parent process ID (PPID): " << getppid() << std::endl;
    }
    else if (pid > 0) { // parent code
	sleep(5); // sleep for 5 seconds
	std::cout << "Parent process ID: " << getpid() << std::endl;
    }

    return 0;
}
