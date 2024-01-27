all: clean bubblesort

bubblesort: bubblesort.o
	gcc -g -m32 -Wall -o bubblesort bubblesort.o

bubblesort.o: bubblesort.c
	gcc -g -m32 -Wall -c -o bubblesort.o bubblesort.c

.PHONY: clean

clean:
	rm -f *.o bubblesort
