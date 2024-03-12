#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

// typedef struct {
// Elf32_Word p_type; /* entry type */
// Elf32_Off p_offset; /* file offset */
// Elf32_Addr p_vaddr; /* virtual address */
// Elf32_Addr p_paddr; /* physical address */
// Elf32_Word p_filesz; /* file size */
// Elf32_Word p_memsz; /* memory size */
// Elf32_Word p_flags; /* entry flags */
// Elf32_Word p_align; /* memory/file alignment */
// } Elf32_Phdr;

const int E_PHOFF = 0x1C; // = 28 = Location of Program Header Offset from the start of ELF File
const int E_PHNUM = 0x2C; // = 44 = Location of Program Header Number from the start of ELF File
const int PH_SIZE = 0x20; // = 32 = Size of each Program Header
const char *ELF_MAGIC = "\x7F\x45\x4C\x46"; // The magic signature  at the beginning of an ELF file
const int ELF_MAGIC_SIZE = 4;

void print_program_header(Elf32_Phdr *phdr, int index) {
    printf("Program header number %d at address 0x%x\n", index, phdr->p_vaddr);
}

int is_elf_file(void *map_start) {
    return memcmp(map_start, ELF_MAGIC, ELF_MAGIC_SIZE) == 0;
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg) {
    if (!is_elf_file(map_start)) {  // Checks if the given file is an ELF file
        return -1;
    }
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) map_start;

    // Optional manual calculation of the program header offset and program header number from the elf file:
    // void *ph_offset = *(void **)(map_start + E_PHOFF);
    // unsigned short ph_num = *(unsigned short*)(map_start + E_PHNUM);

    // Debug prints:
    // printf("Elf file start address: %p\n", map_start);
    // printf("Program header offset: %d\n", ehdr->e_phoff);
    // printf("Program header number: %d\n", ehdr->e_phnum);

    for (int index = 0; index < ehdr->e_phnum; index++) {
        Elf32_Phdr *current_phdr = (Elf32_Phdr *) (map_start + ehdr->e_phoff + index * sizeof(Elf32_Phdr));
        func(current_phdr, index);
    }
    return 0;
}

int main(int argc, char *argv[], char *envp[]) {
    if (argc != 2) {
        printf("Usage: ./loader <elf-filename>\n");
        return 0;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf("Given filename does not exist.\n");
        return 0;
    }
    int file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    void *start_address;
    start_address = mmap(start_address, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (!is_elf_file(start_address)) {  // Checks if the given file is an ELF file
        printf("Not an ELF file.\n");
        return 1;
    }

    foreach_phdr(start_address, print_program_header, 0);
    munmap(start_address, file_size);
    close(fd);
    return 0;
}