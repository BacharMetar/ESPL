#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int debugMode = 1;
    for (int i = 1; i < argc; i++)
    {
        if (debugMode)
        {
            fprintf(stderr, "%s\n", argv[i]);
        }
        if (strcmp(argv[i], "-D") == 0)
        {
            debugMode = 0;
        }
        else  if (strcmp(argv[i], "+D") == 0)
        {
            debugMode = 1;
        }
    }

    return 0;
}