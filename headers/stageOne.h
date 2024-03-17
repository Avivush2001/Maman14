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
                } else contextFlag = errorIllegalSymbol;\
                break;\
            /*ERROR if the string equals one of the other items in the table*/\
            default:\
                contextFlag = errorIllegalKeyWord;\
                break;\
        }
#define IN_CONST_RANGE(_val) (_val <= MAX_CONST && _val >= MIN_CONST)
#define IN_DATA_RANGE(_val) (_val <= MAX_DATA && _val >=MIN_DATA)

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
    errorIllegalSymbol,
    errorSymbolHashTableFull,
    errorDefiningLabel,
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



StageOneFlags stageOne(FILE *, char *);

/*
It gets the a pointer to the line and to an integer to possibly store 
the opcode of the operation in the line. In the end it returns a flag, and inserts labels when encountered.
*/
StageOneFlags lineContextSO(char *, int *);
Bool isLegalSymbol(char *, Bool);
Bool isLabelDefinition(char*);
StageOneFlags defineExternOrEntryLabel(char *, Bool);
StageOneFlags defineConstant(char *);
OperandsFlags areLegalOperands(char *, Field *, Field *);
OperandsFlags getOperandType(char *);
StageOneFlags insertStringToMemory(const char *);
StageOneFlags insertData(char *);
void freeSymbols();
void printSymbols();
void updateDataLabels();