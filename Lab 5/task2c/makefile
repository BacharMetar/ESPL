all: clean task2c

task2c: task2c.o start.o startup.o
	gcc -m32 -c task2c.c -o task2c.o
	ld -o task2c task2c.o start.o startup.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

task2c.o: task2c.c
	gcc -m32 -g -Wall -c -o task2c.o task2c.c

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s

startup.o: startup.s
	nasm -g -f elf -w+all -o startup.o startup.s

.PHONY: clean

clean:
	rm -f *.o task2c
