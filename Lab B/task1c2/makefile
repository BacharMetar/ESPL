all: exec

exec: task1c.c
		gcc -g -c -o task1c.o task1c.c
		gcc -g task1c.o -o task1c
		rm task1c.o

.PHONY: clean
clean:
	rm -rf ./*.o task1c
