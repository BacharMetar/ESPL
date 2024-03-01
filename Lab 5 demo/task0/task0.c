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

void print_program_header(ELF_PROGRAM_HEADER *program_header, int i)
{
  printf("Program header number %d at address 0x%08x\n", i, program_header->p_vaddr);
}

int foreach_phdr(void *map_start, void (*func)(ELF_PROGRAM_HEADER *,int), int arg)
{
    ELF_HEADER* const elf_header = (ELF_HEADER*) map_start;

    for(int i = 0; i < elf_header->e_phnum; i++)
    {
        ELF_PROGRAM_HEADER* current_program_header = map_start + elf_header->e_phoff + i * elf_header->e_phentsize;
        func(current_program_header , i);
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    const int file_descriptor = open("a.out", O_RDONLY);
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

    foreach_phdr(map_start, print_program_header, 0);

    if(munmap(map_start, file_descriptor_stat.st_size) == -1) 
    {
        perror("munmap failed");
        exit(1);
    }

    close(file_descriptor);

    return 0;
}
