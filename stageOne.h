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


/*The field struct is used for a memory function to insert operation binary's.
Note that when using a field that isn't a label, equal 'name' to NULL*/
