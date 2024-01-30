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

// virus *readVirus(FILE *input)
// {
//     virus *new_virus = malloc(sizeof(virus));

//     if (fread(new_virus, 1, 18, input) > 0)
//     {
//         new_virus->sig = malloc(new_virus->SigSize);
//         fread(new_virus->sig, 1, new_virus->SigSize, input);
//         return new_virus;
//     }

//     else
//     {
//         free(new_virus);
//         return NULL;
//     }
// }

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
struct fun_desc
{
    char *name;
    link *(*fun)(link *);
};
// Function prototypes
link *load_signatures(link *virus_list);
link *print_signatures(link *virus_list);
link *detect_viruses(link *virus_list);
link *fix_file(link *virus_list);

link *load_signatures(link *virus_list) {
    const int BUFFER_SIZE = 128;
    char filename[BUFFER_SIZE];

    // Prompt the user for the signature file name
    printf("Enter signature file name: ");
    fgets(filename, BUFFER_SIZE, stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Remove trailing newline

    // Open the signature file for reading
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Find the length of the file
    fseek(file, 0, SEEK_END);
    const long FILE_SIZE = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read the magic number
    char magic_number[5]; // Including the null terminator
    if (fread(magic_number, sizeof(char), 4, file) != 4) {
        fprintf(stderr, "Error reading magic number\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // magic_number[4] = '\0'; // Null-terminate the string

    // // Check if the magic number is valid
    // if (strcmp(magic_number, "VIRL") != 0 && strcmp(magic_number, "VIRB") != 0) {
    //     fprintf(stderr, "Invalid magic number: %s\n", magic_number);
    //     fclose(file);
    //     exit(EXIT_FAILURE);
    // }
    
    // Read viruses from the file and append them to the virus list
    while (ftell(file) < FILE_SIZE) {
        virus *new_virus = readVirus(file);
        if (new_virus == NULL) {
            fprintf(stderr, "Error reading virus\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        virus_list = list_append(virus_list, new_virus);
    }

    fclose(file);

    return virus_list;
}

link *print_signatures(link *virus_list)
{
    list_print(virus_list, stdout);
    return virus_list;
}
link *detect_viruses(link *virus_list)
{
    return NULL;
}
link *fix_file(link *virus_list)
{
    return NULL;
}

// Main function
int main(int argc, char **argv)
{
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
        printf("HHHHEEEELLLOOOOO \n");
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
            virus_list = menu[choice - 1].fun(virus_list);
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