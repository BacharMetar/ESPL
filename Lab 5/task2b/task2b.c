#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <string.h>
#include <bits/mman-map-flags-generic.h>

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

    printf("0x%06x", phdr->p_align);

    // Calculate and print protection and mapping flags
    int protection = 0;
    if (phdr->p_flags & PF_X) {
        protection |= PROT_EXEC;
    }
    if (phdr->p_flags & PF_W) {
        protection |= PROT_WRITE;
    }
    if (phdr->p_flags & PF_R) {
        protection |= PROT_READ;
    }

    int mapping = 0;
    if (phdr->p_type == PT_LOAD) {
        mapping |= MAP_PRIVATE | MAP_FIXED;
    }
    //TODO: check if any other header type is needed here.
    
    printf("  %5d\t%9d\n", protection, mapping);
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)((char *)map_start + ehdr->e_phoff);//adding the offset of the program header table to the starting address of the memory-mapped file.

    for (int i = 0; i < ehdr->e_phnum; ++i) {
        func(&phdr[i], arg);
    }

    return 0;
}


void load_phdr(Elf32_Phdr *phdr, int fd) {
    // Check if the program header type is PT_LOAD
    if (phdr->p_type == PT_LOAD) {
        // Determine the protection flags based on the segment's flags
        int prot = 0;
        if (phdr->p_flags & PF_X) prot |= PROT_EXEC;
        if (phdr->p_flags & PF_W) prot |= PROT_WRITE;
        if (phdr->p_flags & PF_R) prot |= PROT_READ;

        // Get the system's page size - https://man7.org/linux/man-pages/man2/getpagesize.2.html
        long page_size = sysconf(_SC_PAGE_SIZE);
        if (page_size == -1) {
            perror("sysconf");
            exit(EXIT_FAILURE);
        }
        // Align the offset and virtual address to page boundaries
        off_t offset = phdr->p_offset / page_size * page_size;
        off_t vaddr = phdr->p_vaddr / page_size * page_size;

        size_t padding = phdr->p_vaddr - vaddr; // Calculate padding
        // Map the segment into memory
        void *mapped_segment = mmap((void *)vaddr, phdr->p_memsz + padding , prot, MAP_PRIVATE | MAP_FIXED, fd, offset);


        // Check if mapping succeeded
        if (mapped_segment == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
        print_program_header_info(phdr, 0);
    }
}


int main(int argc, char *argv[]) {
    printf("My PID is: %d\n", getpid());
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

    printf("Type      Offset     VirtAddr   PhysAddr   FileSiz  MemSiz    Flg     Align    Protection    Mapping\n");

    // Call load_phdr for each program header
    foreach_phdr(map_start, load_phdr, fd);
    
    // a while loop to test the mmap funciton . 
    //using ps aux | grep <program_name> to get the pid and
    // pmap <pid> to show information about the memory mappings of our program
    char input;
    while (1) {
        scanf("%c", &input);
        if (input == '0') {
            break; // Exit the loop and terminate the program
        }
    }
    if (munmap(map_start, st.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}