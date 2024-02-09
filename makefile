all:	data.h file.c assembler.c hashtable.c preassembler.c
	gcc -ansi -Wall -pedantic -g  data.h file.c assembler.c hashtable.c preassembler.c -o test -lm