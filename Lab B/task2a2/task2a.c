#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short signature_size;
    char virus_name[16];
    unsigned char* signature;
} virus;

const int BYTES_FOR_NAME_AND_SIGNATURE_SIZE = 18;

virus* readVirus(FILE* input)
{
    virus* new_virus = malloc(sizeof(virus));

    if(fread(new_virus, 1, BYTES_FOR_NAME_AND_SIGNATURE_SIZE, input) > 0)
    {
        new_virus->signature = malloc(new_virus->signature_size);
        fread(new_virus->signature, 1, new_virus->signature_size, input);
        return new_virus;
    }

    else
    {
        free(new_virus);
        return NULL;
    }
    
}
void printHex(unsigned char* buffer, unsigned short length, FILE* output)
{
    for(int i = 0; i < length; i++)
    {
        fprintf(output, "%02hhX ", buffer[i]);
    }

    fprintf(output, "\n");
}
void printVirus(virus* this_virus, FILE* output)
{
    printf("Virus name: %s\nVirus size: %u\nSignature: ",this_virus->virus_name, this_virus->signature_size);
    printHex(this_virus->signature, this_virus->signature_size, output);
    printf("\n");
}
//---------task1b---------
typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};
//----------------
//auxiliary functions
int get_file_size(FILE* file)
{
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}
int minimum(int a, int b)
{
    if(a < b)
    {
        return a;
    }

    else
    {
        return b;
    }
}
//------------
void list_print(link *virus_list, FILE* output)
/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
{
    link* current_virus_link = virus_list;

    while (current_virus_link != NULL)
    {
        printVirus(current_virus_link->vir, output);
        current_virus_link = current_virus_link->nextVirus;
    }    
}
//---------------
link* new_link(virus* data)
{
    link* output = malloc(sizeof(link));
    output->vir = data;
    output->nextVirus = NULL;

    return output;
}
link* list_append(link* virus_list, virus* data)
/* Add a new link with the given data to the list (either at the end or the beginning, depending on what your TA tells you), and return a pointer to the list
 (i.e., the first link in the list). If the list is null - create a new entry and return a pointer to the entry. */
{
    if(virus_list == NULL)
    {
        virus_list = new_link(data);
    }

    else
    {
        virus_list->nextVirus = list_append(virus_list->nextVirus, data);
    }

    return virus_list;
}
//----------------
void virus_link_free(link* virus_link)
{
    free(virus_link->vir->signature);
    free(virus_link->vir);
    free(virus_link);
}
void list_free(link *virus_list)
/* Free the memory allocated by the list. */
{
    link* current_virus_link = virus_list;

    if(current_virus_link != NULL)
    {
        list_free(virus_list->nextVirus);
        virus_link_free(current_virus_link);
    }
}
//------------
link* load_list(FILE* input)
{
    link* output_list = NULL;
    const int FILE_SIZE = get_file_size(input);
    //-------------------
    const int OFFSET = 4;

    char buffer[OFFSET];

    int total_bytes = fread(buffer, 1, 4, input);

    while (total_bytes < FILE_SIZE)
    {
        virus* new_virus = readVirus(input);
        output_list = list_append(output_list, new_virus);
        total_bytes += (BYTES_FOR_NAME_AND_SIGNATURE_SIZE + new_virus->signature_size);
    }

    return output_list;
}
link* load_signature(link* UNUSED1, char* UNUSED2)
{
    const int BUFFER_SIZE = 128;//arbitrary large enough size
    char buffer[BUFFER_SIZE];
    printf("enter file name: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    char* file_name = NULL;
    sscanf(buffer, "%ms", &file_name);
    FILE* input = fopen(file_name, "rb");//"rb" means read bytes

    if(input == NULL)
    {
        printf("cannot open file!\n");
        exit(0);
    }

    link* output = load_list(input);

    fclose(input);

    return output;
}
//----------------
link* print_signature(link* signatures, char* UNUSED)
{
    list_print(signatures, stdout);
    return signatures;
}
//----------------
void detect_virus(char *buffer, unsigned int size, link *virus_list)
{
    link* const list_head = virus_list;

    for(int starting_position = 0; starting_position < size; starting_position++)
    {
        link* current_virus_link = list_head;

        while(current_virus_link != NULL)
        {
            int is_clean = 1;

            if(size - starting_position >= current_virus_link->vir->signature_size)
            {
                is_clean = memcmp(buffer + starting_position, current_virus_link->vir->signature, current_virus_link->vir->signature_size);
            }

            if(is_clean == 0)
            {
                printf("starting byte location: %d\n", starting_position);
                printf("virus name: %s\n", current_virus_link->vir->virus_name);
                printf("virus signature size: %d\n", current_virus_link->vir->signature_size);
            }

            current_virus_link = current_virus_link->nextVirus;
        }
    }
}
link* detect_viruses(link* virus_list, char* file_name)
{
    const int BUFFER_SIZE = 10000;
    char buffer[BUFFER_SIZE];

    FILE* input = fopen(file_name, "rb");//"rb" means read bytes

    if(input == NULL)
    {
        printf("cannot open file!\n");
        exit(0);
    }

    const int FILE_SIZE = get_file_size(input);

    fread(buffer, 1, minimum(BUFFER_SIZE, FILE_SIZE), input);
    detect_virus(buffer, minimum(BUFFER_SIZE, FILE_SIZE), virus_list);

    fclose(input);

    return virus_list;
}
//---------
link* fix_file()
{
    printf("Not implemented\n");
    return NULL;
}
//--------
link* quit()
{
    exit(0);
}
//---------
typedef struct menu{
    char* name;
    link* (*fun)(link*, char*);
} menu;
/*
1) Load signatures
2) Print signatures
3) Detect viruses
4) Fix file
5) Quit
*/
int main(int argc, char **argv)
{
    link* list = NULL;

    menu options_menu[] = {{"Load Signatures", load_signature},
                            {"Print signatures", print_signature}, 
                            {"Detect viruses", detect_viruses},
                            {"Fix file", fix_file},
                            {"Quit", quit},
                            {NULL, NULL}};

    const int MENU_LENGTH = sizeof(options_menu) / (sizeof(menu)) - 1;
    int option;

    char* file_name;
    FILE* input;

    if(argc > 1)
    {
        file_name = argv[1];        
    }


    while(1)
    {
        printf("Please choose a function:\n");

        for(int i = 0; i < MENU_LENGTH; i++)
        {
            printf("%d)\t%s\n", i + 1, options_menu[i].name);
        }
        
        printf("Option : ");

        scanf("%d", &option);   

        if(option < 1 || option > MENU_LENGTH)
        {
            printf("Not within bounds\n");
            exit(0);
        }

        printf("Within bounds\n\n");

        fgetc(stdin);//flush the scanf input

        list = options_menu[option - 1].fun(list, file_name);

        printf("DONE.\n\n");
    }

    list_free(list);

    return 0;
}
