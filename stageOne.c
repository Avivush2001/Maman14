#include "data.h"
extern HashTable macroHashTable;
extern HashTable symbolHashTable;
extern int IC, DC;
extern char * registersArr[], * instructionArr[];

StageOneFlags stageOne(FILE *fp, char *fileName) {
    /*str1 is longer by one to make sure the ':' are in the character*/
    char line[MAX_LINE_LENGTH];
    int lineCounter = 1, stringsCounter, possibleOpCode;
    Symbol *symb;
    StageOneFlags contextFlag, errorFlagSO; 
    DEFAULT_CONTEXT_SO
    errorFlagSO = allclearSO;
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL && errorFlagSO != errorEncounteredSO) {
        possibleOpCode = NOT_FOUND;
        contextFlag = lineContextSO(line, contextFlag, &possibleOpCode);
        printf("line %d, flag: %d, opcode: %d\n", lineCounter, contextFlag, possibleOpCode);
        lineCounter++;
    }
    printSymbols();
    freeSymbols();
}


StageOneFlags lineContextSO(char *line, StageOneFlags oldContext, int *possibleOpCode) {
    int i, stringsCounter;
    Symbol *symb = NULL;
    HashTableItem *symbolItem;
    StageOneFlags contextFlag = readingLineSO;
    char str1[MAX_LABEL_SIZE+1], str2[MAX_LABEL_SIZE], str3[MAX_LABEL_SIZE];
    stringsCounter = sscanf(line, "%32s %31s, %31s", str1, str2, str3);

    /*Check for empty line*/
    if (!stringsCounter) contextFlag = skipLine;

    /*Check for comment line*/
    else if (*str1 == ';') contextFlag = skipLine;
    else{
        /*Checking if the first string can be a legal label*/
        if (isLabelDefinition(str1)) {
            i = lookUpTable(&symbolHashTable, str1);

            /*Initializing entry label. If it wasn't an entry label isLabelDefinition would return false*/
            if (i != NOT_FOUND) {
                symbolItem = symbolHashTable.items + i;
                symb = symbolItem->item;
            } else {
                /*Inserting a new label*/
                i = insertToTable(&symbolHashTable, str1);
                if (i != NOT_FOUND) {
                    symbolItem = symbolHashTable.items + i;
                    EXIT_IF((symb = MALLOC_SYMBOL) == NULL)
                    symbolItem->item = symb;
                    symb->symbol = symbolHashTable.items[i].name;
                    symb->entry = False;
                    symb->attr = undefined;
                } else contextFlag = errorSymbolHashTableFull;
            }
        }
        
        /*Make sure it is in the correct state*/
        if (contextFlag == readingLineSO) {
            /*If a symbol has been defined*/
            if (symb != NULL) {
                /*Check the second string for instructions*/
                if((i = findInStringArray(str2, instructionArr, INSTRUCTIONS_SIZE)) != NOT_FOUND) {
                    switch(i) {
                        case 0:
                            contextFlag = warningLabelInDumbPlace;
                            break;
                        case 1:
                            contextFlag = warningLabelInDumbPlace;
                            break;
                        case 2:
                            contextFlag = warningLabelInDumbPlace;
                            break;
                        case 3:
                            contextFlag = isData;
                            break;
                        case 4:
                            contextFlag = isString;
                            break;
                        default:
                            contextFlag = errorIllegalKeyWord;
                            break;
                    }
                    symb->attr = data;
                    symb->value = DC;
                } else if((i = findInOperationsArray(str2))!= NOT_FOUND) {
                    /*Check the second string for operations*/
                    contextFlag = isOperation;
                    *possibleOpCode = i;
                    symb->attr = code;
                    symb->value = IC+100;
                } else contextFlag = errorIllegalKeyWord;
                
                /*Handle a case of a label before .extern .entry .define*/
                if (contextFlag == warningLabelInDumbPlace) {
                    fprintf(stderr, "WARNING Label: %s is in an illegal location and will be considered undefined.\n", symb->symbol);
                    free(symb->symbol);
                    free(symb);
                    symbolItem->name = NULL;
                    symbolItem->item = NULL;
                    strcpy(str1, str2);
                    strcpy(str2, str3);
                    CHECK_INSTRUCTIONS
                }
            } else {
                /*a symbol hasn't been defined*/

                /*Checking if the first string is an instruction*/
                if ((i = findInStringArray(str1, instructionArr, INSTRUCTIONS_SIZE)) != NOT_FOUND) {
                    CHECK_INSTRUCTIONS
                } else if((i = findInOperationsArray(str1)) != NOT_FOUND) {
                    /*Checking if the first string is an Operation*/
                    contextFlag = isOperation;
                    *possibleOpCode = i;
                } else contextFlag = errorIllegalKeyWord;
            }
        }
    }

    return contextFlag;
}

Bool isLegalSymbol(char *possibleSymbol, Bool isConst) {
    Bool flag= isLabelLegal(possibleSymbol);
    Symbol *symb;
    int i;
    if(flag) {
        i = lookUpTable(&symbolHashTable, possibleSymbol);
        /*If a Label is entry the pointer to 'item' in the table will be NULL*/
        if (i != NOT_FOUND && !isConst) {
            symb = symbolHashTable.items[i].item;
            flag = symb->entry;
        } else flag = (i == NOT_FOUND);
        i = lookUpTable(&macroHashTable, possibleSymbol);
        if (flag) flag = (i == NOT_FOUND);
    }
    return flag;
}

Bool isLabelDefinition(char* possibleLabel) {
    Bool flag;
    char *p;
    p = strrchr(possibleLabel, ':');
    flag = (p != NULL);
    if (flag) {
        if (*(p+1) == '\0') {
            *p = '\0';
            flag = isLegalSymbol(possibleLabel, False);
        } else flag = False;
    }
    return flag;
}

void freeSymbols() {
    int indexOfSymbol;
    Symbol *symb;
    for (indexOfSymbol = 0; indexOfSymbol < HASHSIZE; indexOfSymbol++) {
        if ((symb = symbolHashTable.items[indexOfSymbol].item) != NULL) {
            free(symb);
            symbolHashTable.items[indexOfSymbol].item = NULL;
        }
    }
}

/*
DEBUGGING FUNCTION
*/
void printSymbols() {
    int indexOfSymbol;
    Symbol *symb;
    for (indexOfSymbol = 0; indexOfSymbol < HASHSIZE; indexOfSymbol++) {
        if ((symb = symbolHashTable.items[indexOfSymbol].item) != NULL) {
            printf("name: %s, value: %d, entry: %d, attr: %d\n", symb->symbol,symb->value,symb->entry,symb->attr );
        }
    }
}