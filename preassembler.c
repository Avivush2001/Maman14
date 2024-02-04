#include "data.h"



FILE *preassembler(FILE*,char*);
PreassemblerFlags lookForMacro(char*, int*);
SymbolHashTable macroTable[HASHSIZE];
HashTableFlags macroTableFlag;
/*Needed to be global so the rest of the program can recognize it.*/


/***************TODO LIST****************/
/*
-Complete the main function
-Add function prototypes
-Add Better Documantation
-try to split the main preassembler function to different functions
-Add Error handling when possible
*/

/*
Main preassembler program, based on the algorithm presented in the task instructions.
Gets a file pointer, and returns a file pointer to the new .am file.
*/
// FILE *preassembler(FILE *fp, char *fileName) {
//     /******Initializations*******/
//     char *line, *firstWord, *macroName; 
//     /*Pointers to the current line, the first word in the line, and a pointer to the name of the macro we define.*/
//     int indexOfMacro, lineCounter;
//     /*index of the macro we just inserted / macro name we just got, and a line counter for error handling*/
//     PreassemblerFlags macroFlag; 
//     HashTableFlags tableFlag;
//     FILE *nfp;
//     Macro *macptr, *nextMac;
//     nfp = createFileWithSuffix(fileName, 'am');
//     macroFlag = readingLine;
//     tableFlag = hashTableFree;
//     line = malloc(sizeof(char) * MAX_LINE_LENGTH);
//     while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) {
//         firstWord = readFirstWord(line);
//         if (macroFlag != foundMacro) {
//             if ((strcmp(MACRO_DEF, firstWord) == False) && (tableFlag != hashTableFull)) {
//                 macroName = getMacroName(line);
//                 /*Add to table, also add a check for other preserved names and macros*/
//                 if((indexOfMacro = lookUpTable(macroTable, macroName)) > -1) {
//                     indexOfMacro = insertToTable(macroTable, macroName, &tableFlag);
//                     macptr = malloc(sizeof(Macro));
//                     macptr->name = macroName;
//                     macroTable[indexOfMacro].item = macptr;
//                     macroFlag = foundMacro;
//                 } else {
//                     /************Handle Error of macro already in table************/
//                 }
                
//             }
//             else if ((indexOfMacro = lookUpTable(macroTable, firstWord)) > -1) {
//                 /*Insert lines from the table to the file*/
                
//             } else {
//                 /*insert line anyway*/
//             }
            
//         } else {
//             /*
//             Insert lines to table until encounterd endmcr
//             handle case of Macro definition inside macro
//             */
//             if (strcmp(firstWord, MACRO_END) != False) {
//                 nextMac = malloc(sizeof(Macro));
//                 macptr->line = line;
//                 macptr->nextLine = nextMac;
//                 macptr = nextMac;
//             } else {
//                 macroFlag = macroDefinitionEnded;
//                 free(nextMac);
//             }
            
//         }
//         free(firstWord);
        
        
//     }
//     free(line);
// }
FILE *preassembler(FILE *fp, char *fileName) {
    /******Initializations*******/

    /*
    Pointers to the line, 
    to the first word in the line, 
    to the second word.
    */
    char *line, *firstWord, *secondWord; 
    
    /*
    Index of the macro we are defining / inserting into the file, 
    and a line counter for error handling
    */
    int indexOfMacro, lineCounter;

    /*
    Flags for the context of the current line,
    an error flag for when an error was encountered atleast once
    a flag for the previous flag for error handling
    */
    PreassemblerFlags prevFlag, contextFlag, errorFlag; 
    
    /*File pointer to the new file.*/
    FILE *nfp;

    /*Pointers to the current macro item in the list and to the next one*/
    Macro *macptr, *nextMac;

    /*Create a file with a new suffix.*/
    nfp = createFileWithSuffix(fileName, 'am');
    
    /*Flag setups*/
    contextFlag = readingLine;
    errorFlag = allclearPA;
    macroTableFlag = hashTableFree;
    
    /*Line memory allocation*/
    line = malloc(sizeof(char) * MAX_LINE_LENGTH);
    
    /*Intigers setup*/
    lineCounter = 1;
    indexOfMacro = NOT_FOUND;
    
    /*Main loop*/
    while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) {
        prevFlag = contextFlag;
        firstWord = readFirstWord(line);
        /*get its context*/
        contextFlag = lineContext(line, firstWord, contextFlag, &indexOfMacro);
        switch (contextFlag) {
            case addLine: {
                /*Add line to file*/
                contextFlag = readingLine;
                break;
            }
            case macroCall: {
                /*Add all the lines from the macro into the file using indexOfMacro*/
                contextFlag = readingLine;
                break;
            }
            case macroDefinitionStarted: {
                /*Add the macro to the table*/
                contextFlag = macroDefinitionOngoing;
                break;
            }
            case macroDefinitionOngoing: {
                /*Add the line to the macro*/
                break;
            }
            case macroDefinitionEnded: {
                /*Dont do anything, free relevent pointers*/
                contextFlag = readingLine;
                break;
            }
            case errorPA: {
                /*Print Error*/
                errorFlag = contextFlag;
                break;
            }
        }
        free(firstWord);
        lineCounter++;
    }
    free(line);
}
/*
Get the context of the line. The function checks for each relevent flags
if relevent parameters of the line are present. If they are the newFlag won't be an error
*/
PreassemblerFlags lineContext(char *line, char *firstWord, PreassemblerFlags currentFlag,int *indexOfMacro) {
    PreassemblerFlags newFlag;
    newFlag = errorPA;
    switch (currentFlag) {
        /*Reading a line*/
        case readingLine: {
            /*If firstWord isn't a macro end in an irrelevent place*/
            if(strcmp(firstWord,MACRO_END) != 0) {
                newFlag = checkForMacroCall(line, firstWord, indexOfMacro);
            }
            /*If firstWord is a macro definition*/
            if(strcmp(firstWord,MACRO_DEF) == 0 && macroTableFlag==hashTableFree) {
                newFlag = macroDefinitionStarted;
            }
            break;
        }
        /*A macro definition has been started*/
        default: {
            /*If firstWord isn't a new macro definition*/
            if (strcmp(firstWord,MACRO_DEF) != 0) {
                newFlag = macroDefinitionOngoing;
            }
            /*If the macro definition ended*/
            if (strcmp(firstWord,MACRO_END) == 0) {
                newFlag = macroDefinitionEnded;
            }
            break;
        }

        return newFlag;
    }

}
/*More sophesticated method to check for Macro Call*/
PreassemblerFlags checkForMacroCall(char *line, char *firstWord, int *indexOfMacro) {
    PreassemblerFlags newFlag;
    newFlag = errorPA;     
    if ((*indexOfMacro = lookUpTable(macroTable, firstWord)) == NOT_FOUND) {
        /*If no macro exists yet it is a legal label*/
        if (isLabelLegal(firstWord) != True) {
            newFlag = addLine;
        }
    } else newFlag = macroCall;
    return newFlag;
}
/*check if a label is not a macro*/

char *getMacroName(char *line) {
    char *name;
    int size;
    line = strstr(line, MACRO_DEF) + 4;
    name = readFirstWord(line);
    return name;
}

char *readFirstWord(char *line) {
    int size;
    char *word;
    word;
    while(isalpha(*line) == 0 && *line != '\0') {
        line++;
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
int insertToTable(SymbolHashTable *table, char *name, HashTableFlags *tableFlag) {
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

/*
function for finding a word in a string array.
currently used for preserved words.
*/

int findInStringArray(char* word, char *arr[], int size) {
    int index, i;
    index = NOT_FOUND;
    for (i = 0; i < size; i++) {
        if (strcmp(word, arr[i]) == 0) {
            index = i;
            break;
        }
    }
    return index;
}


/***********************Functions for checking if a label is legal***********************/
/*Label not including ':' at the end*/
Bool isLabelLegal(char *label) {
    Bool ans;
    int i, size;
    size = strlen(label);
    ans = True;
    if (isPreservedWord(label) == True) ans = False;
    else {
        if (!isalpha(label[0])) ans = False;
        for (i = 1; i < size; i++) {
            if (!isalpha(label[i]) && !isdigit(label[i])) {
                ans = False;
                break;
            }
        }
        }
    return ans;
}
Bool isPreservedWord(char *label) {
    Bool ans;
    ans = True;
    if (findInStringArray(label, operationsArr, OPERATIONS_SIZE) != NOT_FOUND) ans = False;
    else if (findInStringArray(label, registersArr, REGISTERS_SIZE) != NOT_FOUND) ans = False;
    else if (findInStringArray(label, instructionArr, INSTRUCTIONS_SIZE) != NOT_FOUND) ans = False;
    return ans;
}