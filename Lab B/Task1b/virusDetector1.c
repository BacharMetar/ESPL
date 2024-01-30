#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

typedef struct link link;
struct link
{
    link *nextVirus;
    virus *vir;
};

virus *readVirus(FILE *input)
{
    virus *new_virus = malloc(sizeof(virus));

    if (fread(new_virus, 1, 18, input) > 0)
    {
        new_virus->sig = malloc(new_virus->SigSize);
        fread(new_virus->sig, 1, new_virus->SigSize, input);
        return new_virus;
    }

    else
    {
        free(new_virus);
        return NULL;
    }
}

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
     if (virus_list == NULL) {
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
link *load_signatures(link *virus_list)
{
    char filename[256];
    printf("Enter signature file name: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // Remove trailing newline

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return virus_list;
    }
    int i = 0;
    virus *new_virus;
    while ((new_virus = readVirus(file)) != NULL)
    {
        virus_list = list_append(virus_list, new_virus);
        printf("Attempt %d\n", i);
        i++;
    }

    fclose(file);
    printf("Signatures loaded successfully\n");
    return virus_list;
}
// Function prototypes
virus *readVirus(FILE *input);
void printVirus(virus *virus, FILE *output);
void list_print(link *virus_list, FILE *stream);
link *list_append(link *virus_list, virus *data);
void list_free(link *virus_list);
link *load_signatures(link *virus_list);

int main(int argc, char **argv)
{
    link *virus_list = NULL;
    int choice;

    while (1)
    {
        printf("1) Load signatures\n");
        printf("2) Print signatures\n");
        printf("3) Detect viruses\n");
        printf("4) Fix file\n");
        printf("5) Quit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        if (choice == 1)
        {
            virus_list = load_signatures(virus_list);
        }
        else if (choice == 2)
        {
            list_print(virus_list, stdout);
        }
        else if (choice == 3)
        {
            printf("Detecting viruses...\n");
            // Implement virus detection logic
        }
        else if (choice == 4)
        {
            printf("Fixing file...\n");
            // Implement file fixing logic
        }
        else if (choice == 5)
        {
            printf("Quitting program.\n");
            list_free(virus_list);
            exit(0);
        }
        else
        {
            printf("Invalid choice. Please enter a number from 1 to 5.\n");
        }
    }

    return 0;
}