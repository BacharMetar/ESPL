#include <stdio.h>
#include <stdlib.h>

void displayMenu()
{
    printf("Select operation from the following menu:\n");
    // Add more menu items if needed
}
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

// Struct for function descriptors
struct fun_desc
{
    char *name;
    char (*fun)(char);
};

int main(int argc, char **argv)
{

    // Step 1: Define a pointer 'carray' to a char array of length 5, initialized to an empty string
    char charArray[5];
    charArray[0] = '\0';
    char *carray = charArray;

    // Step 2: Define an array of fun_desc and initialize it
    struct fun_desc menu[] = {
        {"Get String", my_get},
        {"Print String", cprt},
        {"Encrypt", encrypt},
        {"Decrypt", decrypt},
        {"Print Hex", xprt},
        {NULL, NULL} // Sentinel to mark the end of the menu
    };

    // Pre-compute the bounds for user input
    int menuSize = sizeof(menu) / sizeof(menu[0]) - 1; // Excluding the sentinel
    int minChoice = 0;
    int maxChoice = menuSize - 1;

    while (1)
    {
        // Step 3: Display the menu
        printf("Select operation from the following menu:\n");
        for (int i = 0; menu[i].name != NULL; ++i)
        {
            printf("%d)  %s\n", i, menu[i].name);
        }
        // Step 4: Display a prompt and read the user's choice
        printf("Enter your choice: ");
        char input[100];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            // Exit the loop if EOF is encountered
            break;
        }

        // Convert input to integer
        int choice = atoi(input);

        // Check if the choice is within the valid range
        // Step 5: Check if the choice is within bounds
        if (choice >= minChoice && choice <= maxChoice)
        {
            printf("Within bounds\n");

            // Step 6: Evaluate the appropriate function over 'carray'
            carray = map(carray, 5, menu[choice].fun);

            // Step 7: After calling any menu function, let 'carray' point to the new array returned by map( )
            printf("Function result: %s\n", carray);
        }
        else
        {
            printf("Not within bounds. Try again.\n");
        }
    }

    printf("Program terminated.\n");

    return 0;
}