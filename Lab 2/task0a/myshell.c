// #include <linux/limits.h>
#include <stdio.h>
#include <unistd.h> //cwd
#include "LineParser.h"
#include <string.h>
#include <stdlib.h>

#include <limits.h> // for PATH_MAX

#define true 1
#define BUFFER_SIZE 2048

void execute(cmdLine* parsed_cmd_line)
{
    char* const current_command = parsed_cmd_line->arguments[0];
    
    if(strcmp(current_command, "quit") == 0)
    {
        exit(EXIT_SUCCESS);
    }

    if(execv(current_command, parsed_cmd_line->arguments) <  0)
    {
        perror("Failure to execute command");
        exit(EXIT_FAILURE);
    }    
}

int main(int argc, char const *argv[])
{
    FILE* const input = stdin;
    FILE* const output = stdout;

    char path_name_buffer[PATH_MAX];
    char reading_buffer[BUFFER_SIZE];

    while (true)
    {
        //1. print cwd
        getcwd(path_name_buffer, PATH_MAX);
        fprintf(output, "%s\n", path_name_buffer);

        //2. read line from user
        fgets(reading_buffer, BUFFER_SIZE, input);

        //3. parse the input
        cmdLine* parsed_cmd_line = parseCmdLines(reading_buffer);

        //4. execute
        execute(parsed_cmd_line);

        //6. relase cmd line when finished
        freeCmdLines(parsed_cmd_line);
    }    

    return 0;
}
