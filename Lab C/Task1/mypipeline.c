#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pipe_fd[2];

    // 1. Create a pipe
    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 2. Fork a first child process (child1)
    fprintf(stderr, "(parent_process>forking...)\n");
    pid_t child1_pid = fork();
    if (child1_pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }




    if (child1_pid == 0)
    {
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");

        // 3.1 Close the standard output
        close(STDOUT_FILENO);

        // Duplicate the write-end of the pipe using dup
        dup(pipe_fd[1]);

        // Close the file descriptor that was duplicated
        close(pipe_fd[1]);

        // Close the read end of the pipe
        close(pipe_fd[0]);

        fprintf(stderr, "(child1>going to execute cmd:...)\n");
        // Execute "ls -l"
        execlp("ls", "ls", "-l", NULL);

        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "(parent_process>created process with id: %d)\n", child1_pid);
        // 4. In the parent process Close the write end of the pipe
        fprintf(stderr, "(parent_process>closing the write end of the pipe...) \n");
        close(pipe_fd[1]); // immposible to write anf the program

        // Fork a second child process (child2)
        pid_t child2_pid = fork();
        if (child2_pid == -1)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (child2_pid == 0)
        {
            
             fprintf(stderr, "(child2>redirecting stdout to the write end of the pipe...)\n");
            //6. In child2 process
            //6.1 Close the standard input
            close(STDIN_FILENO);

            //6.2 Duplicate the read-end of the pipe using dup
            dup(pipe_fd[0]);

            // 6.3Close the file descriptor that was duplicated
            close(pipe_fd[0]);

            // Close the write end of the pipe
            close(pipe_fd[1]);
            fprintf(stderr, "(child2>going to execute cmd:...)\n");
            //6.4 Execute "tail -n 2"
            execlp("tail", "tail", "-n", "2", NULL);

            perror("Exec failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, "(parent_process>created process with id: %d)\n", child2_pid);
            // In the parent process
            //7. Close the read end of the pipe
            fprintf(stderr, "(parent_process>closing the read end of the pipe...) \n");
            close(pipe_fd[0]);

            // Close the write end of the pipe
            // close(pipe_fd[1]);

            //8. Wait for the child processes to terminate
            fprintf(stderr, "(parent_process>waiting for child processes to terminate...) \n");
            waitpid(child1_pid, NULL, 0);
            waitpid(child2_pid, NULL, 0);

            fprintf(stderr, "(parent_process>exiting...)\n");
        }
    }

    return 0;
}
