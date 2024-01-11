#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void increas(int length, int *i)
{
   
    (*i)++;
    if (*i >= length)
    {
        *i = 0;
    }
}

int main(int argc, char *argv[])
{
    FILE *output = stdout;

    int encodingMode = 0;     // 0  for no encoding , 1  for addition , -1 for subtraction  ;
    char *encodingKey = NULL; // an array of chars storing the encodingkey (cell by cell)

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

    int input;
    int i = 0;
    int length = strlen(encodingKey);
    while ((input = fgetc(stdin)) != EOF)
    {
    //    fprintf(output,"Original char: %c\n", input);

        // Apply encoding only to uppercase letters and digits
        if ((input >= 'A' && input <= 'Z'))
        {
            if (encodingMode == 1)
            {
                int check = input + (encodingKey[i] - '0');
                if (check <= 'Z')
                {
                    input = check;
                }
                else
                {
                    input = 'A' + (check - 'Z' - 1);
                }
            }
            else if (encodingMode == -1)
            {
                int check = input - (encodingKey[i] - '0');
                if (check >= 'A')
                {
                    input = check;
                }
                else
                {
                    input = 'Z' - ('A' - check - 1);
                }
            }
        }
        else if (input >= '0' && input <= '9')
        {
            if (encodingMode == 1)
            {
                int check = input + (encodingKey[i] - '0');
                if (check <= '9')
                {
                    input = check;
                }
                else
                {
                    input = '0' + (check - '9' - 1);
                }
            }
            else if (encodingMode == -1)
            {
                int check = input - (encodingKey[i] - '0');
                if (check >= '0')
                {
                    input = check;
                }
                else
                {
                    input = '9' - ('0' - check - 1);
                }
            }
        }
    // fprintf(output, "i: %d\n", i);
    increas(length, &i);

      fprintf(output, "%c", input) ;


        if (output != stdout)
        {
            fclose(output);
        }
    }

    return 0;
}
