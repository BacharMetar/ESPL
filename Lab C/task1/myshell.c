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

int main(int argc, char **argv)
{

	// printf("Starting the program\n");
	// signal(SIGINT, handler);
	// signal(SIGTSTP, handler);
	// signal(SIGCONT, handler);

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
			if(contains("-d",myCmdLine->arguments,myCmdLine->argCount)) {
				debug = 1;
			}
			if (myCmdLine->argCount >= 1 && strcmp(myCmdLine->arguments[0],"cd") == 0) { // 'cd' command
				if (myCmdLine->argCount != 2) {
					perror("Too many arguments to 'cd' command");
				}
				else {
					if (chdir(myCmdLine->arguments[1]) < 0) {
						perror("Error in chdir()");
					}
				}	
			}
			else if (myCmdLine->argCount >= 1 && strcmp(myCmdLine->arguments[0],"wakeup") == 0) { // 'wakeup' command
				if (myCmdLine->argCount != 2) {
					perror("Too many arguments to 'wakeup' command");
				}
				else {
					char *endptr;
					long process_id = strtol(myCmdLine->arguments[1], &endptr, 10);
					if (*endptr != '\0') { // Check for conversion error
						perror("Error: Invalid characters given as process id");
					}
					else if (kill(process_id, SIGCONT) < 0) {
						perror("Error in kill()");
					}
				}	
			}
			else if (myCmdLine->argCount >= 1 && strcmp(myCmdLine->arguments[0],"nuke") == 0) { // 'nuke' command
				if (myCmdLine->argCount != 2) {
					perror("Too many arguments to 'nuke' command");
				}
				else {
					char *endptr;
					long process_id = strtol(myCmdLine->arguments[1], &endptr, 10);
					if (*endptr != '\0') { // Check for conversion error
						perror("Error: Invalid characters given as process id");
					}
					else if (kill(process_id, SIGKILL) < 0) {
						perror("Error in kill()");
					}
				}	
			}
			else if (execute(myCmdLine) < 0) {
				freeCmdLines(myCmdLine);
				return 1; // exits with non-zero exit code
			}
			freeCmdLines(myCmdLine);
		}
	}

	return 0;
}