#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Returns the encoded character using the given key and add flag (1 for addition and 0 for subtraction),
 * if the input character is valid (A-Z or 1-9), otherwise returns the same character back.
*/
char encode(char c, int key, int add) {
    if (c >= 'A' && c <= 'Z') {
        for(int i=0; i < key; i++) {
            if (add) {
                if (c == 'Z')
                    c = 'A';
                else
                    c++;
            }
            else {
                if (c == 'A')
                    c = 'Z';
                else
                    c--;
            }
        }
    }
    else if (c >= '0' && c <= '9') {
        for(int i=0; i < key; i++) {
            if (add) {
                if (c == '9')
                    c = '0';
                else
                    c++;
            }
            else {
                if (c == '0')
                    c = '9';
                else
                    c--;
            }
        }
    }
    return c;
}

int main(int argc, char **argv) {
    int debug = 1;
    int add = 1;
    char *key_string;
    FILE * infile = stdin;
    FILE * outfile = stdout;
    FILE * errfile = stderr;
    for(int i = 1; i < argc; i++) {
        if(debug) {
            fprintf(errfile,"%s\n",argv[i]);
        }
        if (argv[i][0] == '-' && argv[i][1] =='D') {
                debug = 0;
        }
        else if (argv[i][0] == '+' && argv[i][1] == 'D') {
                debug = 1;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'E') {
            add = 0;
            key_string = argv[i]+2; // Extract the {key} itself from -E{key}
        }
        else if (argv[i][0] == '+' && argv[i][1] == 'E') {
            add = 1;
            key_string = argv[i]+2; // Extract the {key} itself from -E{key}
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'I') {
            infile = fopen(argv[i]+2, "r");
            if (infile == NULL) {
                fprintf(errfile,"Error, could not open the given input file");
                exit(1);
            }
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'O') {
            outfile = fopen(argv[i]+2, "w");
            if (outfile == NULL) {
                fprintf(errfile,"Error, could not open the given output file");
                exit(1);
            }
        }
    }
    int input = 0;
    int key_index = 0;
    while(1) {
        input = fgetc(infile);
        if (feof(infile)) {
            break;
        }
        char encoded = encode((char)input,key_string[key_index]-'0',add);
        key_index++; // increment the key index in a cyclic manner
        if (key_string[key_index] == '\0') {
            key_index = 0;
        }
        fputc(encoded, outfile);
    }
    if (outfile!=stdout) {
        fclose(outfile);
    }
    if (errfile!=stderr) {
        fclose(errfile);
    }
    if (infile!=stdin) {
        fclose(infile);
    }
    return 0;
}