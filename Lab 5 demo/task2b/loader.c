#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define true 1
#define false 0
#define INPUT_MAX_LENGTH 100
#define ELF_HEADER Elf32_Ehdr
#define ELF_SECTION_HEADER Elf32_Shdr
#define ELF_SYMBOL Elf32_Sym
#define ELF_RELOCATION_TABLE_ENTRY Elf32_Rel
#define ELF_PROGRAM_HEADER Elf32_Phdr

char* get_program_header_type(int type)
{
    switch (type){
        case PT_NULL:
            return "NULL";
        case PT_LOAD:
            return "LOAD";
        case PT_DYNAMIC:
            return "DYNAMIC";
        case PT_INTERP:
            return "INTERP";
        case PT_NOTE:
            return "NOTE";
        case PT_SHLIB:
            return "SHLIB";
        case PT_PHDR:
            return "PHDR";
        case PT_LOPROC:
            return "LOPROC";
        case PT_HIPROC:
            return "HIPROC";
        default:
            return "UNKNOWN";
    }
}

char* get_program_header_flag(int type)
{
    switch (type)
    {
        case PF_R: 
            return "R";
        case PF_W:
            return "W";
        case PF_X:
            return "X";
        case (PF_R | PF_W):
            return "R W";
        case (PF_R | PF_X):
            return "R X";
        case (PF_X | PF_W):
            return "W X";
        case (PF_R | PF_W | PF_X):
            return "R W X";
        default:
            return "UNKNOWN";
    }        
}

int get_program_header_protection_flag(int flag)
{
    int protection_flag = 0;  

    // check if representing bit is one.
    if((flag & PF_R) != 0)
    {
        protection_flag |= PROT_READ;
    }  

    if((flag & PF_W) != 0)
    {
        protection_flag |= PROT_WRITE;
    } 

    if((flag & PF_X) != 0)
    {
        protection_flag |= PROT_EXEC;
    } 

    return protection_flag;
}

int get_program_header_mapping_flag(ELF_PROGRAM_HEADER* program_header)
{
    if(program_header->p_type == PT_LOAD)
    {
        return MAP_PRIVATE | MAP_FIXED;
    }

    else
    {
        return 0;
    }
}

void print_program_header(ELF_PROGRAM_HEADER *program_header, int i)
{
    char* const type = get_program_header_type(program_header->p_type);
    const int offset = program_header->p_offset;
    const int virtual_adress = program_header->p_vaddr;
    const int physical_address = program_header->p_paddr;
    const int file_size = program_header->p_filesz;
    const int memory_size = program_header->p_memsz;
    char* const flags = get_program_header_flag(program_header->p_flags);
    const int align = program_header->p_align;
    const int protection_flag = get_program_header_protection_flag(program_header->p_flags);
    const int mapping_flag = get_program_header_mapping_flag(program_header);

    printf("%s\t\t%#08x\t%#08x\t%#10x\t%#07x\t%#07x\t%s\t%#-6x\t%5x\t%12x\n",
            type, offset, virtual_adress, physical_address, file_size, memory_size, flags, align, protection_flag, mapping_flag);
}

int foreach_phdr(void *map_start, void (*func)(ELF_PROGRAM_HEADER *,int), int arg)
{
    ELF_HEADER* const elf_header = (ELF_HEADER*) map_start;

    for(int i = 0; i < elf_header->e_phnum; i++)
    {
        ELF_PROGRAM_HEADER* current_program_header = map_start + elf_header->e_phoff + i * elf_header->e_phentsize;
        func(current_program_header , arg);
    }

    return 0;
}

void load_phdr(ELF_PROGRAM_HEADER *program_header, int fd)
{
    if(program_header->p_type != PT_LOAD)
    {
        return;
    }

    print_program_header(program_header, fd);

    const int offset = (program_header->p_offset & 0xfffff000);
    const int virtual_adress = (program_header->p_vaddr & 0xfffff000);
    const int padding = (virtual_adress & 0xfffff000);
    const int memory_size = program_header->p_memsz;
    int flags = MAP_PRIVATE | MAP_FIXED;
    const int protection_flag = get_program_header_protection_flag(program_header->p_flags);

    void* map_start = mmap((void*) virtual_adress, memory_size + padding, protection_flag, flags, fd, offset);

    if(map_start == MAP_FAILED)
    {
        perror("Program header mmap failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const **argv)
{
    if(argc != 2)
    {
        printf("INVALID FILE NAME\n");
        exit(EXIT_SUCCESS);
    }

    const char* file_name = argv[1];

    const int file_descriptor = open(file_name, O_RDONLY);
    struct stat file_descriptor_stat;

    if(file_descriptor == -1) 
    {
        perror("Cannot open file!");
        exit(EXIT_FAILURE);
    }

    if(fstat(file_descriptor, &file_descriptor_stat) == -1) 
    {
        perror("Stat failed");
        exit(EXIT_FAILURE);
    }

    void *map_start = mmap(NULL, file_descriptor_stat.st_size, PROT_READ, MAP_PRIVATE , file_descriptor, 0);
    
    if(map_start == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    printf("Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\tprotection\tmapping\n");   
    foreach_phdr(map_start, load_phdr, file_descriptor);
    printf("\n");
    char input;
    while (1) {
        scanf("%c", &input);
        if (input == '0') {
            break; // Exit the loop and terminate the program
        }
    }

    if(munmap(map_start, file_descriptor_stat.st_size) == -1) 
    {
        perror("munmap failed");
        exit(1);
    }

    close(file_descriptor);

    return 0;
}
