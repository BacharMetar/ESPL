#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <string.h>


void print_program_header(Elf32_Phdr *phdr, int index) {
    printf("Program header number %d at address 0x%x\n", index, phdr->p_vaddr);
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)((char *)map_start + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; ++i) {
        func(&phdr[i], arg);
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

    foreach_phdr(map_start, print_program_header, 0);

    if (munmap(map_start, st.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}
