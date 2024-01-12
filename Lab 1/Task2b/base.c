#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
int main(int argc, char **argv)
{
    /* TODO: Test your code */

    // char arr1[] = {'H','E','Y','!'}; 
    // char* arr2 = map(arr1, 4, xprt); 
    // printf("%s\n", arr2); 
    // free(arr2);

    int base_len = 5;
    char arr1[base_len];
    char *arr2 = map(arr1, base_len, my_get);
    char *arr3 = map(arr2, base_len, cprt);
    char *arr4 = map(arr3, base_len, xprt);
    char *arr5 = map(arr4, base_len, encrypt);
    free(arr2);
    free(arr3);
    free(arr4);
    free(arr5);


    

    // // Test my_get
    // char input_char = my_get('a');
    // printf(input_char);
    // // printf("my_get test: %c\n", input_char);

    // // Test cprt
    // char cprt_result = cprt('q');
    // // printf("cprt test: %c\n", cprt_result);

    // // Test encrypt
    // char encrypt_result = encrypt('z');
    // printf(encrypt);
    //     // printf("encrypt test: %c\n", encrypt_result);

    // // Test decrypt
    // char decrypt_result = decrypt('b');
    // printf(decrypt);
    // // printf("decrypt test: %c\n", decrypt_result);

    // // Test xprt
    // char xprt_result = xprt('1');
    // // printf("xprt test: %c\n", xprt_result);

    return 0;
}
