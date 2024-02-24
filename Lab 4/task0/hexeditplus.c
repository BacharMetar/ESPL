#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUT_MAX_LEN 128

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

typedef struct fun_desc {
  char *name;
  void (*fun)(state* s);
} fun_desc;

void toggle_debug_mode(state* s) {
    if (s->debug_mode) {
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    } else {
        s->debug_mode = 1;
        printf("Debug flag now on\n");
    }
}

void set_file_name(state* s) {
    printf("Enter file name:\n");
    char inputfilename[INPUT_MAX_LEN] = {0};
    if (inputfilename != fgets(inputfilename, INPUT_MAX_LEN, stdin)) {
        printf("An error has occurred when getting file name.\n");
        return;
    }
    sscanf(inputfilename, "%s\n", inputfilename);
    // Update the file_name variable if the input is valid
    memset(s->file_name, '\0', sizeof(s->file_name));
    strncpy(s->file_name, inputfilename, strlen(inputfilename));
    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void set_unit_size(state* s) {
    printf("Enter unit size:\n");
    char inputunitsize[INPUT_MAX_LEN] = {0};
    if (inputunitsize != fgets(inputunitsize, INPUT_MAX_LEN, stdin)) {
        printf("An error has occurred when getting unit size.\n");
        return;
    }
    sscanf(inputunitsize, "%s\n", inputunitsize);
    // Check invalid input
    char *endptr;
    long unitsize_convert = strtol(inputunitsize, &endptr, 10);
    if (*endptr != '\0') { // Check for conversion error
        fprintf(stderr, "Error: Invalid input given\n");
        return;
    }
    if (unitsize_convert != 1 && unitsize_convert != 2 && unitsize_convert != 4) {
        printf("Invalid unit size given, choose between: 1, 2, 4.\n");
        return;
    }
    // Update the unit_size variable if the input is valid
    s->unit_size = unitsize_convert;
    if (s->debug_mode) {
        fprintf(stderr, "Debug: set size to %d'\n", s->debug_mode);
    }
}

void quit(state* s) {
    if (s->debug_mode) {
        fprintf(stderr, "quitting\n");
    }
    exit(0);
}

void load_into_memory(state* s) {
    printf("Not implemented yet\n");
}

void toggle_display_mode(state* s) {
    printf("Not implemented yet\n");
}

void memory_display(state* s) {
    printf("Not implemented yet\n");
}

void save_into_file(state* s) {
    printf("Not implemented yet\n");
}

void memory_modify(state* s) {
    printf("Not implemented yet\n");
}

int main(int argc, char **argv){
    struct fun_desc menu[] = {
        { "Toggle Debug Mode", toggle_debug_mode },
        { "Set File Name", set_file_name },
        { "Set Unit Size", set_unit_size },
        { "Load Into Memory", load_into_memory },
        { "Toggle Display Mode", toggle_display_mode },
        { "Memory Display", memory_display },
        { "Save Into File", save_into_file },
        { "Memory Modify", memory_modify },
        { "Quit", quit },
        { NULL, NULL }
    };

    // Initialize my_state:
    state my_state;
    my_state.debug_mode = 0;
    memset(my_state.file_name, '\0', sizeof(my_state.file_name));
    my_state.unit_size = 1;
    my_state.mem_count = 0;
    // my_state.display_mode = 0;

    char input[INPUT_MAX_LEN];
    int lowerbound = 0;
    int upperbound = lowerbound + sizeof(menu)/sizeof(menu[0]) - 2;
    while(1) {
        // Display Menu
        for (int i=0; menu[i].name != NULL; i++) {
        printf("%d-%s\n", i, menu[i].name);
        }
        printf("\n");
        if (my_state.debug_mode) {
            fprintf(stderr, "unit_size: %d\n", my_state.unit_size);
            fprintf(stderr, "file_name: %s\n", my_state.file_name);
            fprintf(stderr, "mem_count: %d\n", my_state.mem_count);
            fprintf(stderr, "\n");
        }
        // Get user input
        printf("Select operation from the following menu:\n");
        fgets(input,INPUT_MAX_LEN,stdin);
        if (feof(stdin)) {
            break;
        }
        sscanf(input, "%s\n", input);
        // Check invalid input
        char *endptr;
        long choice = strtol(input, &endptr, 10);
        if (*endptr != '\0') { // Check for conversion error
            fprintf(stderr, "Error: Invalid input given\n\n");
            continue;
        }
        if (choice < lowerbound || choice > upperbound) {
            printf("Not within bounds\n\n");
            continue;
        }
        menu[choice].fun(&my_state);
        printf("\n");
    }
}
