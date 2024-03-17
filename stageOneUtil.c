#include "data.h"

extern HashTable macroHashTable;
extern HashTable symbolHashTable;
extern int IC, DC;
extern char * registersArr[], * instructionArr[];
extern Operation operationsArr[];

void updateDataLabels() {
    int i;
    Symbol *symb;
    for (i = 0; i < HASHSIZE; i++) {
        symb = symbolHashTable.items[i].item;
        if (symb != NULL && symb->attr == data) 
            symb->value += IC+100;
    }
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

OperandsFlags getOperandType(char *token) 
{
    OperandsFlags flag = illegalOperand;
    if(token[0] == '#') 
        flag = isConstant;

    else if(isLabelLegal(token) == True)
        flag = isLabel;

    else if(isLegalArray(token) == True)
        flag = isArray;

    else if(findRegister(token) != -1)
        flag = isRegister;

    return flag;
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

OperandsFlags areLegalOperands(char *str, Field *field1, Field *field2)

{
    
    OperandsFlags flag = legal1Operand;
    char *token;
    const char *delimiter = " , \n";
    int i, operandCounter = 0;
    token = strtok(str, delimiter);

    if(token == NULL)
        flag = noOperands;
        
    while(token != NULL && operandCounter < 2 && flag == legal1Operand)
    {
        flag = getOperandType(token);
        switch(flag) 
        {
            case isConstant:
            {
                wholeNum num = string_to_int(++token);
                if(num.isNum && IN_CONST_RANGE(num.result))
                {
                    operandCounter++;
                    if(operandCounter == 1)
                    {
                        field1->symbol = NULL;
                        field1->type = immediate;
                        field1->value = num.result;
                        flag = legal1Operand;
                    }
                    if(operandCounter == 2)
                    {
                        field2->symbol = NULL;
                        field2->type = immediate;
                        field2->value = num.result;
                        flag = legal2Operands;
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
                            flag = legal1Operand;
                        }
                        if(operandCounter == 2)
                        {
                            field2->symbol = NULL;
                            field2->type = immediate;
                            field2->value = symb->value;
                            flag = legal2Operands;
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
            { 
                char *p = (char *) (calloc(strlen(token), sizeof(char)));
                if(p != NULL)
                {
                    strcpy(p, token);
                    operandCounter++;
                    if(operandCounter == 1)
                    {
                        field1->symbol = p;
                        field1->type = direct;
                        field1->value = 0;
                        flag = legal1Operand;
                    }
                    if(operandCounter == 2)
                    {
                        field2->symbol = p;
                        field2->type = direct;
                        field2->value = 0;
                        flag = legal2Operands;
                    }
                }
                else
                {
                    flag = failedCalloc;
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case isArray:
            {
                char *left = NULL, *right = NULL, label[MAX_LABEL_SIZE], brc[MAX_LABEL_SIZE], *p;
                int i, size = strlen(token);
                wholeNum num;
                left = strchr(token, '[');
                right = strchr(token, ']');
                i = size - strlen(left);
                strncpy(label, token, i);
                label[i] = '\0';
                p = (char *) (calloc(strlen(label), sizeof(char)));
                if(p != NULL)
                {
                    strcpy(p,label);
                    left++;
                    size = strlen(left);
                    i = size - strlen(right);
                    strncpy(brc, left, i);
                    brc[i] = '\0';
                    num = string_to_int(brc);
                    if(num.isNum && IN_CONST_RANGE(num.result))
                    {
                        operandCounter++;
                        if(operandCounter == 1)
                        {
                            field1->symbol = p;
                            field1->type = index;
                            field1->value = num.result;
                            flag = legal1Operand;
                        }
                        if(operandCounter == 2)
                        {
                            field2->symbol = p;
                            field2->type = index;
                            field2->value = num.result;
                            flag = legal2Operands;
                        }  
                    }
                    else if((i = lookUpTable(&symbolHashTable, brc)) != NOT_FOUND)
                    {
                        Symbol *symb = symbolHashTable.items[i].item;
                        if(symb->attr == constant)
                        {
                            operandCounter++;
                            if(operandCounter == 1)
                            {
                                field1->symbol = p;
                                field1->type = index;
                                field1->value = symb->value;
                                flag = legal1Operand;
                            }
                            if(operandCounter == 2)
                            {
                                field2->symbol = p;
                                field2->type = index;
                                field2->value = symb->value;
                                flag = legal2Operands;
                            }  
                        }
                        else
                            flag = illegalConstantOperand;
                    }
                    else
                        flag = illegalConstantOperand;
                }
                else
                {
                    flag = failedCalloc;
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case isRegister:
            {
                int r = findRegister(token);
                operandCounter++;
                if(operandCounter == 1)
                {
                    field1->symbol = NULL;
                    field1->type = reg;
                    field1->value = r;
                    flag = legal1Operand;
                }
                if(operandCounter == 2)
                {
                    field2->symbol = NULL;
                    field2->type = reg;
                    field2->value = r;
                    flag = legal2Operands;
                }  
                break;
            }
            default:
                flag = illegalOperand;
                break;    
        }
        token = strtok(NULL, delimiter);
    }
    if (token != NULL && operandCounter == 2)
        flag = tooManyOperands;
        
    return flag;
}

StageOneFlags errorHandlerSO(StageOneFlags contextFlag, MemoryFlags memFlag, OperandsFlags opFlag, int lineCounter) {
    StageOneFlags flag = allclearSO;
    switch (contextFlag) {
        ERROR_CASE_SO(errorIllegalKeyWord, "Illegal keyword encountered!\n")
        ERROR_CASE_SO(errorIllegalSymbolOrTableFull, "Illegal symbol encountered or the symbol hash table is full!\n")
        ERROR_CASE_SO(errorSymbolHashTableFull, "Symbol is legal but the table is full!\n")
        ERROR_CASE_SO(errorDefiningConstant, "Constant definition failed due to incorrect syntax or illegal posting!\n")
        ERROR_CASE_SO(errorEnteringData, "Data entry failed due to incorrect syntax or illegal posting!\n")
        ERROR_CASE_SO(illegalString, "String entry failed, check it again!\n")
        ERROR_CASE_SO(errorDefiningEntryOrExtern, "Entry or external label definition failed!\n")
        ERROR_CASE_SO(errorOperandTypes, "Illegal operand types or count for the current operation!\n")
        ERROR_CASE_SO(errorMemoryFull, "The memory is full!\n")
        default:
            break;
    }
    switch (opFlag) {
        ERROR_CASE_SO(illegalOperand, "One or two illegal Operands!\n")
        ERROR_CASE_SO(illegalConstantOperand, "An operand expected to be constant, but isn't defined!\n")
        ERROR_CASE_SO(tooManyOperands, "Too many operands than allowed!\n")
        default:
            break;
    }
    switch (memFlag) {
        ERROR_CASE_SO(memoryFull, "The memory is full!\n")
        default:
            break;
    }   
    return flag;
}