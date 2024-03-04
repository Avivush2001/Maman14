all:	data.h assembler.c hashtable.c preassembler.c util.c
	gcc -ansi -Wall -pedantic -g  data.h util.c assembler.c hashtable.c preassembler.c -o test.exe -lm