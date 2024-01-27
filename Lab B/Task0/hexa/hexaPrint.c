#include <stdio.h>
#include <stdlib.h>

// Helper function to print bytes in hexadecimal format
void PrintHex(unsigned char *buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        fprintf(stdout, "%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
   
    // Open the file for reading in binary mode
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // Read the file byte by byte and print the hexadecimal value
    unsigned char byte;
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1)
    {
        printf("%02X ", byte);
    }
    printf("\n");

    // Close the file
    fclose(file);

    return 0;
}
