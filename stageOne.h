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
#define IN_CONST_RANGE(_val) (_val <= MAX_CONST && _val >=MIN_CONST)
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
    allclearSO
} StageOneFlags;

typedef enum {
    isConstant,
    isLabel,
    isArray,
    isRegister,
    illegalOperand,
    illegalConstantOperand,
    illegalLabel
} OperandsFlags;



StageOneFlags stageOne(FILE *, char *);
StageOneFlags lineContextSO(char *, int *);
Bool isLegalSymbol(char *, Bool);
Bool isLabelDefinition(char*);
StageOneFlags defineExternOrEntryLabels(char *, Bool);
StageOneFlags defineConstant(char *);
OperandsFlags areLegalOperands(char *, Field *, Field *);
OperandsFlags getOperandType(char *);
void freeSymbols();
void printSymbols();
