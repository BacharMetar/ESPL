#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> // Include string.h for strlen function

int main()
{
    int pipe_fd[2];
    char buffer[2048];

    // Create a pipe
    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Create a child process
    int pid = fork();

    if (pid == -1)
    {
        perror("Fork failed to create child procces");
        exit(EXIT_FAILURE);
    }

    // if (pid == 0)
    // { // Child process
    //     // Write a message to the parent
    //     write(pipe_fd[1], "hello", 5);

    //     close(pipe_fd[1]); // Close the write end
    // }
    if (pid == 0)
  { 
    // Child process
    // Close the read end of the pipe as it won't be used in the child process
    // Get input from the user
    char user_input[2048];
    printf("Enter a message: ");
    fgets(user_input, sizeof(user_input), stdin);

    // Write the user input to the pipe
    write(pipe_fd[1], user_input, strlen(user_input));

    // Close the write end of the pipe
    close(pipe_fd[1]); 
  }


    if (pid == 0)
    { // Child process
        // Write a message to the parent

        write(pipe_fd[1], "hello", 5);

        close(pipe_fd[1]); // Close the write end
    }
    else
    { // Parent process
        // close(pipe_fd[1]);  // Close the write end

        // Read the message from the child
        read(pipe_fd[0], buffer, sizeof(buffer));
        buffer[5] = '\0'; // Null-terminate the buffer

        // Print the message
        printf("Parent received: %s\n", buffer);

        close(pipe_fd[0]); // Close the read end
    }

    return 0;
}
