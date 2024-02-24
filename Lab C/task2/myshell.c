#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include <linux/limits.h>
#include "LineParser.h"

int debug = 0;

int contains(char *string, char *const *arr, int size) {
	for(int i = 0; i < size; i++) {
		char *curr_str = arr[i];
		if(strcmp(string,curr_str) == 0) {
			return 1;
		}
	}
	return 0;
}

void handler(int sig)
{
	// printf("\nRecieved Signal : %s\n", strsignal(sig));
	if (sig == SIGTSTP)
	{
		signal(SIGTSTP, SIG_DFL);
	}
	else if (sig == SIGCONT)
	{
		signal(SIGCONT, SIG_DFL);
	}
	signal(sig, SIG_DFL);
	raise(sig);
}

int execute(cmdLine *pCmdLine) {
	char *arguments[pCmdLine->argCount+1]; // Pointer to the arguments array starting after the command itself
	memcpy(arguments,pCmdLine->arguments,sizeof(char*)*pCmdLine->argCount);
	arguments[pCmdLine->argCount] = '\0';

	pid_t pid; // process id of the child is 0
	if ((pid = fork()) == -1) {
		// if (debug) { // Should check for the debug flag even in these prints? or only the PID and Executing Command as written?
        	perror("Error in fork()");
			return -1;
		// }
	}
    else if (pid == 0) {
		if (pCmdLine->inputRedirect != NULL) {
			freopen(pCmdLine->inputRedirect, "r", stdin);
		}
		if (pCmdLine->outputRedirect != NULL) {
			freopen(pCmdLine->outputRedirect, "w", stdout);
		}
		if (execvp(pCmdLine->arguments[0],arguments) < 0) {
			// if (debug) {
				perror("Error, cannot run the given command");
			// }
			if (stdin != NULL) {
				fclose(stdin);
			}
			if (stdout != NULL) {
				fclose(stdout);
			}
			// _exit(1);
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
		if (debug) {
			fprintf(stderr, "PID: %d\n", pid);
			fprintf(stderr, "Executing Command: %s\n", pCmdLine->arguments[0]);
		}
		if (pCmdLine->blocking) {
			pid_t child_pid;
			int status;
			child_pid = waitpid(pid, &status, 0); // waits for the child process if the blocking flag is set to true (there was no & at the end of the command)
			if (child_pid == -1) {
				// if (debug) {
					perror("Error in waitpid()");
					return -1;
				// }
			}
		}
	}
	return 0;
}

int parseCommandExecuteIfNoPipe(cmdLine *commandLine) {
	if (contains("-d",commandLine->arguments,commandLine->argCount)) {
		debug = 1;
	}
	if (commandLine->argCount >= 1 && strcmp(commandLine->arguments[0],"cd") == 0) { // 'cd' command
		if (commandLine->argCount != 2) {
			perror("Too many arguments to 'cd' command");
		}
		else {
			if (chdir(commandLine->arguments[1]) < 0) {
				perror("Error in chdir()");
			}
		}	
	}
	else if (commandLine->argCount >= 1 && strcmp(commandLine->arguments[0],"wakeup") == 0) { // 'wakeup' command
		if (commandLine->argCount != 2) {
			perror("Too many arguments to 'wakeup' command");
		}
		else {
			char *endptr;
			long process_id = strtol(commandLine->arguments[1], &endptr, 10);
			if (*endptr != '\0') { // Check for conversion error
				perror("Error: Invalid characters given as process id");
			}
			else if (kill(process_id, SIGCONT) < 0) {
				perror("Error in kill()");
			}
		}	
	}
	else if (commandLine->argCount >= 1 && strcmp(commandLine->arguments[0],"nuke") == 0) { // 'nuke' command
		if (commandLine->argCount != 2) {
			perror("Too many arguments to 'nuke' command");
		}
		else {
			char *endptr;
			long process_id = strtol(commandLine->arguments[1], &endptr, 10);
			if (*endptr != '\0') { // Check for conversion error
				perror("Error: Invalid characters given as process id");
			}
			else if (kill(process_id, SIGKILL) < 0) {
				perror("Error in kill()");
			}
		}	
	}
	else {
		return 0;
	// 	fprintf(stderr, "TESTING1\n");
	// 	if (execute(commandLine) < 0) {
	// 		perror("Error in execute()");
	// 		return -1;
	// 	}
	// 	fprintf(stderr, "TESTING2\n");
	}
	return 1;
}

int main(int argc, char **argv)
{
	while (1)
	{
		char workingDir[PATH_MAX];
		if(getcwd(workingDir, sizeof(workingDir)) == NULL) {
			perror("Error in getcwd()\n");
			return 1; // exits with non-zero exit code
		}
		else {
			printf("%s ", workingDir);
		}
		char input[2048] = {0};
		if(input != fgets(input, 2048, stdin)) {
			perror("An error has occurred when getting user input.\n");
			return 1; // exits with non-zero exit code
		}
		input[strcspn(input, "\n")] = 0; // remove the trailing \n character
		if(strcmp(input,"quit") == 0) {
			break;
		}
		cmdLine *myCmdLine = parseCmdLines(input);
		debug = 0;
		if (myCmdLine != NULL) {
			// Check invalid input and output redirections between piped commands
			cmdLine *currCmdLine = myCmdLine;
			int numCommands = 0;
			while (currCmdLine != NULL) {
				if (currCmdLine->next != NULL) {
					if (currCmdLine->outputRedirect != NULL) {
						fprintf(stderr, "Error: cannot redirect output between piped commands\n");
						freeCmdLines(myCmdLine);
						return 1; // exits with non-zero exit code
					}
					if (currCmdLine->next->inputRedirect != NULL) {
						fprintf(stderr, "Error: cannot redirect input between piped commands\n");
						freeCmdLines(myCmdLine);
						return 1; // exits with non-zero exit code
					}
				}
				currCmdLine = currCmdLine->next;
				numCommands++;
			}
			if (numCommands == 1) {
				int result = parseCommandExecuteIfNoPipe(myCmdLine);
				if (result == 0) {
					execute(myCmdLine);
				}
				freeCmdLines(myCmdLine);
			}
			// Execute the command and the next one using the pipeline, if there are exactly 2 commands (otherwise if there are more, it's much more complicated)
			else if (numCommands == 2) {
				int fd[2]; // Stores the two ends of the pipe
				if (pipe(fd) == -1) {
					perror("Error in pipe()");
					freeCmdLines(myCmdLine);
					return 1;
				}
				// fprintf(stderr, "(parent_process>forking…)\n"); // DEBUG
				pid_t pid_first; // process id of the child is 0
				if ((pid_first = fork()) == -1) {
					perror("Error in fork()");
					freeCmdLines(myCmdLine);
					return 1;
				}
				else if (pid_first == 0) { // first child process
					// fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n"); // DEBUG
					close(1); // closes the standard writing end of the child process
					/*int writeEnd = */ dup2(fd[1], 1); // duplicated the file descriptor of the writing end of the pipe
					close(fd[1]); // closed the original file descriptor of the writing end of the pipe
					
					// Execute: first command
					// fprintf(stderr, "(child1>going to execute cmd: …)\n"); // DEBUG
					int result = parseCommandExecuteIfNoPipe(myCmdLine);
					if (result == 0) {
						if (myCmdLine->inputRedirect != NULL) {
							freopen(myCmdLine->inputRedirect, "r", stdin);
						}
						if (myCmdLine->outputRedirect != NULL) {
							freopen(myCmdLine->outputRedirect, "w", stdout);
						}
						char *arguments[myCmdLine->argCount+1]; // Pointer to the arguments array starting after the command itself
						memcpy(arguments,myCmdLine->arguments,sizeof(char*)*myCmdLine->argCount);
						arguments[myCmdLine->argCount] = '\0';
						if (execvp(arguments[0], arguments) < 0) {
							perror("Error, cannot run the given command");
						}
					}
					// char *cmd_args[] = {"ls", 0};
					// execvp(cmd_args[0], cmd_args);
				}
				else { // parent process
					// fprintf(stderr, "parent_process>created process with id: %d\n", pid_first); // DEBUG
					// fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n"); // DEBUG
					close(fd[1]); // closes the writing end of the pipe
					pid_t pid_second; // process id of the child is 0
					if ((pid_second = fork()) == -1) {
						perror("Error in fork()");
						freeCmdLines(myCmdLine);
						return 1;
					}
					else if (pid_second == 0) { // second child process
						// fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe…)\n"); // DEBUG
						close(0); // closes the standard reading end of the child process
						/*int readEnd = */ dup2(fd[0], 0); // duplicated the file descriptor of the reading end of the pipe
						close(fd[0]); // closed the original file descriptor of the reading end of the pipe
						
						// Execute: second command
						// fprintf(stderr, "(child2>going to execute cmd: …)\n"); // DEBUG
						int result = parseCommandExecuteIfNoPipe(myCmdLine->next);
						if (result == 0) {
							if (myCmdLine->next->inputRedirect != NULL) {
								freopen(myCmdLine->next->inputRedirect, "r", stdin);
							}
							if (myCmdLine->next->outputRedirect != NULL) {
								freopen(myCmdLine->next->outputRedirect, "w", stdout);
							}
							char *arguments[myCmdLine->next->argCount+1]; // Pointer to the arguments array starting after the command itself
							memcpy(arguments,myCmdLine->next->arguments,sizeof(char*)*myCmdLine->next->argCount);
							arguments[myCmdLine->next->argCount] = '\0';
							if (execvp(arguments[0], arguments) < 0) {
								perror("Error, cannot run the given command");
							}
						}
						// char *cmd_args[] = {"wc", "-l", 0};
						// execvp(cmd_args[0], cmd_args);
					}
					else { // parent process
						// fprintf(stderr, "parent_process>created process with id: %d\n", pid_second); // DEBUG
						// fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n"); // DEBUG
						close(fd[0]); // closes the reading end of the pipe
						pid_t child1_pid, child2_pid;
						int status1, status2;
						// fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n"); // DEBUG
						child1_pid = waitpid(pid_first, &status1, 0); // waits for the first child process to finish
						if (child1_pid == -1) {
							perror("Error in waitpid()");
							freeCmdLines(myCmdLine);
							return 1;
						}
						child2_pid = waitpid(pid_second, &status2, 0); // waits for the second child process to finish
						if (child2_pid == -1) {
							perror("Error in waitpid()");
							freeCmdLines(myCmdLine);
							return 1;
						}

						// fprintf(stderr, "(parent_process>exiting…)\n"); // DEBUG
						freeCmdLines(myCmdLine);
					}
				}
			}
			else {
				printf("Unsupported number of commands given\n");
			}
			// freeCmdLines(myCmdLine);
		}
	}

	return 0;
}