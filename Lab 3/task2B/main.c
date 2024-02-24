#include "util.h"
#include <stdio.h>

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_READ 3
#define SYS_OPEN 5
#define O_RDONLY 0
#define O_WRONLY 1
#define SYS_EXIT 1
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
        system_call(SYS_EXIT, 0x55, 0, 0); 
        return 0;
    }
    int i;
    for (i = 1; i < argc; i++) {
       char* filename;
        int attach_virus = 0;



        if(strlen(argv[i]) >=2 &&argv[i][0] == '-' && argv[i][1] == 'a'){
            attach_virus = 1;
            if (strlen(argv[i]) == 2) {
                system_call(SYS_WRITE, STDOUT, "Filename not provided after -a option\n", 38);
                system_call(SYS_EXIT, 0x55, 0, 0); 
                return 0;
            } else {
                filename = argv[i] + 2;
            }
        } else {
        
            filename = argv[i];
        }

        int file_descriptor;
        if (attach_virus) {
            /* need to attach the virus to the end of the file*/
            file_descriptor = system_call(SYS_OPEN, filename, O_WRONLY | O_APPEND , 0777);
            if (file_descriptor < 0) {
                system_call(SYS_WRITE, STDOUT, "Error opening file\n", 19);
                system_call(SYS_EXIT, 0x55, 0, 0); 
                return 0;
            }
            system_call(SYS_WRITE, STDOUT, filename, strlen(filename));
            system_call(SYS_WRITE, STDOUT, " VIRUS ATTACHED\n", 16);
            infection();
            infector(filename);

        } else {
          
         /* print the file content*/
            file_descriptor = system_call(SYS_OPEN, filename, O_RDONLY, 0777);
            if (file_descriptor < 0) {
                system_call(SYS_WRITE, STDOUT, "Error opening file\n", 19);
                system_call(SYS_EXIT, 0x55, 0, 0); 
                return 0;
            }
            char buffer[BUFFER_SIZE];
            int bytes_read;
            while ((bytes_read = system_call(SYS_READ, file_descriptor, buffer, BUFFER_SIZE)) > 0) {
                system_call(SYS_WRITE, STDOUT, buffer, bytes_read);
            }
        }
        system_call(SYS_CLOSE, file_descriptor, 0, 0);
    }
    return 0;
}
