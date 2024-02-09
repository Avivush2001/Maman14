#include "data.h"

/*Initialize a hashtable by setting the itemCount to 0 and the flag to hashTableFree*/
SymbolHashTable *initializeHashTable(SymbolHashTable *table) {
    int i;
    ITIRATE_HASHES {
        table->items[i].name = MALLOC_LABEL;
        *table->items[i].name = '\0';
    }
    table->itemCount = 0;
    table->flag = hashTableFree;
    return table;
}
/*The function Generates a key by summing up the characters in the string*/
int generateKey(char *name) {
    int key;
    key = 0;
    while(*name != '\0') key += *(name++);
    return key;
}

/*
The function is based on a hash function from a previous course. 
(where 'k' is the key and 'm' is the size of the hash table. note that 'm' must be a prime number, and in this case HASHSIZE is 127)
h1(k) = k % m 
h2(k) = 1 + (k % (m-1))
h(k, i) = (h1(k) + i * h2(k)) % m
*/
int generateHash(int key, int i) {
    int hashFunc1, hashFunc2;
    hashFunc1 = key % HASHSIZE;
    hashFunc2 = 1 + (key % (HASHSIZE-1));
    return (hashFunc1 + (i*hashFunc2)) % HASHSIZE;
}

/*
Given a table and a name, the function looks up the name in the table using the hash function.
If found 'index' will be updated to hash that found it.
If not found 'index' will stay 'NOT_FOUND'.
*/
int lookUpTable(SymbolHashTable *table, char *name) {
    int hash, i, key, index;
    index = NOT_FOUND;
    key = generateKey(name);
    ITIRATE_HASHES {
        hash = generateHash(key, i);
        if(!strcmp(table->items[hash].name, name)) {
            index = hash;
            break;
        }
    }
    
    
    return index;
}


/*
Given a table and a name to insert, the function in a similler fashion to he look up function,
searches the table for a free place to put the name. It also updates the item count and the flag if the table is full.
*/
int insertToTable(SymbolHashTable *table, char *name) {
    int hash, i, key, index;
    char *tableName;
    key = generateKey(name);
    index = NOT_FOUND;
    ITIRATE_HASHES {
        hash = generateHash(key, i);
        if(*(tableName = table->items[hash].name) == '\0') {
            table->items[hash].name = strcpy(tableName, name);
            table->itemCount++;
            index = hash;
            break;
        }
    }
    if (table->itemCount == (HASHSIZE)) table->flag = hashTableFull;
    
    return index;
}

/*Frees hashtable items' names. It's the responsibility of other functions to 
free the pointer and its contents.*/
SymbolHashTable *freeTableNames(SymbolHashTable *table) {
    int i;
    char* name;
    ITIRATE_HASHES {
        name = table->items[i].name;
        free(name);
    }
    return table;
}