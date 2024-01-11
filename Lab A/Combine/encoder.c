#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void increase(int length, int *i)
{
    (*i)++;
    if (*i >= length)
    {
        *i = 0;
    }
}
int main(int argc, char *argv[])
{
    int debugMode = 1;
    FILE *output = stdout;
    FILE *input = stdin;

    int encodingMode = 0;     // 0  for no encoding , 1  for addition , -1 for subtraction  ;
    char *encodingKey = NULL; // an array of chars storing the encodingkey (cell by cell)

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
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'E')
        {
            encodingMode = -1;
            encodingKey = argv[i] + 2;
        }
    }

    int inputChar;
    int i = 0;
    int length = 0;
    while (argv[1][2 + length] != '\0')
    {
        // fprintf(output, "%c", argv[1][2 + length]);
        length++;
    }

    // ================================ Task 3 ===================================
    // Check for input file option
    // Open input file
    for (int i = 1; i < argc - 1; ++i)
    {
        if (strcmp(argv[i], "-I") == 0)
        {
            // Open input file
            input = fopen(argv[i + 1], "r");
            if (!input)
            {
                fprintf(stderr, "Error opening input file: %s\n", argv[i + 1]);
                exit(EXIT_FAILURE);
            }
            break; // Only one input file option is allowed
        }
    }

    // Open output file
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-O") == 0)
        {
            // Open output file
            output = fopen(argv[i + 1], "w");
            if (!output)
            {
                fprintf(stderr, "Error opening output file: %s\n", argv[i + 1]);
                exit(EXIT_FAILURE);
            }
            break; // Only one output file option is allowed
        }
    }

    //====================end============================================================

    while ((inputChar = fgetc(input)) != EOF)
    {
        // fprintf(output,"MESSAGE %c\n", "got inside while loop");
        // fprintf(output, "Inside txt: %c\n", inputChar);

        // Apply encoding only to uppercase letters and digits
        if ((inputChar >= 'A' && inputChar <= 'Z'))
        {

            // fprintf(output, "%c\n", encodingKey[i]);

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
        // fprintf(output, "i: %d\n", i);
        increase(length, &i);

        //   fprintf(output, "%c", inputChar) ;
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