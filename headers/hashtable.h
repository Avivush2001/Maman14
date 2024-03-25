#define HASHSIZE 127
#define ITERATE_HASHES for(i = 0; i < HASHSIZE; i++)
#define TABLE_NAME_AT(_hash) table->items[_hash].name
#define ITEM_AT_INDEX table->items[i].item

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
    HashTableItem *items;
    unsigned itemCount;
    HashTableFlags flag;
} HashTable;



/*Initialize a hashtable*/
void initializeHashTable(HashTable*);

/*Generate a key for the hash function.*/
int generateKey(char*);

/*A hash function.*/
int generateHash(int, int);

/*Given a string, looks it up in the table.*/
int lookUpTable(HashTable*, char*);

/*Given a string it inserts it to the table.*/
int insertToTable(HashTable*, char*);

/*Frees hashtable items' names. It's the responsibility of other functions to 
free the pointer and its contents.*/
void freeTableNames(HashTable*);