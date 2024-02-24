all: clean start

start: start.s
	nasm -g -f elf32 -o start.o start.s
	gcc -m32 -g -Wall -o start start.o -nostdlib
	rm start.o

.PHONY: clean all

clean:
	rm -rf ./*.o start