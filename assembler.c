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
    fp = fopen("test1PA.as", "r");
    flagPA = preassembler(fp, "test1PA.am");
    if (flagPA == errorEncounteredPA) {
        remove("test1PA.am");
    }
    fclose(fp);
    fp = fopen("test1PA.am", "r");
    stageOne(fp, "test1PA");
    fclose(fp);
    freeTableNames(&macroHashTable);
    freeTableNames(&symbolHashTable);
    return 0;
}


