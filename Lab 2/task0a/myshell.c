#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // for PATH_MAX
#include "LineParser.h"

#define MAX_INPUT_SIZE 2048
// #define PATH_MAX 2048

void execute(cmdLine *pCmdLine)
{
    // Use execv to replace the current process with the specified program
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
    {
        // execv failed
        perror("execvp");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    char path_name[MAX_INPUT_SIZE];
    cmdLine *parsedLine;

    FILE *const input = stdin;
    FILE *const output = stdout;

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
