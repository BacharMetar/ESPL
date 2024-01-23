#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // for PATH_MAX
#include "LineParser.h"
#include <sys/wait.h>

#include <fcntl.h> // for open
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_INPUT_SIZE 2048
// #define PATH_MAX 2048
int debugMode;

void execute(cmdLine *pCmdLine)
{

    // Handle "cd" command
    if (strcmp(pCmdLine->arguments[0], "cd") == 0)
    {
        // Get the directory path from the next argument
        char *directory_path = pCmdLine->arguments[1];
        // Use chdir to change the current working directory
        if (chdir(directory_path) == -1)
        {
            // Print an error message to stderr if chdir fails
            perror("cd error");
        }
        if (debugMode == 1)
        {
            fprintf(stderr, "PID: %d\nExecuting command %s\n", getpid(), pCmdLine->arguments[0]);
        }
    }

    else
    {
        // Use fork to create a child process
        int pid = fork();

        // Check for fork failure
        if (pid == -1)
        {
            perror("fork failure");
            exit(EXIT_FAILURE);
        }

        // Parent process
        if (pid > 0)
        {
            fprintf(stderr, "PID: %d\n", pid);
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);

            // Check if the process should run in the background
            if (pCmdLine->blocking == 1)
            {
                // Wait for the child process to complete
                waitpid(pid, NULL, 0);
            }
        }
        // Child process
        else if (pid == 0)
        {
            // check if exist any INPUT source
            if (pCmdLine->inputRedirect != NULL)
            {
                // base on information from: https://blog.dennisokeeffe.com/blog/2020-07-28-redirections-in-c
                int fd1 = open(pCmdLine->inputRedirect, O_RDONLY);
                if (fd1 == -1)
                {
                    perror("Input redirection failed");
                    _exit(EXIT_FAILURE);
                }
                if (dup2(fd1, STDIN_FILENO) < 0)
                {
                    perror("Unable to duplicate file descriptor.");
                    exit(EXIT_FAILURE);
                }
                close(fd1);
            }

            // check if exist any OUTPUT source
            if (pCmdLine->outputRedirect != NULL)
            {
                //based on information from : https://stackoverflow.com/questions/76222562/issues-writing-to-a-file-when-redirecting-output-from-another-file
                int fd2 = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd2 == -1)
                {
                    perror("Output redirection failed");
                    _exit(EXIT_FAILURE);
                }
                if (dup2(fd2, STDOUT_FILENO) < 0)
                {
                    perror("Unable to duplicate file descriptor.");
                    exit(EXIT_FAILURE);
                }
                close(fd2);
            }
            // Use execvp to replace the current process with the specified program
            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
            {
                // execvp failed
                perror("failure to execvp CHILD");
                _exit(EXIT_FAILURE);
            }
        }

        if (debugMode == 1)
        {
            fprintf(stderr, "PID: %d\nExecuting command %s\n", pid, pCmdLine->arguments[0]);
        }
    }
}

int main(int argc, char const *argv[])
{
    char path_name[MAX_INPUT_SIZE];
    cmdLine *parsedLine;

    FILE *const input = stdin;
    FILE *const output = stdout;

    debugMode = 0;
    for (int i = 1; i < argc; i++)
    {
        if (debugMode)
        {
            fprintf(stderr, "%s\n", argv[i]);
        }
        if (strcmp(argv[i], "-d") == 0)
        {
            debugMode = 1;
        }
    }

    while (1)
    {
        // Display prompt
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            fprintf(output, "%s$ ", cwd);
        }
        else
        {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }
        // Read path_name
        if (fgets(path_name, MAX_INPUT_SIZE, input) == NULL)
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }

        // Parse path_name
        parsedLine = parseCmdLines(path_name);
        // Check for "quit" command
        if (strcmp(parsedLine->arguments[0], "quit") == 0)
        {
            fprintf(output, "%s$ ", "Exiting the shell.\n");
            // Release any remaining resources
            freeCmdLines(parsedLine);
            exit(EXIT_SUCCESS);
        }

        // Execute the parsed command
        else
        {
            execute(parsedLine);
        }
        // Release cmdLine resources
        freeCmdLines(parsedLine);
    }

    printf("Exiting the shell.\n");
    return 0;
}
