#include "data.h"
#define MACRO_DEF "mcr"
#define MACRO_END "endmcr"


FILE *preassembler(FILE*,char*);
States lookForMacro(char*, int*);
SymbolHashTable macroTable[HASHSIZE];
/*Needed to be global so the rest of the program can recognize it.*/



/***************TODO LIST****************/
/*
-Complete the main function
-Add function prototypes
-Add Better Documantation
-try to split the main preassembler function to different functions
-Add Error handling when possible
*/
FILE *preassembler1(FILE *fp, char *fileName) {
    char *line, *firstWord, *macroName;
    int indexOfMacro;
    States macroFlag, tableFlag;
    FILE *nfp;
    Macro *macptr, *nextMac;
    nfp = createFileWithSuffix(fileName, 'am');
    macroFlag = readingLine;
    tableFlag = hashTableFree;
    line = malloc(sizeof(char) * MAX_LINE_LENGTH);
    while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) {
        firstWord = readFirstWord(line);
        if (macroFlag != foundMacro) {
            if ((strcmp(MACRO_DEF, firstWord) == False) && (tableFlag != hashTableFull)) {
                macroName = getMacroName(line);
                /*Add to table, also add a check for other preserved names and macros*/
                if((indexOfMacro = lookUpTable(macroTable, macroName)) > -1) {
                    indexOfMacro = insertToTable(macroTable, macroName, &tableFlag);
                    macptr = malloc(sizeof(Macro));
                    macptr->name = macroName;
                    macroTable[indexOfMacro].item = macptr;
                    macroFlag = foundMacro;
                } else {
                    /************Handle Error of macro already in table************/
                }
                
            }
            else if ((indexOfMacro = lookUpTable(macroTable, firstWord)) > -1) {
                /*Insert lines from the table to the file*/
                
            } else {
                /*insert line anyway*/
            }
            
        } else {
            /*
            Insert lines to table until encounterd endmcr
            handle case of Macro definition inside macro
            */
            if (strcmp(firstWord, MACRO_END) != False) {
                nextMac = malloc(sizeof(Macro));
                macptr->line = line;
                macptr->nextLine = nextMac;
                macptr = nextMac;
            } else {
                macroFlag = macroDefinitionEnded;
                free(nextMac);
            }
            
        }
        free(firstWord);
        
        
    }
    free(line);
}


char *getMacroName(char *line) {
    char *name;
    int size;
    line = strstr(line, MACRO_DEF) + 4;
    name = readFirstWord(line);
    return name;
}

char *readFirstWord(char *line) {
    int i, size;
    char *word;
    word;
    i = 0;
    while(isalpha(*line) == 0 && *line != '\0') {
        line++;
        i++;
    }
    for(size = 0; isgraph(line[size]) != 0; size++);
    word = malloc(sizeof(char) * (size+1));
    word = strncpy(word, line, size);
    word[size] = '\0';
    /* If it found atleast one letter, it will find its size than will copy it to the pointer */
    
    return word;
}


/*************Returns a hash table's items index, is -1 if not found.************/

int lookUpTable(SymbolHashTable *table, char *name) {
    int hash, i, key, index;
    index = NOT_FOUND;
    key = generateKey(name);
    for(i = 0; i < HASHSIZE; i++) {
        hash = generateHash(key, i);
        if(strcmp(table[hash].name, name) == False) {
            index = hash;
            break;
        }
    }
    return index;
}

int generateKey(char *name) {
    int key, i, size;
    key = 0;
    size = strlen(name);
    for (i = 0; i < size; i++) key += name[i];
    return key;
}


int generateHash(int key, int i) {
    int hashFunc1, hashFunc2;
    hashFunc1 = key % HASHSIZE;
    hashFunc2 = 1 + (key % (HASHSIZE-1));
    return (hashFunc1 + (i*hashFunc2)) % HASHSIZE;
}
/*For inserting an item into the table for the first time. returns the index of it.*/
int insertToTable(SymbolHashTable *table, char *name, States *tableFlag) {
    int hash, i, key, index;
    key = generateKey(name);
    index = NOT_FOUND;
    for(i = 0; i < HASHSIZE; i++) {
        hash = generateHash(key, i);
        if(table[hash].name == NULL) {
            index = hash;
            break;
        }
    }
    if (i == (HASHSIZE-1)) *tableFlag = hashTableFull;
    return index;
}