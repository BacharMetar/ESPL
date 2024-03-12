all: clean task2d

task2d: task2d.o start.o startup.o
	gcc -m32 -c task2d.c -o task2d.o
	ld -o task2d task2d.o start.o startup.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

task2d.o: task2d.c
	gcc -m32 -g -Wall -c -o task2d.o task2d.c

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s

startup.o: startup.s
	nasm -g -f elf -w+all -o startup.o startup.s

.PHONY: clean

clean:
	rm -f *.o task2d
