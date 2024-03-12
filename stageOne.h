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
StageOneFlags lineContextSO(char *line, StageOneFlags oldContext, int *possibleOpCode);
Bool isLegalSymbol(char *possibleSymbol, Bool isConst);
Bool isLabelDefinition(char* possibleLabel);
void freeSymbols();
void printSymbols();
