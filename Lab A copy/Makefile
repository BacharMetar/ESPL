all: output

output: main.o numbers.o add.o
	gcc -g -Wall -o output main.o numbers.o add.o

main.o: main.c
	gcc -g -Wall -c -o main.o main.c

numbers.o: numbers.c
	gcc -g -Wall -c -o numbers.o numbers.c

add.o: add.s
	nasm -g -f elf -w+all -o add.o add.s

clean:
	rm -f *.o output
