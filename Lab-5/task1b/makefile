all: clean loader

loader: loader.o
	gcc -m32 -g -Wall -o loader loader.o

loader.o:
	gcc -m32 -c loader.c -o loader.o

.PHONY: clean
clean:
	rm -f *.o loader