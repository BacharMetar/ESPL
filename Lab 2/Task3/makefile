all: clean myshell

myshell: myshell.o LineParser.o
	gcc -g -m32 -Wall -o myshell myshell.o LineParser.o 

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c


.PHONY: clean

clean:
	rm -f *.o myshell