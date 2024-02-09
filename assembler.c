#include "data.h"



int main() {
    SymbolHashTable macroHashTable;
    FILE *fp, *nfp;
    fp = fopen("testPA1.as", "r");
    nfp = preassembler(fp, "testPA1.am", &macroHashTable);
    fclose(fp);
    fclose(nfp);
    freeTableNames(&macroHashTable);
}