all:	data.h file.c assembler.c
	gcc -ansi -Wall -pedantic -g  data.h file.c assembler.c -o test -lm