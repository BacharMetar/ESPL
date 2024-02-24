all: hexeditplus

hexeditplus: task1.o
	gcc -m32 -g -Wall -o hexeditplus task1.o

task1.o: task1.c
	gcc -g -Wall -m32 -c -o task1.o task1.c

.PHONY: clean
clean:
	rm -f *.o hexeditplus