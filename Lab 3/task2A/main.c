#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_READ 3
#define SYS_OPEN 5
#define O_RDONLY 0
#define SYS_CLOSE 6
#define BUFFER_SIZE 1024

extern int system_call();

int main(int argc, char* argv[], char* envp[]) {
    if (argc != 2) {
        /* Print error message if the user didn't provide the file name */
        system_call(SYS_WRITE, STDOUT, "Usage: ./program <filename>\n", 29);
        return 0;
    }

    /* // Open the file for reading */
    int file_descriptor = system_call(SYS_OPEN, argv[1], O_RDONLY, 0777);
    if (file_descriptor < 0) {
       /* // Print error message if the file cannot be opened*/
        system_call(SYS_WRITE, STDOUT, "Error opening file\n", 19);
        return 0;
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;

    /* // Read the content of the file and write it to the standard output */
    while ((bytes_read = system_call(SYS_READ, file_descriptor, buffer, BUFFER_SIZE)) > 0) {
        system_call(SYS_WRITE, STDOUT, buffer, bytes_read);
    }

    /* // Close the file */
    system_call(SYS_CLOSE, file_descriptor, 0, 0);

    return 0;
}
