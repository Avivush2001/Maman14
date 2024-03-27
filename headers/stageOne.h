#define DEFAULT_CONTEXT_SO contextFlag = readingLineSO;
#define MALLOC_SYMBOL malloc(sizeof(Symbol))
#define CHECK_INSTRUCTIONS switch(i) {\
            /*case ".entry"*/\
            case 0:\
                contextFlag = entryDefinition;\
                break;\
            /*case ".extern"*/\
            case 1:\
                contextFlag = externDefinition;\
                break;\
            /*case".define"*/\
            case 2:\
                if(isLegalSymbol(str2, True)) {\
                    contextFlag = constantDefinition;\
                } else contextFlag = errorIllegalSymbolOrTableFull;\
                break;\
            /*ERROR if the string equals one of the other items in the table*/\
            default:\
                contextFlag = errorIllegalKeyWord;\
                break;\
        }
#define IN_CONST_RANGE(_val) (_val <= MAX_CONST && _val >= MIN_CONST)
#define IN_DATA_RANGE(_val) (_val <= MAX_DATA && _val >=MIN_DATA)
#define ERROR_CASE_SO(_flag, _errorMessage) ERROR_CASE("First", _flag, _errorMessage)\
flag = errorEncounteredSO;\
break;

#define BREAK_WITH_FLAG(_flag) flag = _flag;\
                        operandCounter = NOT_FOUND;\
                        break;

#define SETUP_FIELD_CONSTANT(_num) field##_num ->symbol = NULL;\
                    field##_num ->type = immediate;\
                    field##_num->value = value;\
                    flag = legal##_num##Operand;

#define SETUP_FIELD_LABEL(_num)field##_num ->symbol = label;\
                    field##_num ->type = direct;\
                    field##_num ->value = 0;\
                    flag = legal##_num##Operand;

#define SETUP_FIELD_ARRAY(_num)field##_num ->symbol = label;\
                    field##_num ->type = index;\
                    field##_num ->value = value;\
                    flag = legal##_num##Operand;

#define SETUP_FIELD_REG(_num) field##_num->symbol = NULL;\
                    field##_num->type = reg;\
                    field##_num ->value = value;\
                    flag = legal##_num##Operand;

typedef enum {
    readingLineSO,
    externDefinition,
    entryDefinition,
    skipLine,
    constantDefinition,
    isData,
    isString,
    mightBeOperation,
    isOperation,
    warningLabelInDumbPlace,
    errorEncounteredSO,
    errorIllegalKeyWord,
    errorSymbolHashTableFull,
    errorDefiningConstant,
    errorIllegalConstant,
    errorEnteringData,
    errorMemoryFull,
    errorOperandTypes,
    errorDefiningEntryOrExtern,
    illegalString,
    legalString,
    errorIllegalSymbolOrTableFull,
    errorDefinedEntryLabel,
    allclearSO
} StageOneFlags;

typedef enum {
    noOperands,
    legal1Operand,
    legal2Operand,
    isConstant,
    isLabel,
    isArray,
    isRegister,
    illegalOperand,
    illegalConstantOperand,
    tooManyOperands,
    errorCommas,
    failedCalloc,
    notReadOperands
} OperandsFlags;


/*This is the main first stage function.*/
StageOneFlags stageOne(FILE *);
/*Check if a symbol, constant or label is legal and undefined already.*/
Bool isLegalSymbol(char *, Bool);

/*Checks if the first string at the start of a line is a label definition.*/
Bool isLabelDefinition(char*);

/*
Parses the line to find the operands and sets up two
field structures accordingly.
*/
OperandsFlags areLegalOperands(char *, Field *, Field *);

/*Finds the type of an operand*/
OperandsFlags getOperandType(char *);

/*Error handling function*/
StageOneFlags errorHandlerSO(StageOneFlags, MemoryFlags, OperandsFlags, int);

/*Frees the symbols*/
void freeSymbols();

/*Prints symbols for debugging*/
void printSymbols();

/*Update data labels to the correct address*/
void updateDataLabels();

