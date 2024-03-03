all: clean task2b

task2b: task2b.o start.o 
	gcc -m32 -c task2b.c -o task2b.o
	ld -o task2b task2b.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

task2b.o: task2b.c
	gcc -m32 -g -Wall -c -o task2b.o task2b.c

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s

.PHONY: clean

clean:
	rm -f *.o task2b