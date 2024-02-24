all: mypipeline

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c

looper.o: looper.c
	gcc -g -Wall -m32 -c -o looper.o looper.c

looper: looper.o
	gcc -m32 -g -Wall -o looper looper.o

myshell.o: myshell.c
	gcc -g -Wall -m32 -c -o myshell.o myshell.c

myshell: myshell.o LineParser.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

mypipeline.o: mypipeline.c
	gcc -g -Wall -m32 -c -o mypipeline.o mypipeline.c

mypipeline: mypipeline.o
	gcc -m32 -g -Wall -o mypipeline mypipeline.o

.PHONY: clean
clean:
	rm -f *.o myshell mypipeline looper