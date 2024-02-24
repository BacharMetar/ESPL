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

typedef struct process{
	cmdLine* cmd;                         /* the parsed command line*/
	pid_t pid; 		                  /* the process id that is running the command*/
	int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
	struct process *next;	                  /* next process in chain */
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void freeProcessList(process* process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

process** my_process_list;

/*
	A helper function that returns 1 iff the given string is contained in the given strings array.
*/
int contains(char *string, char *const *arr, int size) {
	for(int i = 0; i < size; i++) {
		char *curr_str = arr[i];
		if(strcmp(string,curr_str) == 0) {
			return 1;
		}
	}
	return 0;
}

/*
	A helper function that executes the given cmdLine on a separate process (and blocks if the blocking flag is set).
*/
int execute(cmdLine *pCmdLine) {
	char *arguments[pCmdLine->argCount+1]; // Pointer to the arguments array starting after the command itself
	memcpy(arguments,pCmdLine->arguments,sizeof(char*)*pCmdLine->argCount);
	arguments[pCmdLine->argCount] = '\0';

	pid_t pid; // process id of the child is 0
	if ((pid = fork()) == -1) {
		perror("Error in fork()");
		return -1;
	}
    else if (pid == 0) {
		if (pCmdLine->inputRedirect != NULL) {
			freopen(pCmdLine->inputRedirect, "r", stdin);
		}
		if (pCmdLine->outputRedirect != NULL) {
			freopen(pCmdLine->outputRedirect, "w", stdout);
		}
		if (execvp(pCmdLine->arguments[0],arguments) < 0) {
			perror("Error, cannot run the given command");
			if (stdin != NULL) {
				fclose(stdin);
			}
			if (stdout != NULL) {
				fclose(stdout);
			}
			_exit(1);
			// return -1;
		}
		if (stdin != NULL) {
			fclose(stdin);
		}
		if (stdout != NULL) {
			fclose(stdout);
		}
		_exit(0);
	}
	else {
		if (debug) {
			fprintf(stderr, "PID: %d\n", pid);
			fprintf(stderr, "Executing Command: %s\n", pCmdLine->arguments[0]);
		}
		addProcess(my_process_list, pCmdLine, pid);
		if (pCmdLine->blocking) {
			pid_t child_pid;
			int status;
			child_pid = waitpid(pid, &status, 0); // waits for the child process if the blocking flag is set to true (there was no & at the end of the command)
			if (child_pid == -1) {
				perror("Error in waitpid()");
				return -1;
			}
		}
	}
	return 0;
}

#define FREE(X) if(X) free((void*)X)

/*
	A helper function similar to freeCmdLines from LineParser but only frees the given cmdLine without freeing the whole list using the next pointers.
*/
void freeCmdLine(cmdLine *pCmdLine)
{
  int i;
  if (!pCmdLine)
    return;

  FREE(pCmdLine->inputRedirect);
  FREE(pCmdLine->outputRedirect);
  for (i=0; i<pCmdLine->argCount; ++i)
      FREE(pCmdLine->arguments[i]);

  FREE(pCmdLine);
}

/*
	Receive a process list (process_list), a command (cmd), and the process id (pid) of the process running the command.
	Note that process_list is a pointer to a pointer so that we can insert at the beginning of the list if we wish.
*/
void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
	process * new_proc = malloc(sizeof(process));
	new_proc->cmd = cmd;
	new_proc->pid = pid;
	new_proc->status = RUNNING;
	new_proc->next = process_list[0];
	process_list[0] = new_proc;
}

/*
	Print the processes in the following format for each one:
	<index in process list> <process id> <process status> <the command together with its arguments>
	If a process was "freshly" terminated, delete it after printing it (meaning print the list with the updated status, then delete the dead processes).
*/
void printProcessList(process** process_list) {
	updateProcessList(process_list);
	printf("INDEX\tPID\tCOMMAND\tSTATUS\n");
	if (process_list != NULL) {
		process* proc = process_list[0];
		process* prev_proc = NULL;
		int index = 0;
		while(proc != NULL) {
			char *statusStr = "";
			if (proc->status == RUNNING) {
				statusStr = "Running";
			} else if (proc->status == TERMINATED) {
				statusStr = "Terminated";
			} else if (proc->status == SUSPENDED) {
				statusStr = "Suspended";
			}
			printf("%d\t%d\t%s\t%s\n", index, proc->pid, proc->cmd->arguments[0], statusStr);
			
			// Delete the process if it was freshly terminated
			if (proc->status == TERMINATED) {
				if(prev_proc == NULL) {
					process_list[0] = proc->next;
					freeCmdLine(proc->cmd); // Note: each piped command runs on a different process so they are freed correctly.
					free(proc);
				} else {
					prev_proc->next = proc->next;
					freeCmdLine(proc->cmd); // Note: each piped command runs on a different process so they are freed correctly.
					free(proc);
				}
			} else {
				prev_proc = proc;
			}

			proc = proc->next;
			index++;
		}
	}
}

/*
	Free all memory allocated for the process list.
*/
void freeProcessList(process* process_list) {
	process* proc = process_list;
	while (proc != NULL) {
		process* next_proc = proc->next;
		freeCmdLine(proc->cmd);
		free(proc);
		proc = next_proc;
	}
}

/*
	Go over the process list, and for each process check if it is done.
	You can use waitpid with the option WNOHANG.
	WNOHANG does not block the calling process, the process returns from the call to waitpid immediately.
	If no process with the given process id exists, then waitpid returns -1.
	In order to learn if a process was stopped (SIGTSTP), resumed (SIGCONT) or terminated (SIGINT),
	It's highly essential you read and understand how to use waitpid(2) before implementing this function
*/
void updateProcessList(process **process_list) {
	if (process_list != NULL) {
		process *proc = process_list[0];
		while (proc != NULL) {
			pid_t child_pid;
			int status;
			child_pid = waitpid(proc->pid, &status, WNOHANG | WUNTRACED | WCONTINUED); // does not wait for the child process
			if (child_pid == -1) {
				proc->status = TERMINATED;
			}
			else if (WIFEXITED(status)) { // Why is it returning true for ./looper that is running for some reason?
				// proc->status = TERMINATED;
			} else if (WIFSIGNALED(status)) { // Returns true for any signal given to the process, not only a termination signal
				if (WTERMSIG(status) == SIGINT) { // WIFSIGNALED returns true almost every time... so without this check it terminates on the next cycle
					proc->status = TERMINATED;
				}
			} else if (WIFSTOPPED(status)) {
				proc->status = SUSPENDED;
			} else if (WIFCONTINUED(status)) {
				proc->status = RUNNING;
			}
			proc = proc->next;
		}
	}
}

/*
	Find the process with the given id in the process_list and change its status to the received status
*/
void updateProcessStatus(process* process_list, int pid, int status) {
	process *proc = process_list;
	while (proc != NULL) {
		if (proc->pid == pid) {
			proc->status = status;
			break;
		}
	}
}

/*
	A helper function that executes the given command if it is a main thread command ('cd', 'wakeup', 'nuke', 'suspend', etc.)
	If the given command does not require a new process, it executed it on the same running process and returns 1, otherwise returns 0.
*/
int executeParsedCommandSameProcess(cmdLine *commandLine) {
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
			else if (kill(process_id, SIGINT) < 0) {
				perror("Error in kill()");
			}
		}	
	}
	// suspend <process id> - suspends a running process. Send SIGTSTP to the respective process. This is similar to typing CTRL-Z in the shell when running the process.
	else if (commandLine->argCount >= 1 && strcmp(commandLine->arguments[0],"suspend") == 0) { // 'suspend' command
		if (commandLine->argCount != 2) {
			perror("Too many arguments to 'suspend' command");
		}
		else {
			char *endptr;
			long process_id = strtol(commandLine->arguments[1], &endptr, 10);
			if (*endptr != '\0') { // Check for conversion error
				perror("Error: Invalid characters given as process id");
			}
			else if (kill(process_id, SIGTSTP) < 0) {
				perror("Error in kill()");
			}
		}	
	}
	else if (commandLine->argCount >= 1 && strcmp(commandLine->arguments[0],"procs") == 0) { // 'procs' command
		if (commandLine->argCount != 1) {
			perror("Too many arguments to 'procs' command");
		}
		else {
			printProcessList(my_process_list);
		}	
	}
	else {
		return 0; // Signifies the command needs to create a different process and wasn't executed yet.
	}
	return 1;
}

int main(int argc, char **argv)
{
	my_process_list = malloc(sizeof(process*));
	my_process_list[0] = NULL;
	while (1)
	{
		char workingDir[PATH_MAX];
		if(getcwd(workingDir, sizeof(workingDir)) == NULL) {
			perror("Error in getcwd()");
			return 1; // exits with non-zero exit code
		}
		else {
			printf("%s ", workingDir);
		}
		char input[2048] = {0};
		if(input != fgets(input, 2048, stdin)) {
			perror("An error has occurred when getting user input");
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
			int numCommands = 0; // counts the number of commands in the given command line
			while (currCmdLine != NULL) {
				if (currCmdLine->next != NULL) {
					// Checks the validity of the input/output redirection within piped commands
					if (currCmdLine->outputRedirect != NULL) {
						fprintf(stderr, "Error: cannot redirect output between piped commands\n");
						freeProcessList(my_process_list[0]);
						return 1; // exits with non-zero exit code
					}
					if (currCmdLine->next->inputRedirect != NULL) {
						fprintf(stderr, "Error: cannot redirect input between piped commands\n");
						freeProcessList(my_process_list[0]);
						return 1; // exits with non-zero exit code
					}
				}
				currCmdLine = currCmdLine->next;
				numCommands++;
			}
			if (numCommands == 1) {
				int result = executeParsedCommandSameProcess(myCmdLine);
				if (result == 0) { // The command requires a different process to run, so we run it within a child process using 'execute'
					execute(myCmdLine);
				} else {
					freeCmdLines(myCmdLine);
				}
			}
			// Execute the 2 commands using the pipeline, if there are exactly 2 commands (otherwise if there are more, it's much more complicated)
			else if (numCommands == 2) {
				int fd[2]; // Stores the two ends of the pipe
				if (pipe(fd) == -1) {
					perror("Error in pipe()");
					freeProcessList(my_process_list[0]);
					return 1;
				}
				// fprintf(stderr, "(parent_process>forking…)\n"); // DEBUG
				pid_t pid_first; // process id of the child is 0
				if ((pid_first = fork()) == -1) {
					perror("Error in fork()");
					freeProcessList(my_process_list[0]);
					return 1;
				}
				else if (pid_first == 0) { // first child process
					// fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n"); // DEBUG
					close(1); // closes the standard writing end of the child process
					/*int writeEnd = */ dup2(fd[1],1); // duplicated the file descriptor of the writing end of the pipe
					close(fd[1]); // closed the original file descriptor of the writing end of the pipe
					
					// Execute: first command
					// fprintf(stderr, "(child1>going to execute cmd: …)\n"); // DEBUG
					int result = executeParsedCommandSameProcess(myCmdLine);
					if (result == 0) { // The command requires a different process to run, so we run it here in the 1st child process
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
					return 0; // exit from the child process
				}
				else { // parent process
					// fprintf(stderr, "parent_process>created process with id: %d\n", pid_first); // DEBUG
					// fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n"); // DEBUG
					close(fd[1]); // closes the writing end of the pipe
					pid_t pid_second; // process id of the child is 0
					if ((pid_second = fork()) == -1) {
						perror("Error in fork()");
						freeProcessList(my_process_list[0]);
						return 1;
					}
					else if (pid_second == 0) { // second child process
						// fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe…)\n"); // DEBUG
						close(0); // closes the standard reading end of the child process
						/*int readEnd = */ dup2(fd[0],0); // duplicated the file descriptor of the reading end of the pipe
						close(fd[0]); // closed the original file descriptor of the reading end of the pipe
						
						// Execute: second command
						// fprintf(stderr, "(child2>going to execute cmd: …)\n"); // DEBUG
						int result = executeParsedCommandSameProcess(myCmdLine->next);
						if (result == 0) { // The command requires a different process to run, so we run it here in the 2nd child process
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
						return 0; // exit from the child process
					}
					else { // parent process
						// fprintf(stderr, "parent_process>created process with id: %d\n", pid_second); // DEBUG
						// fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n"); // DEBUG
						close(fd[0]); // closes the reading end of the pipe
						pid_t child1_pid, child2_pid;
						int status1, status2;
						// fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n"); // DEBUG
						addProcess(my_process_list, myCmdLine, pid_first);
						addProcess(my_process_list, myCmdLine->next, pid_second);
						child1_pid = waitpid(pid_first, &status1, 0); // waits for the first child process to finish
						if (child1_pid == -1) {
							perror("Error in waitpid()");
							freeProcessList(my_process_list[0]);
							return 1;
						}
						child2_pid = waitpid(pid_second, &status2, 0); // waits for the second child process to finish
						if (child2_pid == -1) {
							perror("Error in waitpid()");
							freeProcessList(my_process_list[0]);
							return 1;
						}
						// fprintf(stderr, "(parent_process>exiting…)\n"); // DEBUG
					}
				}
			}
			else {
				printf("Unsupported number of commands given\n");
			}
		}
	}
	freeProcessList(my_process_list[0]);
	return 0;
}