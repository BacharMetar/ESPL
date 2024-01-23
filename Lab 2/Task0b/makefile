all:clean Looper

Looper: Looper.o
	gcc -g -m32 -Wall -o Looper Looper.o

Looper.o: Looper.c
	gcc -g -m32 -Wall -c -o Looper.o Looper.c

.PHONY: clean

clean:
	rm -f *.o Looper