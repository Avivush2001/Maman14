#include "data.h"


extern HashTable symbolHashTable;
extern int IC, DC;
extern Operation operationsArr[];

StageOneFlags stageOne(FILE *fp, char *fileName) {
    /*str1 is longer by one to make sure the ':' are in the character*/
    char line[MAX_LINE_LENGTH];
    int lineCounter = 1, possibleOpCode;
    Symbol *symb;
    StageOneFlags contextFlag, errorFlagSO; 
    OperandsFlags opFlag;
    MemoryFlags memFlag;
    initializeMemory();
    DEFAULT_CONTEXT_SO
    errorFlagSO = allclearSO;
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL && errorFlagSO != errorEncounteredSO) {
        Field field1 = {immediate,NULL,0 }, field2 = {immediate,NULL,0 };
        possibleOpCode = NOT_FOUND;
        opFlag = notReadOperands;
        memFlag = memoryAvailable;
        contextFlag = lineContextSO(line, &possibleOpCode);
        switch(contextFlag) {
            case entryDefinition:
                contextFlag = defineExternOrEntryLabel(line,True);
                break;
            case externDefinition:
                contextFlag = defineExternOrEntryLabel(line,False);
                break;
            case constantDefinition:
                contextFlag = defineConstant(line);
                break;
            case isOperation:
                opFlag = areLegalOperands(strstr(line, operationsArr[possibleOpCode].name)+3, &field1, &field2);
                switch (opFlag) {
                    case noOperands:
                        if (operationsArr[possibleOpCode].fields == 0) {
                            memFlag = insertOperation(possibleOpCode, NULL, NULL);
                        } else {
                            contextFlag = errorOperandTypes;
                        }
                        break;
                    case legal1Operand:
                        if (operationsArr[possibleOpCode].fields == 1 && operationsArr[possibleOpCode].allowedDst[field1.type]) {
                            memFlag = insertOperation(possibleOpCode, NULL, &field1);
                        } else {
                            contextFlag = errorOperandTypes;
                        }
                        break;
                    case legal2Operands:
                        if (operationsArr[possibleOpCode].fields == 2 && operationsArr[possibleOpCode].allowedSrc[field1.type] && operationsArr[possibleOpCode].allowedDst[field2.type]) {
                            memFlag = insertOperation(possibleOpCode, &field1, &field2);
                        } else {
                            contextFlag = errorOperandTypes;
                        }
                        break;
                    default:
                        break;
                    
                }
                break;
            case isData:
                contextFlag = insertData(line); 
                break;
            case isString: 
                contextFlag = insertStringToMemory(line);
                break;
            default:
                break;
        }
        /*TODO write an error handling function for operands and use here*/
        /*It should get all the flags in this function*/
        lineCounter++;
    }
    
    updateDataLabels();
    printSymbols();
    addDataToMemory();
    freeSymbols();
    freeMemory();
}

/*
The function is a bit of black magic to be honest working so well after the first try.
I tried to touch it and combine some of the 'if' statement to a switch-case.
It didn't work well as while the function outputted the correct flags
and initialized the Labels, but for some reason it gave them some garbage values
and flag, so now I am afraid to touch it. Yet I will try to document it as best to my abilities.

The function is a bit like the pre assembler's line context function, in  that
that it uses the first two strings in the line to return a flag on how
the first stage function should continue. (there is also a 3rd string, incase
there is a label before an instruction where it shouldn't be)

After first checking for a comment line or an empty line it checks if the first string in the line is a label definition.
If it is, it is already added and we look it up in the table, create a symbol for it, and insert it to the table.
Then we check for errors and if we entered a symbol.
If we did, check the second string for instructions or an operation and change the flag accordingly.
If no symbol entered, we do basically the same but for the second String
*/
StageOneFlags lineContextSO(char *line, int *possibleOpCode) {
    int i, stringsCounter;
    Symbol *symb = NULL;
    HashTableItem *symbolItem;
    StageOneFlags contextFlag = readingLineSO;
    char str1[MAX_LABEL_SIZE+1], str2[MAX_LABEL_SIZE], str3[MAX_LABEL_SIZE];
    stringsCounter = sscanf(line, "%32s %31s, %31s", str1, str2, str3);

    
    if (!stringsCounter) contextFlag = skipLine;/*Check for empty line*/
    else if (*str1 == ';') contextFlag = skipLine;/*Check for comment line*/
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
                if((i = findInstruction(str2)) != NOT_FOUND) {
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
                } else if((i = findOperation(str2))!= NOT_FOUND) {
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
                    symbolItem->name = "\0";
                    symbolItem->item = NULL;
                    strcpy(str1, str2);
                    strcpy(str2, str3);
                    CHECK_INSTRUCTIONS
                }
            } else {
                /*a symbol hasn't been defined*/

                /*Checking if the first string is an instruction*/
                if ((i = findInstruction(str1)) != NOT_FOUND) {
                    CHECK_INSTRUCTIONS
                } else if((i = findOperation(str1)) != NOT_FOUND) {
                    /*Checking if the first string is an Operation*/
                    contextFlag = isOperation;
                    *possibleOpCode = i;
                } else contextFlag = errorIllegalKeyWord;
            }
        }
    }

    return contextFlag;
}


StageOneFlags defineExternOrEntryLabel(char *line, Bool isEntry) 
{
    char *p, label[MAX_LABEL_SIZE], garbage[MAX_LABEL_SIZE];
    StageOneFlags flag = allclearSO;
    Symbol *symb;
    int i;
    *garbage = '\0';
    *label = '\0';
    if (isEntry)
        p = strchr(line, '.') + 6;
    else p = strchr(line, '.') + 7;

    sscanf(p, "%s %s", label, garbage);
    printf("%s\n", label);
    printf("%s\n", garbage);
    if (*garbage != '\0' || *label == '\0') flag = errorDefiningEntryOrExtern;
    if(flag == allclearSO && isLegalSymbol(label, True) && (i = insertToTable(&symbolHashTable, label)) != NOT_FOUND) {
        symb = MALLOC_SYMBOL;
        EXIT_IF(symb == NULL)
        symb->symbol = symbolHashTable.items[i].name;
        if (isEntry) {
            symb->attr = undefined;
        } else symb->attr = external;
        symb->entry = isEntry;
        symb->value = 0;
            symbolHashTable.items[i].item = symb;
        flag = allclearSO;
    } else flag = errorDefiningEntryOrExtern;
    return flag;
}

StageOneFlags defineConstant(char *line) {
    char *p = strchr(line, '.') + 7, str1[MAX_LABEL_SIZE], str2[MAX_LABEL_SIZE], str3[MAX_LABEL_SIZE] ,garbage[2];
    StageOneFlags flag;
    int i;
    Symbol *symb;
    wholeNum value = {False, 0};
    *str1 = *str2 = *str3 = *garbage ='\0';
    sscanf(p, "%31s %31s %31s %1s", str1, str2, str3, garbage);
    if (*str3 == '\0' || strcmp(str2,"=") || *garbage != '\0') {
        flag = errorDefiningConstant;
    } else {
        value = string_to_int(str3);
        if (isLegalSymbol(str1, True)  && (i = insertToTable(&symbolHashTable, str1)) != NOT_FOUND && value.isNum && IN_CONST_RANGE(value.result)) {
            symb = MALLOC_SYMBOL;
            EXIT_IF(symb == NULL)
            symb->symbol = symbolHashTable.items[i].name;
            symb->attr = constant;
            symb->value = value.result;
            symb->entry = False;
            symbolHashTable.items[i].item = symb;
            flag = allclearSO;
        } else {
            flag = errorIllegalSymbolOrTableFull;
        }
    }
    return flag;
}

StageOneFlags insertStringToMemory(const char *str)
{
    StageOneFlags flag = legalString;
    MemoryFlags memFlag;
    Data data = {0};
    char *left, *right, *p = strchr(str,'.') + 7;
    if(str == NULL || *str == '\0')
        flag = illegalString;
    else
    {
        left = strchr(str, '"');
        right = strrchr(str, '"');
        if(left == NULL || right == NULL || left == right) /* Not enough " (needs at least 2 different ones to contain the string) */
            flag = illegalString;
        else
        {
            while(p < left && flag == legalString)
            {
                if(isgraph(*p) != 0)
                    flag = illegalString; /* left " has to be the first char of the string */
                p++;
            }
            p = right + 1;
            while(*p != '\0' && flag == legalString)
            {
                if(isgraph(*p) != 0)
                    flag = illegalString; /* right " has to be the last char of the string */
                p++;
            }
            left++; /* Now left is the first char of the string */
            while(left < right && flag == legalString)
            {
                if(isgraph(*left) != 0)
                {
                    data.value = (int) (*left);
                    memFlag = insertDataWord(&data);
                    left++;
                    if(memFlag != wordCreationSuccess)
                        flag = errorMemoryFull;
                }
                else
                    flag = illegalString;
            }
            if(flag == legalString) /* If we reached the end of the string and it's legal up to this point add null terminator in the end */
            {
                data.value = 0;
                memFlag = insertDataWord(&data);
                if(memFlag != wordCreationSuccess)
                    flag = errorMemoryFull;
            }
        }
        
    }
    return flag;  
}

StageOneFlags insertData(char *line) {
    const char *delimiter = " , \n";
    char *p = strchr(line,'.') + 5, *token;
    Bool flagNoComma = False;
    StageOneFlags flag = allclearSO;
    MemoryFlags memFlag;
    int i = 0, j;
    if (strchr(line, ',') == NULL) {
        flagNoComma = True;
    } /*else if(!isgraph(*(strchr(line, ',')-1))) {
        flag = errorEnteringData;
    }*/
    for(j = 0; j < strlen(p); j++) {
        if (isgraph(*(p+j)) && *(p+j) != ',')
            break;
        if (*(p+j) == ',') {
            flag = errorEnteringData;
            break;
        }

    }
    token = strtok(p, delimiter);
    while (token != NULL && flag != errorEnteringData) {
        if ((i++) == 1 && flagNoComma) 
            flag = errorEnteringData;
        else {
            wholeNum number;
            Data data = {0};
            number = string_to_int(token);
            if (number.isNum && IN_DATA_RANGE(number.result)) {
                data.value = number.result;
                memFlag = insertDataWord(&data);
            } else if((j = lookUpTable(&symbolHashTable, token)) != NOT_FOUND) {
                Symbol *symb = symbolHashTable.items[j].item;
                    if(symb->attr == constant) {
                        data.value = symb->value;
                        memFlag = insertDataWord(&data);
                    } else flag = errorEnteringData;
            } else
                flag = errorEnteringData;
            
            if (memFlag == memoryFull) {
                flag = errorMemoryFull;
                break;
            }
        }
        token = strtok(NULL, delimiter);
    }
    return flag;
}

