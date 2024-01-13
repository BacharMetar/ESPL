#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base.h"


// functions declerations:
char *map(char *array, int array_length, char (*f)(char));
char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);

char *map(char *array, int array_length, char (*f)(char))
{
    char *mapped_array = (char *)(malloc(array_length * sizeof(char)));
    /* TODO: Complete during task 2.a */
    for (int i = 0; i < array_length; ++i)
    {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

char nextChar(char c)
{
    return c + 1;
}

char my_get(char c)
{
    // Ignore the input character 'c'- this line exist to avoid compiling warnings.
    (void)c;
    // Read and return a character from stdin using fgetc
    return (char)fgetc(stdin);
}

char cprt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        printf("%c\n", c);
    }
    else
    {
        printf(".\n");
    }
    return c;
}
char encrypt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        // Encrypt the character by adding 1 to its value
        return c + 1;
    }
    else
    {
        // Return unchanged if c is not in the specified range
        return c;
    }
}
// https://log2base2.com/c-questions/io/how-to-print-hexadecimal-in-c.html -idea to print hexdecimal in C

char xprt(char c)
{
    // Print the single hexadecimal digit of c followed by a new line
    printf("%x\n", c);

    // Return c unchanged
    return c;
}

char decrypt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        // Decrypt the character by reducing 1 from its value
        return c - 1;
    }
    else
    {
        // Return unchanged if c is not in the specified range
        return c;
    }
}
