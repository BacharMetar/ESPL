#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int encodingMode = 1;  // Example: 1 for addition, -1 for subtraction
    int encodingKey = 0;   // Initialize encoding key

    // Check for encoding flag and key in command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '+' && argv[i][1] == 'E') {
            encodingMode = 1;
            encodingKey = atoi(argv[i] + 2);
        } else if (argv[i][0] == '-' && argv[i][1] == 'E') {
            encodingMode = -1;
            encodingKey = atoi(argv[i] + 2);
        }
    }

    printf("Encoding Key: %d\n", encodingKey);

    int c;
    while ((c = fgetc(stdin)) != EOF) {
        printf("Original char: %c\n", c);

        // Apply encoding only to uppercase letters and digits
        if ((c >= 'A' && c <= 'Z')) {
            if (encodingMode == 1) {
                int check = c + encodingKey;
                if (check <= 'Z') {
                    c = check;
                } else {
                    c = 'A' + (check - 'Z' - 1);
                }
            } else if (encodingMode == -1) {
                int check = c - encodingKey;
                if (check >= 'A') {
                    c = check;
                } else {
                    c = 'Z' - ('A' - check - 1);
                }
            }
        } else if (c >= '0' && c <= '9') {
            if (encodingMode == 1) {
                int check = c + encodingKey;
                if (check <= '9') {
                    c = check;
                } else {
                    c = '0' + (check - '9' - 1);
                }
            } else if (encodingMode == -1) {
                int check = c - encodingKey;
                if (check >= '0') {
                    c = check;
                } else {
                    c = '9' - ('0' - check - 1);
                }
            }
        }

        printf("Encoded char: %c\n", c);
    }

    return 0;
}
