#define RAM_SIZE 4096
#define MAX_LINE_LENGTH 81
#define MAX_LABEL_SIZE 32
#define HASHSIZE (RAM_SIZE / (MAX_LABEL_SIZE))
#define NOT_FOUND -1
#define OPERATIONS {"mov", "cmp", "add", "sub", "not", "clr","lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"}
#define REGISTERS {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"}
#define INSTRUCTIONS {".data",".string", ".entry" ,".extern", MACRO_DEF, MACRO_END}
#define OPERATIONS_SIZE 16
#define REGISTERS_SIZE 8
#define INSTRUCTIONS_SIZE 6
#define MACRO_DEF "mcr"
#define MACRO_END "endmcr"

/*
THESE ARE TEMPORARY UPDATE THEM AND THEIR FUNCTIONS WITH THE PROPER STRUCTS
*/
char * operationsArr[] = {"mov", "cmp", "add", "sub", "not", "clr","lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};
char * registersArr[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char * instructionArr[] = {".data",".string", ".entry" ,".extern", MACRO_DEF, MACRO_END};