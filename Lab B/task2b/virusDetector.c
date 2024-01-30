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

typedef struct link
{
    struct link *nextVirus;
    virus *vir;
} link;

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

void list_print(link *virus_list, FILE *stream)
{
    if (virus_list == NULL)
    {
        printf("The virus list is empty.\n");
        return;
    }
    // Traverse the linked list
    link *current = virus_list;
    while (current != NULL)
    {
        // Print virus details to the given stream
        fprintf(stream, "Virus Name: %s\n", current->vir->virusName);
        fprintf(stream, "Signature Size: %hu\n", current->vir->SigSize);
        fprintf(stream, "Signature (Hexadecimal): ");
        for (int i = 0; i < current->vir->SigSize; i++)
        {
            fprintf(stream, "%02X ", current->vir->sig[i]);
        }
        fprintf(stream, "\n\n");

        // Move to the next link
        current = current->nextVirus;
    }
}

link *list_append(link *virus_list, virus *data)
{
    // Create a new link
    link *new_link = malloc(sizeof(link));
    if (new_link == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_link->vir = data;
    new_link->nextVirus = NULL; // Ensure that the new link points to NULL initially

    // If the list is empty, set the new link as the first link and return
    if (virus_list == NULL)
    {
        return new_link;
    }

    // Traverse the list to find the last link
    link *current = virus_list;
    while (current->nextVirus != NULL)
    {
        current = current->nextVirus;
    }

    // Append the new link to the end of the list
    current->nextVirus = new_link;

    // Return a pointer to the first link in the list
    return virus_list;
}

void list_free(link *virus_list)
{
    // Traverse the list
    while (virus_list != NULL)
    {
        link *current = virus_list;         // Store the current link
        virus_list = virus_list->nextVirus; // Move to the next link
        // Free the virus data
        free(current->vir->sig);
        free(current->vir);
        // Free the link itself
        free(current);
    }
}

// Define the function descriptor struct
// struct fun_desc
// {
//     char *name;
//     link *(*fun)(link *);
// };

struct fun_desc
{
    char *name;
    link *(*fun)(link *, char *); // Update the function signature
};

// Function prototypes
link *load_signatures(link *virus_list, char *file_name);
link *print_signatures(link *virus_list, char *file_name);
link *detect_viruses(link *virus_list, char *file_name);
link *fix_file(link *virus_list, char *file_name);

link *load_signatures(link *virus_list, char *file_name)
{
    const int BUFFER_SIZE = 128;
    char filename[BUFFER_SIZE];

    // Prompt the user for the signature file name
    printf("Enter signature file name: ");
    fgets(filename, BUFFER_SIZE, stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Remove trailing newline

    // Open the signature file for reading
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Find the length of the file
    fseek(file, 0, SEEK_END);
    const long FILE_SIZE = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the magic number
    char magic_number[5]; // Including the null terminator
    if (fread(magic_number, sizeof(char), 4, file) != 4)
    {
        fprintf(stderr, "Error reading magic number\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // Read viruses from the file and append them to the virus list
    while (ftell(file) < FILE_SIZE)
    {
        virus *new_virus = readVirus(file);
        if (new_virus == NULL)
        {
            fprintf(stderr, "Error reading virus\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        virus_list = list_append(virus_list, new_virus);
    }

    fclose(file);

    return virus_list;
}

link *print_signatures(link *virus_list, char *file_name)
{
    list_print(virus_list, stdout);
    return virus_list;
}

void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    if (buffer == NULL || virus_list == NULL || size == 0)
    {
        printf("Invalid buffer or virus list\n");
        return;
    }

    // Traverse through the virus list and compare with file content
    link *current = virus_list;
    while (current != NULL)
    {
        virus *current_virus = current->vir;
        unsigned short signature_size = current_virus->SigSize;

        // Compare the virus signature with the buffer content
        for (int i = 0; i <= size - signature_size; i++)
        {
            if (memcmp(buffer + i, current_virus->sig, signature_size) == 0)
            {
                printf("Virus detected:\n");
                printf("Start byte location: %d\n", i);
                printf("Virus name: %s\n", current_virus->virusName);
                printf("Signature size: %hu\n", current_virus->SigSize);
            }
        }

        current = current->nextVirus;
    }
}

link *detect_viruses(link *virus_list, char *file_name)
{
    const int BUFFER_SIZE = 10240; // 10K bytes buffer size
    char buffer[BUFFER_SIZE];

    // Open the file indicated by the command-line argument FILE
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return virus_list;
    }

    // fread() the entire contents of the suspected file into the buffer
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);

    // Scan the content of the buffer to detect viruses
    detect_virus(buffer, (unsigned int)bytes_read, virus_list);

    return virus_list;
}

// link *fix_file(link *virus_list, char *file_name) {
//     if (virus_list == NULL || file_name == NULL) {
//         printf("Invalid virus list or file name\n");
//         return virus_list;
//     }

//     FILE *file = fopen(file_name, "rb+");
//     if (file == NULL) {
//         perror("Error opening suspected file");
//         return virus_list;
//     }

//     // Prompt the user for the starting byte location
//     int start_byte;
//     printf("Enter the starting byte location of the virus: ");
//     scanf("%d", &start_byte);

//     // Prompt the user for the signature size
//     unsigned short signature_size;
//     printf("Enter the signature size of the virus: ");
//     scanf("%hu", &signature_size);

//     // Move to the location of the virus in the file
//     fseek(file, start_byte, SEEK_SET);

//     // Neutralize the virus by writing the RET instruction (0xC3)
//     unsigned char retInstruction = 0xC3;
//     for (int i = 0; i < signature_size; ++i) {
//         fwrite(&retInstruction, sizeof(unsigned char), 1, file);
//     }

//     fclose(file);
//     printf("Virus at byte %d with signature size %hu has been fixed.\n", start_byte, signature_size);
//     return virus_list;
// }

// Define a global variable to store the signature size
short signitureSize = 0;

void neutralize_virus(char *fileName, int signatureOffset)
{
    FILE *file = fopen(fileName, "r+b"); // Open the file for read/write in binary mode
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Move the file pointer to the location of the virus signature offset
    fseek(file, signatureOffset, SEEK_SET);

    // Write the RET instruction to the first byte of the virus signature
    // unsigned char ret_instruction = 0xC3; // RET instruction in x86 assembly
    // fwrite(&ret_instruction, sizeof(unsigned char), 1, file);

    // fclose(file);
    char buffer[signitureSize];
    for(int i = 0; i < signitureSize; i++)
    {
        buffer[i] = 0;
    }

    fwrite(buffer, 1, signitureSize, file);

    fclose(file);
}

link *fix_file(link *virus_list, char *file_name)
{
    // Open the suspected file
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return virus_list;
    }
    long startByte = 0; // Initialize the start byte

    // Iterate through the linked list of viruses
    link *current = virus_list;
    while (current != NULL)
    {
        fseek(file, startByte, SEEK_SET);
        // Neutralize the virus by calling neutralize_virus function
        signitureSize = current->vir->SigSize;
        neutralize_virus(file_name, startByte);

        // Move to the next virus
        startByte += current->vir->SigSize;

        current = current->nextVirus;
    }

    fclose(file);
    return virus_list;
}






// Main function
int main(int argc, char **argv)
{

    // Open the suspected file
    char *suspected_file = argv[1];
    if (suspected_file == NULL)
    {
        perror("Error opening suspected file");
        return EXIT_FAILURE;
    }

    // Allocate a buffer to hold the contents of the suspected file

    //***********************************************************
    // Initialize virus list
    link *virus_list = NULL;

    // Array of function descriptors
    struct fun_desc menu[] = {
        {"Load signatures", load_signatures},
        {"Print signatures", print_signatures},
        {"Detect viruses", detect_viruses},
        {"Fix file", fix_file},
        {"Quit", NULL}, // End of menu marker
        {NULL, NULL}    // mark the end of the menu

    };
    int choice;
    while (1)
    {
        // Display the menu
        printf("\nMenu:\n");
        for (int i = 0; menu[i].name != NULL; i++)
        {
            printf("%d) %s\n", i + 1, menu[i].name);
        }
        // Prompt for choice
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        // Check if the choice is within bounds
        if (choice < 1 || choice > 5)
        {
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        // Execute the selected function
        if (menu[choice - 1].fun != NULL)
        {
            if (choice != 3)
            {
                virus_list = menu[choice - 1].fun(virus_list, suspected_file);
            }
            else
            {
                detect_viruses(virus_list, suspected_file);
            }
        }
        else
        {
            printf("Quitting...\n");
            break;
        }
    }

    // Free memory before exiting
    // Add code to free virus_list

    return 0;
}