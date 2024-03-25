/*Macros*/

#define MACRO_DEF "mcr"
#define MACRO_END "endmcr"
#define FAILED_MALLOC_PA contextFlag = errorEncounteredPA
#define MALLOC_MACRO malloc(sizeof(Macro))
#define MACRO_AT_INDEX macroHashTable.items[indexOfMacro].item
#define MACRO_NAME_AT_INDEX macroHashTable.items[indexOfMacro].name
#define COMPARE_MACRO_END !strcmp(str1,MACRO_END)
#define DEFAULT_CONTEXT_PA contextFlag = readingLinePA
#define ERROR_CASE_PA(_flag, _errorMessage) ERROR_CASE("pre assembler", _flag, _errorMessage)\
newFlag = errorEncounteredPA;\
*contextFlag = skipMacroDefinition;\
break;
#define FATAL_ERROR_CASE_PA(_flag, _errorMessage) ERROR_CASE("pre assembler", _flag, _errorMessage)\
newFlag = errorEncounteredPA;\
break;
#define WARNING_CASE_PA(_flag, _errorMessage) WARNING_CASE("pre assembler", _flag, _errorMessage)\
*contextFlag = readingLinePA;\
break;

/*structures*/

/*Flags used by the pre assembler*/
typedef enum {
    readingLinePA,
    macroDefinitionStarted,
    macroDefinitionOngoing,
    macroDefinitionEnded,
    macroCall,
    addLine,
    skipMacroDefinition,
    skipUndefinedMacro,
    allclearPA,
    errorMacroNameIllegal,
    errorMacroNameAlreadyDefined,
    errorMacroHashTableFull,
    errorEncounteredPA,
    errorNoMacroNameGiven

} PreassemblerFlags;
/*Struct that defines a macro.
holds its name, a line, and a pointer to a macro structure
that holds the same name and the next line.*/
typedef struct MacroStruct {
    char *name;
    char *line;
    struct MacroStruct *nextLine;
} Macro;

/*Function Prototypes*/

/*Gets a pointer to the .as file, a new .am file name to give it, and a pointer to the
macro hash table. Returns a pointer to the new .am file created or returns the
pointer to the .as file if an error occurred during the run.*/
PreassemblerFlags preassembler(FILE*,char*);




