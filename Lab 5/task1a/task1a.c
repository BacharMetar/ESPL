#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <string.h>

// Assume get_phdr_type is defined elsewhere
const char *get_phdr_type(int p_type) {
    static const char *phdr_types[] = {
        "NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR"
    };

    if (p_type >= PT_NULL && p_type <= PT_PHDR) {
        return phdr_types[p_type - PT_NULL];
    } else {
        return "UNKNOWN";
    }
}

void print_program_header_info(Elf32_Phdr *phdr, int index) {
    printf("%-9s 0x%08x 0x%08x 0x%08x 0x%06x 0x%06x ", 
            get_phdr_type(phdr->p_type),
            phdr->p_offset,
            phdr->p_vaddr,
            phdr->p_paddr,
            phdr->p_filesz,
            phdr->p_memsz);

    // Print flags
    printf("%s  %s  %s ", 
            phdr->p_flags & PF_X ? "E" : " ", 
            phdr->p_flags & PF_W ? "W" : " ",
            phdr->p_flags & PF_R ? "R" : " ");

    printf("0x%06x\n", phdr->p_align);
}


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; ++i) {
        func(&phdr[i], i);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <executable>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    void *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    printf("Type      Offset     VirtAddr   PhysAddr   FileSiz  MemSiz    Flg     Align\n");
    foreach_phdr(map_start, print_program_header_info, 0);

    if (munmap(map_start, st.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}