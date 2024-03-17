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
    errorIllegalSymbolOrTableFull,
    errorEnteringData,
    errorMemoryFull,
    errorOperandTypes,
    errorDefiningEntryOrExtern,
    illegalString,
    legalString, 
    allclearSO
} StageOneFlags;

typedef enum {
    isConstant,
    isLabel,
    isArray,
    isRegister,
    legal1Operand,
    legal2Operands,
    illegalOperand,
    illegalConstantOperand,
    tooManyOperands,
    noOperands,
    failedCalloc,
    notReadOperands
} OperandsFlags;


/*
This is the main first stage function.
*/
StageOneFlags stageOne(FILE *, char *);

/*
It gets the a pointer to the line and to an integer to possibly store 
the opcode of the operation in the line. In the end it returns a flag, and inserts labels when encountered.
*/
StageOneFlags lineContextSO(char *, int *);
Bool isLegalSymbol(char *, Bool);
Bool isLabelDefinition(char*);

/*
This function defines an external or entry label. It gets the line and
if it is an entry call, handles the definition, and returns a flag.
*/
StageOneFlags defineExternOrEntryLabel(char *, Bool);

/*
This functions gets the line and defines a constant.
*/
StageOneFlags defineConstant(char *);
OperandsFlags areLegalOperands(char *, Field *, Field *);
OperandsFlags getOperandType(char *);
StageOneFlags insertStringToMemory(const char *);

/*
Inserts data from the line.
*/
StageOneFlags insertData(char *);
StageOneFlags errorHandlerSO(StageOneFlags, MemoryFlags, OperandsFlags, int);
void freeSymbols();
void printSymbols();
void updateDataLabels();
