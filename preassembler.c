#include "data.h"
#define MACRO_DEF "mcr"
#define MACRO_END "endmcr"


FILE *preassembler(FILE*);
States lookForMacro(char*, int*);
Symbol macroTable[HASHSIZE];/*Needed to be global so the rest of the program can recognize it.*/



FILE *preassembler(FILE *fp) {
    char *line, *macroName, *firstWord;
    int indexOfMacro;
    States state;
    /*remember to create a fileeeeeeeeeeee*/
    state = waitingForFile;
    indexOfMacro = NOT_FOUND;
    line = malloc(sizeof(char) * MAX_LINE_LENGTH);
    while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) {
        state = lookForMacro(line, &indexOfMacro);
        switch (state) {
            case emptyLine: break;
            case foundMacroDefinition: {
                /*
                Search if added already. If not add it and start add it
                and change the flag to savingMacro.
                */
                macroName = getMacroName(line); /*REMEMBER TO HANDLE
                CASE OF NAME NOT FOUND*/
                indexOfMacro = lookUpTable(macroTable, macroName);
                if (indexOfMacro == -1) {
                    state = savingMacro;
                    while (
                    ((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) && 
                    (state == savingMacro)) {
                        firstWord = readFirstWord(line);
                        if (strcmp(firstWord,MACRO_END) != False) {
                            /*Add the line to the table*/
                        } else state =  macroDefinitionEnded;
                        free(firstWord);
                    }
                }
                free(macroName);
                /*Maybe put this into a function*/
                break;
            }
            case foundMacro: {
                /*
                Add the macro lines to the file.
                */
               break;
            }
            default: {
                /*
                Add the line.
                */
            }
        }
        free(line);
    }
}
States lookForMacro(char *line, int *index) {
    States flag;
    char *firstWord;
    flag = noMacroFound;
    firstWord = readFirstWord(line);
    if (strcmp(firstWord,MACRO_DEF) == False) flag = foundMacroDefinition;
    if ((*index = lookUpTable(macroTable, firstWord)) != NOT_FOUND) flag = foundMacro;
    if (*firstWord == '\0') flag = emptyLine;
    free(firstWord);
    return flag;
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
        };
    }
    return hash;
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