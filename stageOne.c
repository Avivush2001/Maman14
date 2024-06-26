#include "data.h"


extern HashTable symbolHashTable;
extern int IC, DC;
extern Operation operationsArr[];

/*
It gets the a pointer to the line and to an integer to possibly store 
the opcode of the operation in the line. In the end it returns a flag, and inserts labels when encountered.
*/
static StageOneFlags lineContextSO(char *, int *, Symbol **);

/*
This function defines an external or entry label. It gets the line and
if it is an entry call, handles the definition, and returns a flag.
*/
static StageOneFlags defineExternOrEntryLabel(char *, Bool);

/*This functions gets the line and defines a constant.*/
static StageOneFlags defineConstant(char *);

/*Inserts string from the line to the memory.*/
static StageOneFlags insertStringToMemory(const char *, Symbol *);

/*Inserts data from the line to the memory.*/
static StageOneFlags insertData(char *, Symbol *);

/*
This is the main stage one function. It gets the file, and starts reading each line.
It first gets the context of the line and defines labels if there is one.
than based on the context it will: define an entry/external label or constant,
insert to the memory data or string, or insert to the memory
an operation an its operands.
If an error occurs the loop breaks.
*/
StageOneFlags stageOne(FILE *fp) {
    Symbol *definedLabel;
    char line[MAX_LINE_LENGTH];
    int lineCounter = 1, possibleOpCode;
    StageOneFlags contextFlag, errorFlagSO; 
    OperandsFlags opFlag;
    MemoryFlags memFlag;
    DEFAULT_CONTEXT_SO
    errorFlagSO = allclearSO;
    /*Main loop to get the line*/
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        /*
        Set up 2 fields. No need to malloc extra memory for them
        since they won't be needed to be saved, just their values.
        */
        Field field1 = {immediate,NULL,0 }, field2 = {immediate,NULL,0 };
        definedLabel = NULL;
        possibleOpCode = NOT_FOUND;
        opFlag = notReadOperands;
        memFlag = memoryAvailable;
        /*Get the line context*/
        contextFlag = lineContextSO(line, &possibleOpCode, &definedLabel);
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
                /*
                First it sets up the operands, checking if they are legal,
                and counts them. Then based on that count we check
                the the number of fields enabled for the given operation,
                and the same for the fields' types.
                If all goes well it will insert the operation and the operands
                to the memory. The memory function assumes all the information
                above is correct, and that the field pointers were inserted correctly.
                */
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
                    case legal2Operand:
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
                contextFlag = insertData(line, definedLabel); 
                break;
            case isString: 
                contextFlag = insertStringToMemory(line, definedLabel);
                break;
            default:
                break;
        }
        /*Handle errors and advance the line counter.*/
        errorFlagSO = errorHandlerSO(contextFlag, errorFlagSO, memFlag, opFlag, lineCounter);
        lineCounter++;
    }
    addDataToMemory();
    updateDataLabels();
    return errorFlagSO;
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
static StageOneFlags lineContextSO(char *line, int *possibleOpCode, Symbol **definedLabel) {
    int i, stringsCounter;
    Symbol *symb = NULL;
    HashTableItem *symbolItem;
    StageOneFlags contextFlag = readingLineSO;
    char str1[MAX_LABEL_SIZE+1], str2[MAX_LABEL_SIZE], str3[MAX_LABEL_SIZE];
    /*Get strings*/
    stringsCounter = sscanf(line, "%32s %31s %31s", str1, str2, str3);
    if (stringsCounter == -1) contextFlag = skipLine;/*Check for empty line*/
    else if (*str1 == ';') contextFlag = skipLine;/*Check for comment line*/
    else{
        /*Checking if the first string can be a legal label*/
        if (isLabelDefinition(str1)) {
            i = lookUpTable(&symbolHashTable, str1);
            /*Initializing entry label. If it wasn't an entry label isLabelDefinition would return false*/
            if (i != NOT_FOUND) {
                symbolItem = symbolHashTable.items + i;
                symb = symbolItem->item;
                if (symb->attr != undefined) 
                    contextFlag = errorDefinedEntryLabel;
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
                    symb->arrayLength = 0;
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
                    printf( "WARNING Label: %s is in an illegal location and will be considered undefined.\n", symb->symbol);
                    free(symb->symbol);
                    free(symb);
                    symb = NULL;
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
    *definedLabel = symb;
    return contextFlag;
}

/*
This function defines an external or entry label. It finds the instruction in the line
and skips it, and then scans two strings, one for the label and another one for unexpected strings.
Look it up in the table, if it isn't found check if it is legal and insert it.
If it is found, incase it is already an entry print a warning, if not turn the isEntry flag on.
If it is found and it is defining an external label, and it is already external print a warning, if not change the flag to an error.
*/
static StageOneFlags defineExternOrEntryLabel(char *line, Bool isEntry) 
{
    char *p, label[MAX_LABEL_SIZE], garbage[2];
    StageOneFlags flag = allclearSO;
    Symbol *symb;
    int i;
    *garbage = '\0';
    *label = '\0';

    /*Skip the instruction*/
    if (isEntry)
        p = strchr(line, '.') + 6;
    else p = strchr(line, '.') + 7;

    /*Get the label*/
    sscanf(p, "%31s %1s", label, garbage);

    /*Check the input*/
    if (*garbage != '\0' || *label == '\0') flag = errorDefiningEntryOrExtern;

    i = lookUpTable(&symbolHashTable, label);

    /*If it isn't Found*/
    if (i == NOT_FOUND && flag == allclearSO) {
        if(isLegalSymbol(label, True) && (i = insertToTable(&symbolHashTable, label)) != NOT_FOUND) {
            symb = MALLOC_SYMBOL;
            EXIT_IF(symb == NULL)
            symb->symbol = symbolHashTable.items[i].name;
            if (isEntry) {
                symb->attr = undefined;
            } else symb->attr = external;
            symb->entry = isEntry;
            symb->value = 0;
            symb->arrayLength = 0;
            symbolHashTable.items[i].item = symb;
        } else flag = errorDefiningEntryOrExtern;

    } else if (flag == allclearSO) { /*If it is Found*/
        symb = symbolHashTable.items[i].item;
        if (isEntry) {
            if (symb->entry) 
                printf( "WARNING entry label %s defined multiple times.\n", label);
            
            if (symb->attr != external && symb->attr != constant) 
                symb->entry = isEntry;
            else flag = errorDefiningEntryOrExtern;
        } else {
            if (symb->attr == external)
                printf( "WARNING extern label %s defined multiple times.\n", label);
            else flag = errorDefiningEntryOrExtern;
            
        }
    }
    return flag;
}

/*
The function skips the instruction in the line, and scanf for 4 strings:
the symbol of the constant (not it is not checked for legality since it
is checked in the line context function)
the equals character and the constant itself, and leftover graphical characters.

It checks if the parameters are legal, inserting the constant to the table and initializing it. 
*/
static StageOneFlags defineConstant(char *line) {
    char *p = strchr(line, '.') + 7, str1[MAX_LABEL_SIZE], str2[MAX_LABEL_SIZE], str3[MAX_LABEL_SIZE] ,garbage[2];
    StageOneFlags flag;
    int i;
    Symbol *symb;
    WholeNum value = {False, 0};
    *str1 = *str2 = *str3 = *garbage ='\0';
    
    /*Get the parameters*/
    sscanf(p, "%31s %31s %31s %1s", str1, str2, str3, garbage);
    /*Check legality of parameters*/
    if (*str3 == '\0' || strcmp(str2,"=") || *garbage != '\0') {
        flag = errorDefiningConstant;
    } else {
        /*Convert the string to an int.*/
        value = stringToInt(str3);
        /*Ensure the legality of the value and insert the constant.*/
        if ((i = insertToTable(&symbolHashTable, str1)) != NOT_FOUND && value.isNum && IN_CONST_RANGE(value.result)) {
            symb = MALLOC_SYMBOL;
            EXIT_IF(symb == NULL)
            symb->symbol = symbolHashTable.items[i].name;
            symb->attr = constant;
            symb->value = value.result;
            symb->entry = False;
            symbolHashTable.items[i].item = symb;
            flag = allclearSO;
        } else {
            if (i == NOT_FOUND) flag = errorSymbolHashTableFull;
            if (!value.isNum || !IN_CONST_RANGE(value.result)) flag = errorIllegalConstant;
        }
    }
    return flag;
}

/*
The function take the line, skips to after the .string keyword,
checks the brackets and the rest of the line, if it is fine,
it reads the string between the brackets, by reading each char,
putting it in a data structure and inserting it to the memory
*/
static StageOneFlags insertStringToMemory(const char *str, Symbol * label)
{
    StageOneFlags flag = legalString;
    MemoryFlags memFlag;
    Data data = {0};
    char *left, *right, *p = strchr(str,'.') + 7;
    int counter = 0;
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
                    counter++;
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
                counter++;
                if(memFlag != wordCreationSuccess)
                    flag = errorMemoryFull;
            }
        }
        
    }
    label->arrayLength = counter;
    return flag;  
}

/*
The function skips the instruction, checks if there are commas and if their placements are correct.
Then it uses strtok to read as many tokens as there are, check their legality and insert
them as data to the memory.
*/
static StageOneFlags insertData(char *line, Symbol * label) {
    const char *delimiter = " , \n \r";
    char *p = strchr(line,'.') + 5, *token;
    Bool flagNoComma = False, commaFlag = True;
    StageOneFlags flag = allclearSO;
    MemoryFlags memFlag;
    int i = 0, j, counter = 0;
    if (strchr(line, ',') == NULL) {
        flagNoComma = True;
    }
    
    /*Check the commas*/
    for(j = 0; j < strlen(p) && !flagNoComma; j++) {
        if (isgraph(*(p+j))) {
            commaFlag = True;
            if (*(p+j) == ',')
                commaFlag = False;
        }
        
    }
    
    if (!commaFlag) flag = errorEnteringData;
    /*Start of insertion loop*/
    token = strtok(p, delimiter);
    while (token != NULL && flag != errorEnteringData) {
        /*Check in case there are too many tokens*/
        if ((i++) == 1 && flagNoComma) 
            flag = errorEnteringData;
        else {
            WholeNum number;
            Data data = {0};
            number = stringToInt(token); /*Read the number*/
            /*Check legality*/
            if (number.isNum && IN_DATA_RANGE(number.result)) {
                data.value = number.result;
                memFlag = insertDataWord(&data); /*Insert the data*/
                counter++;
            } else if((j = lookUpTable(&symbolHashTable, token)) != NOT_FOUND) { /*Check if it is constant*/
                Symbol *symb = symbolHashTable.items[j].item;
                    if(symb->attr == constant) {
                        data.value = symb->value;
                        memFlag = insertDataWord(&data); /*Insert the data*/
                        counter++;
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
    label->arrayLength = counter;
    return flag;
}



