all: clean loader

loader: loader.o start.o startup.o
	gcc -m32 -c loader.c -o loader.o
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

loader.o: loader.c
	gcc -m32 -g -Wall -c -o loader.o loader.c

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s

startup.o: start.s
	nasm -g -f elf -w+all -o startup.o startup.s

.PHONY: clean

clean:
	rm -f *.o loader