#include "data.h"

char * operationsArr[] = {"mov", "cmp", "add", "sub", "not", "clr","lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};
char * registersArr[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char * instructionArr[] = {".data",".string", ".entry" ,".extern", MACRO_DEF, MACRO_END};
int main() {
    SymbolHashTable *macroHashTable;
    FILE *fp, *nfp;
    macroHashTable = malloc(sizeof(SymbolHashTable));
    fp = fopen("test1PA.as", "r");
    nfp = preassembler(fp, "test1PA.am", macroHashTable);
    if (fp != nfp) {
        fclose(nfp);
    }
    fclose(fp);
    freeTableNames(macroHashTable);
    
    return 0;
}

