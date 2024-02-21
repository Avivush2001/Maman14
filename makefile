all:	data.h file.c assembler.c hashtable.c preassembler.c util.c
	gcc -ansi -Wall -pedantic -g  data.h util.c file.c assembler.c hashtable.c preassembler.c -o test.exe -lm