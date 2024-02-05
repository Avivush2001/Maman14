#include "data.h"

/*Initialize a hashtable by setting the itemCount to 0 and the flag to hashTableFree*/
SymbolHashTable *initializeHashTable(SymbolHashTable *table) {
    table->itemCount = 0;
    table->flag = hashTableFree;
}
/*The function Generates a key by summing up the characters in the string*/
int generateKey(char *name) {
    int key, i, size;
    key = 0;
    size = strlen(name);
    for (i = 0; i < size; i++) key += name[i];
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

    for(i = 0; i < HASHSIZE; i++) {
        hash = generateHash(key, i);
        if(strcmp(table->items[hash].name, name) == False) {
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
    key = generateKey(name);
    index = NOT_FOUND;
    for(i = 0; i < HASHSIZE; i++) {
        hash = generateHash(key, i);
        if(table->items[hash].name == NULL) {
            table->items[hash].name = name;
            table->itemCount++;
            index = hash;
            break;
        }
    }
    if (table->itemCount == (HASHSIZE)) table->flag = hashTableFull;
    return index;
}