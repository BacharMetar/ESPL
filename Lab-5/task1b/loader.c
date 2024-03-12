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

const char *get_phdr_type(int p_type) {
    switch (p_type) { // Taken from ELF.h source file online
        case PT_NULL:         return "NULL";           /* Program header table entry unused */
        case PT_LOAD:         return "LOAD";           /* Loadable program segment */
        case PT_DYNAMIC:      return "DYNAMIC";        /* Dynamic linking information */
        case PT_INTERP:       return "INTERP";         /* Program interpreter */
        case PT_NOTE:         return "NOTE";           /* Auxiliary information */
        case PT_SHLIB:        return "SHLIB";          /* Reserved */
        case PT_PHDR:         return "PHDR";           /* Entry for header table itself */
        case PT_TLS:          return "TLS";            /* Thread-local storage segment */
        case PT_NUM:          return "NUM";            /* Number of defined types */
        case PT_LOOS:         return "LOOS";           /* Start of OS-specific */
        case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";   /* GCC .eh_frame_hdr segment */
        case PT_GNU_STACK:    return "GNU_STACK";      /* Indicates stack executability */
        case PT_GNU_RELRO:    return "GNU_RELRO";      /* Read-only after relocation */
        case PT_SUNWBSS:      return "SUNWBSS";        /* Sun Specific segment */
        case PT_SUNWSTACK:    return "SUNWSTACK";      /* Stack segment */
        case PT_HIOS:         return "HIOS";           /* End of OS-specific */
        case PT_LOPROC:       return "LOPROC";         /* Start of processor-specific */
        case PT_HIPROC:       return "HIPROC";         /* End of processor-specific */
        default:              return "UNKNOWN";
    }
}

void print_program_header(Elf32_Phdr *phdr, int index) {
    // printf("Program header number %d at address 0x%x:\n", index, phdr->p_vaddr);

    printf("%-16s ", get_phdr_type(phdr->p_type));
    printf("0x%06x 0x%08x 0x%08x 0x%05x 0x%05x ", phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz);
    
    // Print Flags
    printf("%s%s%s ",
        phdr->p_flags & PF_R ? "R" : " ",
        phdr->p_flags & PF_W ? "W" : " ",
        phdr->p_flags & PF_X ? "E" : " ");
    printf("0x%04x\t", phdr->p_align);

    // Calculate and print protection and mapping flags
    int protection_flags = 0;
    int mapping_flags = 0;
    if (phdr->p_flags & PF_R) {
        protection_flags |= PROT_READ;
    }
    if (phdr->p_flags & PF_W) {
        protection_flags |= PROT_WRITE;
    }
    if (phdr->p_flags & PF_X) {
        protection_flags |= PROT_EXEC;
    }

    if (phdr->p_type == PT_LOAD) {
        mapping_flags |= MAP_FIXED | MAP_PRIVATE;
    }
    // TODO: Do other types of program headers need to have different mapping flags? 
    // Because we only need to support static linked files, i.e. without program headers of type DYNAMIC or INTERP.
    printf("%d\t%d\n", protection_flags, mapping_flags);
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

    printf("Type\t\t Offset\t  VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\tProtect\tMapping\n");
    foreach_phdr(start_address, print_program_header, 0);
    munmap(start_address, file_size);
    close(fd);
    return 0;
}