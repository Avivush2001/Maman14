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

/* Checks if a string is a whole number- if not return 0, if it is return the number as a int */

wholeNum string_to_int(const char *str) 
{
    wholeNum num;
  if (str == NULL || *str == '\0') {
    num.isNum = 0;
    return num; // Empty string is not a number
  }

  int sign = 1; // 1 for positive, -1 for negative
  if (*str == '+') {
    str++; // Skip the positive sign if present
  }
  else if (*str == '-') {
    sign = -1;
    str++; // Skip the negative sign if present
  }

  long long int x = 0; // Use long long to handle larger numbers

  while (*str) {
    if (!isdigit(*str)) {
      num.isNum = 0;
      return num; // Invalid character encountered
    }
    int digit_value = *str - '0'; // Convert ASCII digit to numeric value
    x = x * 10 + digit_value; // Build the integer

    // Check for overflow/underflow 
    if (x * sign < INT_MIN || x * sign > INT_MAX) {
      x = (sign == 1 ? INT_MAX : INT_MIN); // Return max/min for overflow/underflow
      num.result = x;
      num.isNum = 1;
      return num;
    }

    str++;
  }

  num.result = x;
  num.isNum = 1;
  return num;
}

Bool areLegalOperands(char *str, Field *field1, Field *field2)
{
    char *flag;
    char *token;
    const char *delimiter = ",";
    int operandCounter = 0;
    token = strtok(str, delimiter);
    while(token != NULL && operandCounter <= 2)
    {
        if((token[0] != '#') && (isalpha(token[0]) == 0))
        {
            flag = "illegal operand";
            return False;
        }
        if(token[0] == '#') /* it is possibly an immediate operand */
        {
            token++;
            wholeNum num = string_to_int(token);
            if(num.isNum == 1)
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
            else if(isLegalSymbol(token, True) == True)
            {
                operandCounter++;
                if(operandCounter == 1)
                {
                    field1->symbol = NULL;
                    field1->type = immediate;
                    field1->value = &symbolHashTable.items[lookUpTable(&symbolHashTable, token)].item;
                }
                if(operandCounter == 2)
                {
                    field2->symbol = NULL;
                    field2->type = immediate;
                    field2->value = &symbolHashTable.items[lookUpTable(&symbolHashTable, token)].item;
                }
            }
            else
            {
                flag = "illegal number or an unrecognized label";
                return False;
            }
        }

        token = strtok(NULL, delimiter);
    }
}
