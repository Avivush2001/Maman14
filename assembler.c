#include "data.h"


/*Global Assembler Tables*/
HashTableItem macroItems[HASHSIZE];
HashTableItem symbolItems[HASHSIZE];
HashTable macroHashTable = {macroItems, 0, hashTableFree};
HashTable symbolHashTable = {symbolItems, 0, hashTableFree};

int main() {
    /*Can't be initialized another way besides allocating memory.*/
    
    FILE *fp;
    PreassemblerFlags flagPA;
    initializeHashTable(&macroHashTable);
    initializeHashTable(&symbolHashTable);
    fp = fopen("testOriginal.as", "r");
    flagPA = preassembler(fp, "testOriginal");
    if (flagPA == errorEncounteredPA) {
        remove("testOriginal.am");
    }
    fclose(fp);
    fp = fopen("testOriginal.am", "r");
    stageOne(fp);
    fclose(fp);
    freeTableNames(&macroHashTable);
    freeTableNames(&symbolHashTable);
    return 0;
}


