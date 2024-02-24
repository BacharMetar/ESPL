all: clean encoder

encoder: encoder.s
	nasm -g -f elf32 -o encoder.o encoder.s
	gcc -m32 -g -Wall -o encoder encoder.o -nostdlib
	rm encoder.o

.PHONY: clean all

clean:
	rm -rf ./*.o encoder