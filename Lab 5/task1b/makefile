all: clean task1b

task1b: task1b.o 
	gcc -m32 -g -Wall -o task1b task1b.o 

task1b.o: task1b.c
	gcc -g -Wall -m32 -c -o task1b.o task1b.c

.PHONY: clean

clean: 
	rm -f *.o task1b