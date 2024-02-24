#include <stdio.h>

int digit_counter(char* str) {
    int count = 0;
    while (*str != '\0') {
        if (*str >= '0' && *str <= '9') {
            count++;
        }
        str++;
    }
    return count;
}

int main(int argc, char **argv){
    if (argc < 2) {
        printf("usage: ntsc <string>\n");
    } else {
        printf("The number of digits in the string is: %d\n", digit_counter(argv[1]));
    }
}