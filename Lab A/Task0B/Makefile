my_echo: my_echo.o
	gcc -m32 -g -Wall -o my_echo my_echo.o

my_echo.o: my_echo.c
	gcc -m32 -g -Wall -c -o my_echo.o my_echo.c

clean:
	rm -f my_echo my_echo.o
