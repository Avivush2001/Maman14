#define HASHSIZE 127
#define ITIRATE_HASHES for(i = 0; i < HASHSIZE; i++)
/*Flags to represent if a table is full or free to add to.*/
typedef enum {
    hashTableFree,
    hashTableFull
} HashTableFlags;
/*A HashTableItem has a name and a void pointer, so it can be used to point at either a macro, or a symbol as needed.*/
typedef struct {
    char *name;
    void *item;
} HashTableItem;
/*A struct that holds an array of hash table items, a count of their items, and a flag*/
typedef struct {
    HashTableItem items[HASHSIZE];
    /*THESE TWO FIELDS MUST BE SET UP WHEN BEFORE USING ANY OF THE FUNCTIONS BELOW*/
    unsigned itemCount : 8;
    HashTableFlags flag;
} SymbolHashTable;



/*Initialize a hashtable*/
SymbolHashTable *initializeHashTable(SymbolHashTable*);

/*Generate a key for the hash function.*/
int generateKey(char*);

/*A hash function.*/
int generateHash(int, int);

/*Given a string, looks it up in the table.*/
int lookUpTable(SymbolHashTable*, char*);

/*Given a string it inserts it to the table.*/
int insertToTable(SymbolHashTable*, char*);

/*Frees hashtable items' names. It's the responsibility of other functions to 
free the pointer and its contents.*/
SymbolHashTable *freeTableNames(SymbolHashTable*);