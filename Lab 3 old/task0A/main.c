#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  /*Complete the task here*/
  /* Print "argc = "*/
  char* print_argc = "argc = ";
  system_call(SYS_WRITE, STDOUT, print_argc, strlen(print_argc));

  /* Convert argc to string and print it*/
  char* argc_string = itoa(argc);
  system_call(SYS_WRITE, STDOUT, argc_string, strlen(argc_string));

  /* Print newline */
  system_call(SYS_WRITE, STDOUT, "\n", strlen("\n"));
  
  int i;
  for (i = 0; i < argc; i++) {
      /* Print argument */
      system_call(SYS_WRITE, STDOUT, argv[i], strlen(argv[i]));
      /* Print newline after each argument */
      system_call(SYS_WRITE, STDOUT, "\n", 1);
  }

    return 0;
}