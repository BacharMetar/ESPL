#include "util.h"
#include <stdio.h> 

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_READ 3
#define SYS_OPEN 5
#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 64
#define O_APPEND 1024
#define SYS_CLOSE 6
#define BUFFER_SIZE 1024

extern int system_call();
extern void infection(); 
extern void infector(char *filename); 

int main(int argc, char* argv[], char* envp[]) {
    if (argc < 2) {
        system_call(SYS_WRITE, STDOUT, "Usage: ./program <filename> [-a]\n", 32);
        return 0;
    }

    /* Check if the "-a" option is provided */
    int attach_virus = 0;
    if (argc > 1 && (strncmp("-a", argv[1], 2) == 0)) {
        attach_virus = 1;
    }

    /* Open the file for reading or appending */
    int file_descriptor;
    if (attach_virus) {
        file_descriptor = system_call(SYS_OPEN, argv[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
    } else {
        file_descriptor = system_call(SYS_OPEN, argv[1], O_RDONLY, 0777);
    }

    if (file_descriptor < 0) {
        system_call(SYS_WRITE, STDOUT, "Error opening file\n", 19);
        return 0;
    }

    if (attach_virus) {
        /* Append the virus code to the end of the file (implementation needed) */
        /* Print "VIRUS ATTACHED" next to the file name */
        system_call(SYS_WRITE, STDOUT, argv[2], strlen(argv[2]));
        system_call(SYS_WRITE, STDOUT, " VIRUS ATTACHED\n", 16);
        infection();
        infector(argv[2]); 

       
    } else {
        char buffer[BUFFER_SIZE];
        int bytes_read;

        /* Read the content of the file and write it to the standard output */
        while ((bytes_read = system_call(SYS_READ, file_descriptor, buffer, BUFFER_SIZE)) > 0) {
            system_call(SYS_WRITE, STDOUT, buffer, bytes_read);
        }
    }

    /* Close the file */
    system_call(SYS_CLOSE, file_descriptor, 0, 0);

    return 0;
}
