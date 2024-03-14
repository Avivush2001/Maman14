#include "data.h"

extern HashTable macroHashTable;
extern HashTable symbolHashTable;
extern int IC, DC;
extern char * registersArr[], * instructionArr[];
extern Operation operationsArr[];

StageOneFlags stageOne(FILE *fp, char *fileName) {
    /*str1 is longer by one to make sure the ':' are in the character*/
    char line[MAX_LINE_LENGTH];
    int lineCounter = 1, possibleOpCode;
    Symbol *symb;
    StageOneFlags contextFlag, errorFlagSO; 
    
    DEFAULT_CONTEXT_SO
    errorFlagSO = allclearSO;
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL && errorFlagSO != errorEncounteredSO) {
        Field field1 = {immediate,NULL,0 }, field2 = {immediate,NULL,0 };
        possibleOpCode = NOT_FOUND;
        contextFlag = lineContextSO(line, &possibleOpCode);
        printf("line %d, flag: %d, opcode: %d\n", lineCounter, contextFlag, possibleOpCode);
        switch(contextFlag) {
            case entryDefinition:
                contextFlag = defineExternOrEntryLabels(line,True);
                printf("%d\n", contextFlag);
                break;
            case externDefinition:
                contextFlag = defineExternOrEntryLabels(line,False);
                printf("%d\n", contextFlag);
                break;
            case constantDefinition:
                contextFlag = defineConstant(line);
                printf("%d\n", contextFlag);
                break;
            case isOperation:
                areLegalOperands(strstr(line, operationsArr[possibleOpCode].name)+3, &field1, &field2);
                break;
            default:
                break;
        }
        lineCounter++;
    }
    printSymbols();
    freeSymbols();
}

/*Return flags besides errors:
skipLine
isData
isString
isOperation
entryDefinition
externDefinition
constantDefinition

The function is a bit of black magic to be honest working so well after a first try.
I tried to touch it and change some of the 'if' statement to a switch-case.
It didn't work well as while the function outputted the correct flags
and initialized the Labels, but for some reason it gave them some garbage values
and flag, so now I am afraid to touch it.

Yet I will try to document it as best to my abilities.

The function is a bit like the pre assembler's line context function, in  that
that it uses the first two strings in the line to return a flag on how
the first stage function should continue. (there is also a 3rd string, incase
there is a label before an instruction where it shouldn't be)

It gets the a pointer to the line and to an integer to possibly store 
the opcode of the operation in the line.

It 
*/
StageOneFlags lineContextSO(char *line, int *possibleOpCode) {
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
                    symbolItem->name = "\0";
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


/*When checking for labels in operands don't use this function use the one
in util.c*/
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

/*Should be called only if the "externDefinition" flag is on*/

StageOneFlags defineExternOrEntryLabels(char *line, Bool isEntry) {
    char *p, *token, *delimiter = " \n";
    StageOneFlags flag;
    Symbol *symb;
    int i;
    if (isEntry)
        p = strchr(line, '.') + 6;
    else p = strchr(line, '.') + 7;
    token = strtok(p, delimiter);
    while(token != NULL) {
        if(isLegalSymbol(token, False) && (i = insertToTable(&symbolHashTable, token)) != NOT_FOUND) {
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
        } else {
            printf("%s\n", token);
            flag = errorIllegalSymbolOrTableFull;
            break;
        }
        token = strtok(NULL, delimiter);
    }
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
    if (*str3 == '\0' || *str2 == '=' || *garbage != '\0') {
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

OperandsFlags areLegalOperands(char *str, Field *field1, Field *field2)
{
    
    OperandsFlags flag = legalOperands;
    char *token;
    const char *delimiter = " , \n";
    int i, operandCounter = 0;
    token = strtok(str, delimiter);
    while(token != NULL && operandCounter < 2 && flag == legalOperands)
    {
        flag = getOperandType(token);
        printf("%s %d\n", token, flag);
        switch(flag) 
        {
            case isConstant:
            {
                wholeNum num = string_to_int(++token);
                if(num.isNum)
                {
                    operandCounter++;
                    if(operandCounter == 1)
                    {
                        field1->symbol = NULL;
                        field1->type = immediate;
                        field1->value = num.result;
                    }
                    if(operandCounter == 2)
                    {
                        field2->symbol = NULL;
                        field2->type = immediate;
                        field2->value = num.result;
                    }
                }
                else if((i = lookUpTable(&symbolHashTable, token)) != NOT_FOUND) 
                {
                    Symbol *symb = symbolHashTable.items[i].item;
                    if (symb->attr == constant) 
                    {
                        operandCounter++;
                        if(operandCounter == 1)
                        {
                            field1->symbol = NULL;
                            field1->type = immediate;
                            field1->value = symb->value;
                        }
                        if(operandCounter == 2)
                        {
                            field2->symbol = NULL;
                            field2->type = immediate;
                            field2->value = symb->value;
                        }
                    } 
                    else 
                        flag = illegalConstantOperand;
                }
                else
                    flag = illegalConstantOperand;
                break;
            }
                
            case isLabel:
            { /*DON'T LOOK IN THE TABLE just check if it is legal*/
                operandCounter++;
                if((i = lookUpTable(&symbolHashTable, token)) == NOT_FOUND)
                {
                    insertToTable(&symbolHashTable, token);
                    if(operandCounter == 1)
                    {
                        field1->symbol = token;
                        field1->type = direct;
                        field1->value = 0;
                    }
                    if(operandCounter == 2)
                    {
                        field2->symbol = token;
                        field2->type = direct;
                        field2->value = 0;
                    }
                }
                else
                {
                    Symbol *symb = symbolHashTable.items[i].item;
                    if(operandCounter == 1)
                    {
                        field1->symbol = token;
                        field1->type = direct;
                        field1->value = symb->value;
                    }
                    if(operandCounter == 2)
                    {
                        field2->symbol = token;
                        field2->type = direct;
                        field2->value = symb->value;
                    }
                }
                break;
            }
            case isArray:
            {
                char *left = NULL, *right = NULL, label[MAX_LABEL_SIZE], brc[MAX_LABEL_SIZE];
                int j, size = strlen(token);
                wholeNum num;
                left = strchr(str, '[');
                right = strchr(str, ']');
                j = size - strlen(left);
                strncpy(label, token, j);
                label[j] = '\0';
                left++;
                size = strlen(left);
                j = size - strlen(right);
                strncpy(brc, left, j);
                brc[j] = '\0';
                num = string_to_int(brc);
                i = lookUpTable(&symbolHashTable, label);
                if(num.isNum == True)
                    {
                        if(i == NOT_FOUND)
                            insertToTable(&symbolHashTable, label);

                        operandCounter++;
                        if(operandCounter == 1)
                        {
                            field1->symbol = label;
                            field1->type = index;
                            field1->value = num.result;
                        }
                        if(operandCounter == 2)
                        {
                            field2->symbol = label;
                            field2->type = index;
                            field2->value = num.result;
                        }  
                    }
                else
                    {
                        if((j = lookUpTable(&symbolHashTable, brc)) != NOT_FOUND)
                        {
                            Symbol *symb = symbolHashTable.items[j].item;
                            if(symb->attr == constant)
                            {
                                if(i == NOT_FOUND)
                                    insertToTable(&symbolHashTable, label);

                                operandCounter++;
                                if(operandCounter == 1)
                                {
                                    field1->symbol = label;
                                    field1->type = index;
                                    field1->value = symb->value;
                                }
                                if(operandCounter == 2)
                                {
                                    field2->symbol = label;
                                    field2->type = index;
                                    field2->value = symb->value;
                                }  
                            }
                            else
                                flag = illegalConstantOperand;
                        }
                        else
                            flag = illegalConstantOperand;
                    }
                break;
            }
            case isRegister:
            {
                int r = findInStringArray(token, registersArr, 8);
                operandCounter++;
                if(operandCounter == 1)
                {
                    field1->symbol = NULL;
                    field1->type = reg;
                    field1->value = r;
                }
                if(operandCounter == 2)
                {
                    field2->symbol = NULL;
                    field2->type = reg;
                    field2->value = r;
                }  
                break;
            }

            default:
                flag = illegalOperand;
                break;    
        }
        token = strtok(NULL, delimiter);
    }
    printf("field1: %p %d %d\n",field1->symbol, field1->type, field1->value);
    printf("field2: %p %d %d\n",field2->symbol, field2->type, field2->value);
    if (token != NULL) flag = tooManyOperands;
    return flag;
}

OperandsFlags getOperandType(char *token) 
{
    OperandsFlags flag = illegalOperand;
    if(token[0] == '#') 
        flag = isConstant;

    else if(isLabelLegal(token) == True)
        flag = isLabel;

    else if(isLegalArray(token) == True)
        flag = isArray;

    else if(findInStringArray(token, registersArr, 8) != -1)
        flag = isRegister;

    return flag;
}
