all: hexeditplus ntsc_correct

hexeditplus: task1.o
	gcc -m32 -g -Wall -o hexeditplus task1.o

task1.o: task1.c
	gcc -g -Wall -m32 -c -o task1.o task1.c

ntsc_correct: task4.o
	gcc -m32 -fno-pie -fno-stack-protector -g -Wall -o ntsc_correct task4.o

task4.o: task4.c
	gcc -g -Wall -m32 -fno-pie -fno-stack-protector -c -o task4.o task4.c

.PHONY: clean
clean:
	rm -f *.o hexeditplus ntsc_correct