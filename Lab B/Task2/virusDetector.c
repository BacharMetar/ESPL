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
link *load_signatures(link *virus_list,char* file_name);
link *print_signatures(link *virus_list,char* file_name);
link *detect_viruses(link *virus_list, char* file_name);
link *fix_file(link *virus_list,char* file_name);

link *load_signatures(link *virus_list,char* file_name)
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

link *print_signatures(link *virus_list,char* file_name)
{
    list_print(virus_list, stdout);
    return virus_list;
}
// // link *detect_viruses(link *virus_list)
// // {
// //     // Prompt the user for the file to scan
// //     char filename[256];
// //     printf("Enter the file name to scan for viruses: ");
// //     fgets(filename, sizeof(filename), stdin);
// //     filename[strcspn(filename, "\n")] = '\0'; // Remove trailing newline

// //     // Open the file for reading
// //     FILE *file = fopen(filename, "rb");
// //     if (file == NULL)
// //     {
// //         perror("Error opening file");
// //         return virus_list;
// //     }

// //     // Read the content of the file
// //     fseek(file, 0, SEEK_END);
// //     long file_size = ftell(file);
// //     fseek(file, 0, SEEK_SET);

// //     unsigned char *file_content = malloc(file_size);
// //     if (file_content == NULL)
// //     {
// //         perror("Memory allocation failed");
// //         fclose(file);
// //         return virus_list;
// //     }

// //     fread(file_content, 1, file_size, file);
// //     fclose(file);

// //     // Traverse through the virus list and compare with file content
// //     int virus_count = 0;
// //     link *current = virus_list;
// //     while (current != NULL)
// //     {
// //         virus *current_virus = current->vir;
// //         for (int i = 0; i <= file_size - current_virus->SigSize; i++)
// //         {
// //             if (memcmp(file_content + i, current_virus->sig, current_virus->SigSize) == 0)
// //             {
// //                 printf("Virus detected: %s at position %d\n", current_virus->virusName, i);
// //                 virus_count++;
// //             }
// //         }
// //         current = current->nextVirus;
// //     }

// //     if (virus_count == 0)
// //     {
// //         printf("No viruses detected in the file.\n");
// //     }

// //     free(file_content);
// //     return virus_list;
// // }

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

link *detect_viruses(link *virus_list, char* file_name)
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

link *fix_file(link *virus_list,char* file_name)
{
    return NULL;
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
                virus_list = menu[choice - 1].fun(virus_list,suspected_file);
            }
            else
            {
                detect_viruses(virus_list,suspected_file);
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