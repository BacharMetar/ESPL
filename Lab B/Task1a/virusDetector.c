#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

virus *readVirus(FILE *input)
{
    // Allocate memory for a new virus
    virus *new_virus = malloc(sizeof(virus));
    if (new_virus == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Read virus details directly into the struct
    if (fread(new_virus, 1, 18, input) > 0)
    {
        // Allocate memory for the signature
        new_virus->sig = malloc(new_virus->SigSize);
        if (new_virus->sig == NULL)
        {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // Read the virus signature from the file
        fread(new_virus->sig, 1, new_virus->SigSize, input);
    }

    return new_virus;
}

void PrintHex(unsigned char *buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        fprintf(stdout, "%02X ", buffer[i]);
    }
    printf("\n");
}
void printVirus(virus *virus, FILE *output)
{
    // Print virus name
    fprintf(output, "Virus Name: %s\n", virus->virusName);

    // Print virus signature size in decimal
    fprintf(output, "Signature Size: %hu\n", virus->SigSize);

    // Print virus signature in hexadecimal
    fprintf(output, "Signature (Hexadecimal): ");
    for (int i = 0; i < virus->SigSize; i++)
    {
        fprintf(output, "%02X ", virus->sig[i]);
    }
    fprintf(output, "\n\n");
}


int main(int argc, const char **argv)
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // // Find the length of the file
    // fseek(input, 0, SEEK_END);
    // long file_size = ftell(input);
    // fseek(input, 0, SEEK_SET);
    struct stat file_stat;

    if (stat(argv[1], &file_stat) == -1) {
        perror("Error getting file status");
        return 1;
    }

    long file_size = file_stat.st_size;

    // Read the magic number
    char magic_number[5]; // Including the null terminator
    if (fread(magic_number, sizeof(char), 4, input) != 4)
    {
        fprintf(stderr, "Error reading magic number\n");
        fclose(input);
        return EXIT_FAILURE;
    }
    magic_number[4] = '\0'; // Null-terminate the string

    // Check if the magic number is valid
    if (strcmp(magic_number, "VIRL") != 0 && strcmp(magic_number, "VIRB") != 0)
    {
        fprintf(stderr, "Invalid magic number: %s\n", magic_number);
        fclose(input);
        return EXIT_FAILURE;
    }

    // Process the virus signatures
    while (ftell(input) < file_size)
    {
        virus *new_virus = readVirus(input);
        if (new_virus == NULL)
        {
            fprintf(stderr, "Error reading virus\n");
            fclose(input);
            return EXIT_FAILURE;
        }

        printVirus(new_virus, stdout);
        free(new_virus->sig);
        free(new_virus);
    }

    fclose(input);
    return EXIT_SUCCESS;
}
