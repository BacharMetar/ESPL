#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

// int debug = 0;

// a simplified implementation of the execute function from myshell.c
int execute(char *command, char **arguments) {
	pid_t pid; // process id of the child is 0
	if ((pid = fork()) == -1) {
		// if (debug) { // Should check for the debug flag even in these prints? or only the PID and Executing Command as written?
        	perror("Error in fork()");
			return -1;
		// }
	}
    else if (pid == 0) {
		if (execvp(command,arguments) < 0) {
			// if (debug) {
				perror("Error, cannot run the given command");
			// }
			if (stdin != NULL) {
				fclose(stdin);
			}
			if (stdout != NULL) {
				fclose(stdout);
			}
			_exit(1);
			return -1;
		}
		if (stdin != NULL) {
			fclose(stdin);
		}
		if (stdout != NULL) {
			fclose(stdout);
		}
	}
	else {
		// if (debug) {
		// 	fprintf(stderr, "PID: %d\n", pid);
		// 	fprintf(stderr, "Executing Command: %s\n", pCmdLine->arguments[0]);
		// }
		// if (pCmdLine->blocking) {
			pid_t child_pid;
			int status;
			child_pid = waitpid(pid, &status, 0); // waits for the child process
			if (child_pid == -1) {
				// if (debug) {
					perror("Error in waitpid()");
					return -1;
				// }
			}
		// }
	}
	return 0;
}

int main(int argc, char **argv)
{
	int fd[2]; // Stores the two ends of the pipe
	if (pipe(fd) == -1) {
		perror("Error in pipe()");
		return 1;
	}

	fprintf(stderr, "(parent_process>forking…)\n"); // DEBUG

	pid_t pid; // process id of the child is 0
	if ((pid = fork()) == -1) {
        perror("Error in fork()");
		return 1;
	}
    else if (pid == 0) { // first child process
		fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n"); // DEBUG
		fclose(stdout); // closes the standard writing end of the child process
		/*int writeEnd = */ dup(fd[1]); // duplicated the file descriptor of the writing end of the pipe
		close(fd[1]); // closed the original file descriptor of the writing end of the pipe
		
		// Now we can safely use writeEnd
		// Execute: ls -l
		char *command = "ls";
		char *arguments[] = {"ls", "-l", 0};
		// cmdLine *myCmdLine = parseCmdLines(command);
		// if (myCmdLine != NULL) {
			fprintf(stderr, "(child1>going to execute cmd: 'ls -l' …)\n"); // DEBUG
			execute(command, arguments);
			// execute(myCmdLine);
			// freeCmdLines(myCmdLine);
		// }
		return 0;
	}
	else { // parent process
		fprintf(stderr, "parent_process>created process with id: %d\n", pid); // DEBUG
		fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n"); // DEBUG
		close(fd[1]); // closes the writing end of the pipe
		pid_t pid_second; // process id of the child is 0
		if ((pid_second = fork()) == -1) {
			perror("Error in fork()");
			return 1;
		}
		else if (pid_second == 0) { // second child process
			fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe…)\n"); // DEBUG
			fclose(stdin); // closes the standard reading end of the child process
			/*int readEnd = */ dup(fd[0]); // duplicated the file descriptor of the reading end of the pipe
			close(fd[0]); // closed the original file descriptor of the reading end of the pipe
			
			// Now we can safely use readEnd
			// Execute: tail -n 2
			char *command = "tail";
			char *arguments[] = {"tail", "-n", "2", 0};
			// cmdLine *myCmdLine = parseCmdLines(command);
			// if (myCmdLine != NULL) {
				fprintf(stderr, "(child2>going to execute cmd: 'tail -n 2' …)\n"); // DEBUG
				execute(command, arguments);
				// execute(myCmdLine);
				// freeCmdLines(myCmdLine);
			// }
			return 0;
		}
		else { // parent process
			fprintf(stderr, "parent_process>created process with id: %d\n", pid_second); // DEBUG
			fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n"); // DEBUG
			close(fd[0]); // closes the reading end of the pipe
			pid_t child1_pid, child2_pid;
			int status;
			// printf("Waiting for child processes to finish execution to the pipe.\n");
			fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n"); // DEBUG
			child1_pid = waitpid(pid, &status, 0); // waits for the first child process to finish
			if (child1_pid == -1) {
					perror("Error in waitpid()");
					return 1;
			}
			child2_pid = waitpid(pid_second, &status, 0); // waits for the second child process to finish
			if (child2_pid == -1) {
					perror("Error in waitpid()");
					return 1;
			}

			// char buffer[64] = {0};
			// read(readEnd, buffer, 64);
			// printf("Received the message: %s\n", buffer);
			// printf("Exiting program.\n");
			// close(readEnd);
			fprintf(stderr, "(parent_process>exiting…)\n"); // DEBUG
			return 0;
		}
	}
	return 0;
}