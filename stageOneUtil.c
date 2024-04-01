#include "data.h"

extern HashTable macroHashTable;
extern HashTable symbolHashTable;
extern int IC, DC;
extern char * registersArr[], * instructionArr[];
extern Operation operationsArr[];

/*Function to check for commas between operands*/
static int commaCheck(char *);

/*Update data labels to the correct address*/
void updateDataLabels() {
    int i;
    Symbol *symb;
    for (i = 0; i < HASHSIZE; i++) {
        symb = symbolHashTable.items[i].item;
        if (symb != NULL && symb->attr == data) 
            symb->value += IC+100;
    }
}

/*Frees the symbols in the table*/
void freeSymbols() {
    int i;
    Symbol *symb;
    for (i = 0; i < HASHSIZE; i++) {
        if ((symb = symbolHashTable.items[i].item) != NULL) {
            free(symb);
            symbolHashTable.items[i].item = NULL;
        }
    }
}

/*DEBUGGING FUNCTION*/
void printSymbols() {
    int i;
    Symbol *symb;
    for (i = 0; i < HASHSIZE; i++) {
        if ((symb = symbolHashTable.items[i].item) != NULL) {
            printf("name: %s, value: %d, entry: %d, attr: %d\n", symb->symbol,symb->value,symb->entry,symb->attr );
        }
    }
    
}

/*Checks the operand type.*/
OperandsFlags getOperandType(char *token) 
{
    OperandsFlags flag = illegalOperand;
    if(token[0] == '#') 
        flag = isConstant;

    else if(isValidSymbol(token) == True)
        flag = isLabel;

    else if(isLegalArray(token) == True)
        flag = isArray;

    else if(findRegister(token) != -1)
        flag = isRegister;

    return flag;
}

/*
checks if it can be a valid symbol, and then 
look up in the tables to check if it was defined or not as either
a label, constant, or macro. It gets a boolean 'isConst' so it can
also check entry labels correctly when defining them.
*/
Bool isLegalSymbol(char *possibleSymbol, Bool isConst) {
    Bool flag= isValidSymbol(possibleSymbol);
    Symbol *symb;
    int i;
    if(flag) {
        i = lookUpTable(&symbolHashTable, possibleSymbol);
        if (i != NOT_FOUND && !isConst) {
            symb = symbolHashTable.items[i].item;
            flag = symb->entry;
        } else flag = (i == NOT_FOUND);
        i = lookUpTable(&macroHashTable, possibleSymbol);
        if (flag) flag = (i == NOT_FOUND);
    }
    return flag;
}

/*
Check if a string is a possible label definition.
it removes the ':' and calls to isLegalSymbol to check 
if it wasn't defined already.
*/
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

/*
The function uses strtok to get the individual operands,
parses them and sets up the two fields.
*/
OperandsFlags areLegalOperands(char *str, Field *field1, Field *field2)
{ 
    OperandsFlags flag = noOperands;
    char *token;
    const char *delimiter = " , \n \r";
    int i, operandCounter = 0, expectedOperands = commaCheck(str), value;
    token = strtok(str, delimiter);
    while(token != NULL && operandCounter < 2 && operandCounter != NOT_FOUND)
    {
        flag = getOperandType(token);
        switch(flag) 
        {
            case isConstant:
            {
                /*Check and get the value of the constant*/
                wholeNum num = string_to_int(++token);
                if(num.isNum && IN_CONST_RANGE(num.result))
                    value = num.result;
                else if((i = lookUpTable(&symbolHashTable, token)) != NOT_FOUND) 
                {
                    Symbol *symb = symbolHashTable.items[i].item;
                    if (symb->attr == constant) value = symb->value;
                    else {
                        BREAK_WITH_FLAG(illegalConstantOperand)
                    }
                } else {
                    BREAK_WITH_FLAG(illegalConstantOperand)
                }
                /*Setup the field*/
                operandCounter++;
                if(operandCounter == 1)
                {
                    SETUP_FIELD_CONSTANT(1)
                }
                if(operandCounter == 2)
                {
                    SETUP_FIELD_CONSTANT(2)
                }
                break;
                
            }
                
            case isLabel:
            {
                /*Allocate memory for the label and setup the field*/
                char *label = MALLOC_LABEL;
                EXIT_IF((label == NULL))
                strcpy(label, token);
                operandCounter++;
                if(operandCounter == 1)
                {
                    SETUP_FIELD_LABEL(1)
                }
                if(operandCounter == 2)
                {
                    SETUP_FIELD_LABEL(2)
                }
                
                break;
            }
            case isArray:
            {
                char *left = NULL, *right = NULL, *label, brc[MAX_LABEL_SIZE];
                int i, size = strlen(token);
                wholeNum num;
                Symbol *symb ;

                /*Get positions of brackets*/
                left = strchr(token, '[');
                right = strchr(token, ']');
                i = size - strlen(left);

                /*Get label*/
                label = MALLOC_LABEL;
                EXIT_IF(label == NULL)
                strncpy(label, token, i);
                label[i] = '\0';

                /*Get brackets*/
                left++;
                size = strlen(left);
                i = size - strlen(right);
                strncpy(brc, left, i);
                brc[i] = '\0';

                /*Get the value between the brackets*/
                num = string_to_int(brc);
                if(num.isNum && IN_CONST_RANGE(num.result))
                    value = num.result;
                else if((i = lookUpTable(&symbolHashTable, brc)) != NOT_FOUND) 
                {
                    symb = symbolHashTable.items[i].item;
                    if (symb->attr == constant) value = symb->value;
                    else {
                        BREAK_WITH_FLAG(illegalConstantOperand)
                    }
                } else {
                    BREAK_WITH_FLAG(illegalConstantOperand)
                }

                /*Setup the field*/
                operandCounter++;
                if(operandCounter == 1)
                {
                    SETUP_FIELD_ARRAY(1)
                }
                if(operandCounter == 2)
                {
                    SETUP_FIELD_ARRAY(2)
                }
                break;
            }
            case isRegister:
            {
                /*Get the register and setup the field*/
                value = findRegister(token);
                operandCounter++;
                if(operandCounter == 1)
                {
                    SETUP_FIELD_REG(1)
                }
                if(operandCounter == 2)
                {
                    SETUP_FIELD_REG(2)
                }  
                break;
            }
            default:
                BREAK_WITH_FLAG(illegalOperand)
        }
        token = strtok(NULL, delimiter);
    }
    /*Checks a bunch of stuff, like
    if there are too many operands or commas*/
    switch(operandCounter) {
        case 0:
            if (operandCounter != --expectedOperands)
                flag = errorCommas;
            break;
        case 2:
            if (token != NULL)
                flag = tooManyOperands;
        case 1:
            if (operandCounter != expectedOperands)
                flag = errorCommas;
            break;
        default:
            break;
        }
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
        ERROR_CASE_SO(errorIllegalConstant, "Illegal integer as constant!\n")
        ERROR_CASE_SO(errorDefinedEntryLabel, "Entry label already defined!\n")
        default:
            break;
    }
    switch (opFlag) {
        ERROR_CASE_SO(illegalOperand, "One or two illegal Operands!\n")
        ERROR_CASE_SO(illegalConstantOperand, "An operand expected to be constant, but isn't defined!\n")
        ERROR_CASE_SO(tooManyOperands, "Too many operands than allowed!\n")
        ERROR_CASE_SO(errorCommas, "Check the commas in the line!\n")
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
/*
Function to check for commas between operands
*/
static int commaCheck(char *str) {
    int expectedOperands = 0;
    char *p;
    if ((p = strchr(str, ',')) == NULL)
        expectedOperands = 1;
    else if(strrchr(str, ',') == p)
        expectedOperands = 2;
    return expectedOperands;
}
