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

    int encodingMode = 0;     // 0 for no encoding, 1 for addition, -1 for subtraction
    char *encodingKey = NULL; // array of chars storing the encoding key (cell by cell)

    char *inputFileName = NULL;  // variable to store the input file name
    char *outputFileName = NULL; // variable to store the output file name

    // ================================ Task 1 ===================================
    // Scan through the arguments and identify options and values
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-D", 2) == 0)
        {
            debugMode = 0;
        }
        else if (strncmp(argv[i], "+D", 2) == 0)
        {
            debugMode = 1;
        }
        else if (strncmp(argv[i], "+E", 2) == 0)
        {
            encodingMode = 1;
            encodingKey = argv[i] + 2;
        }
        else if (strncmp(argv[i], "-E", 2) == 0)
        {
            encodingMode = -1;
            encodingKey = argv[i] + 2;
        }
        else if (strncmp(argv[i], "-I", 2) == 0)
        {
            inputFileName = (argv[i][2] != '\0') ? argv[i] + 2 : argv[++i];
        }
        else if (strncmp(argv[i], "-O", 2) == 0)
        {
            outputFileName = (argv[i][2] != '\0') ? argv[i] + 2 : argv[++i];
        }
    }

    // ================================ Task 2 ===================================
    // Open input file
    if (inputFileName)
    {
        input = fopen(inputFileName, "r");
        if (!input)
        {
            fprintf(stderr, "Error opening input file: %s\n", inputFileName);
            exit(EXIT_FAILURE);
        }
    }

    // Open output file
    if (outputFileName)
    {
        output = fopen(outputFileName, "w");
        if (!output)
        {
            fprintf(stderr, "Error opening output file: %s\n", outputFileName);
            exit(EXIT_FAILURE);
        }
    }

    // ================================ Task 3 ===================================
    // Process the input and output files
    int inputChar;
    int i = 0;
    int length = encodingKey ? strlen(encodingKey) : 0;

    while ((inputChar = fgetc(input)) != EOF)
    {
        if ((inputChar >= 'A' && inputChar <= 'Z') || (inputChar >= '0' && inputChar <= '9'))
        {
            if (encodingMode == 1)
            {
                int check = inputChar + (encodingKey[i] - '0');
                inputChar = (check <= 'Z') ? check : 'A' + (check - 'Z' - 1);
            }
            else if (encodingMode == -1)
            {
                int check = inputChar - (encodingKey[i] - '0');
                inputChar = (check >= 'A') ? check : 'Z' - ('A' - check - 1);
            }
        }

        // Increment index
        increase(length, &i);

        // Print char
        fputc(inputChar, output);
    }

    fputc('\n', output);

    // Close files if they were opened
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
