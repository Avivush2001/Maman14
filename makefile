all:	data.h assembler.c hashtable.c preassembler.c util.c memory.c stageOne.c
	gcc -ansi -Wall -pedantic -g  data.h util.c assembler.c hashtable.c preassembler.c memory.c stageOne.c -o test.exe -lm