all: clean print

print: print.s
	nasm -g -f elf32 -o print.o print.s
	gcc -m32 -g -Wall -o print print.o -nostdlib
	rm print.o

.PHONY: clean all

clean:
	rm -rf ./*.o print