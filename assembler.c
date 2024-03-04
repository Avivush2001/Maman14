#include "data.h"


/*I made the items arrays global so Errors won't be thrown later*/
HashTableItem macroItems[HASHSIZE];
HashTableItem symbolItems[HASHSIZE];
HashTable macroHashTable = {macroItems, 0, hashTableFree};
HashTable symbolHashTable = {symbolItems, 0, hashTableFree};
    /*Operation operation1 = {"code", code, {True,True,True,True},{True,True,True,True} , 0},
     operation2 = {"code2", code, {True,True,True,True},{True,True,True,True} , 0};*/

int main() {
    /*Can't be initialized another way besides allocating memory.*/
    
    FILE *fp;
    PreassemblerFlags flagPA;
    initializeHashTable(&macroHashTable);
    fp = fopen("test1PA.as", "r");
    flagPA = preassembler(fp, "test1PA.am");
    if (flagPA == errorEncounteredPA) {
        remove("test1PA.am");
    }
    fclose(fp);
    freeTableNames(&macroHashTable);
    return 0;
}


