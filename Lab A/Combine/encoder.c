#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int debugMode = 1;
    FILE *output = stdout;
    FILE *input = stdin;

    int encodingMode = 0;     // 0  for no encoding , 1  for addition , -1 for subtraction  ;
    char *encodingKey = NULL; // an array of chars storing the encodingkey (cell by cell)
    int inputChar;
    int i = 0;
    int length = 0;

    //================Task1===================================================================
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
        else if (strcmp(argv[i], "+D") == 0)
        {
            debugMode = 1;
        }
    }

    //================Task2===================================================================
    for (int i = 1; i < argc; ++i)
    {
        // checking what kind of operation (if at all needs)
        if (argv[i][0] == '+' && argv[i][1] == 'E')
        {
            encodingMode = 1;
            encodingKey = argv[i] + 2;
            while (argv[i][2 + length] != '\0')
            {
                length++;
            }
            // fprintf(stdout, "%d\n", length);
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'E')
        {
            encodingMode = -1;
            encodingKey = argv[i] + 2;
            while (argv[i][2 + length] != '\0')
            {
                length++;
            }
            // fprintf(stdout, "%d\n", length);
        }
    }

    // ================================ Task 3 ===================================
    // Check for input file option
    // Open input file
    // Open input file
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-I", 2) == 0)
        {
            input = fopen(argv[i] + 2, "r");
            if (!input)
            {
                fprintf(stderr, "Error opening input file: %s\n", argv[i + 1]);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    // Open output file
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-O", 2) == 0)
        {
            output = fopen(argv[i] + 2, "w");
            if (!output)
            {
                fprintf(stderr, "Error opening output file: %s\n", argv[i + 1]);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    //====================end============================================================

    while ((inputChar = fgetc(input)) != EOF)
    {
        // Apply encoding only to uppercase letters and digits
        if ((inputChar >= 'A' && inputChar <= 'Z'))
        {
            if (encodingMode == 1)
            {
                int check = inputChar + (encodingKey[i] - '0');
                if (check <= 'Z')
                {
                    inputChar = check;
                }
                else
                {
                    inputChar = 'A' + (check - 'Z' - 1);
                }
            }
            else if (encodingMode == -1)
            {
                int check = inputChar - (encodingKey[i] - '0');
                if (check >= 'A')
                {
                    inputChar = check;
                }
                else
                {
                    inputChar = 'Z' - ('A' - check - 1);
                }
            }
        }
        else if (inputChar >= '0' && inputChar <= '9')
        {
            if (encodingMode == 1)
            {
                int check = inputChar + (encodingKey[i] - '0');
                if (check <= '9')
                {
                    inputChar = check;
                }
                else
                {
                    inputChar = '0' + (check - '9' - 1);
                }
            }
            else if (encodingMode == -1)
            {
                int check = inputChar - (encodingKey[i] - '0');
                if (check >= '0')
                {
                    inputChar = check;
                }
                else
                {
                    inputChar = '9' - ('0' - check - 1);
                }
            }
        }
        // Increment index
        i = (i + 1) % length;
        // print char
        fputc(inputChar, output);
    }

    fputc('\n', output);
    if (input != stdin && input != NULL)
    {
        fclose(input);
    }
    if (output != stdout && output != NULL)
    {
        fclose(output);
    }

    return 0;
}