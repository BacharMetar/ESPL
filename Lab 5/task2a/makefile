all: clean task2a

task2a: task2a.o start.o 
	gcc -m32 -c task2a.c -o task2a.o
	ld -o task2a task2a.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

task2a.o: task2a.c
	gcc -m32 -g -Wall -c -o task2a.o task2a.c

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s

.PHONY: clean

clean:
	rm -f *.o task2a