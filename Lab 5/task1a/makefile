all: clean task1a

task1a: task1a.o 
	gcc -m32 -g -Wall -o task1a task1a.o 

task1a.o: task1a.c
	gcc -g -Wall -m32 -c -o task1a.o task1a.c

.PHONY: clean

clean: 
	rm -f *.o task1a