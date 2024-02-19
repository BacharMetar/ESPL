#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *output_file = NULL;
    for (int i = 1; i < argc; i++) {
      
        if (strncmp(argv[i], "-o", 2) == 0) {
          
            char *filename = argv[i] + 2;
            

            output_file = fopen(filename, "w");
            
       
            if (output_file != NULL) {
                printf("Opened file: %s\n", filename);
            
                fclose(output_file);
            } else {
                printf("Error: Could not open file %s\n", filename);
            }
        }
    }

    return 0;
}
