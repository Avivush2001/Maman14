/*Macros*/

#define MACRO_DEF "mcr"
#define MACRO_END "endmcr"
#define FAILED_MALLOC_PA contextFlag = errorEncounteredPA
#define MALLOC_MACRO malloc(sizeof(Macro))
#define MACRO_AT_INDEX macroTable->items[indexOfMacro].item
#define MACRO_NAME_AT_INDEX macroTable->items[indexOfMacro].name
#define COMPARE_MACRO_END !strcmp(field1,MACRO_END)
#define DEFAULT_CONTEXT_PA contextFlag = readingLine
#define ERROR_CASE_PA(_flag, _errorMessege) ERROR_CASE("pre assembler", _flag, _errorMessege)\
newFlag = errorEncounteredPA;\
*contextFlag = skipMacroDefinition;\
break;
#define FATAL_ERROR_CASE_PA(_flag, _errorMessege) ERROR_CASE("pre assembler", _flag, _errorMessege)\
newFlag = errorEncounteredPA;\
break;
#define WARNING_CASE_PA(_flag, _errorMessege) WARNING_CASE("pre assembler", _flag, _errorMessege)\
*contextFlag = readingLine;\
break;

/*structures*/

/*Flags used by the pre assembler*/
typedef enum {
    readingLine,
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
pointer to the .as file if an error occured during the run.*/
PreassemblerFlags preassembler(FILE*,char*,SymbolHashTable*);

/*Checks the context of a given line.*/
PreassemblerFlags lineContext(PreassemblerFlags, char *, int *, SymbolHashTable *);

/*Checks a given field for a macro call*/
PreassemblerFlags checkForMacroCall(char *, int *, SymbolHashTable *);

/*Checks if a given name can be defined as a macro.*/
PreassemblerFlags canDefineMacro(char *, int , SymbolHashTable *);

/*Error handler. Using the context flag decide which error to display.*/
PreassemblerFlags errorHandler(PreassemblerFlags *, PreassemblerFlags, int, char *);

/*Frees the macros in the table, not the table itself.
We won't need the actual macros in the rest of the assembler's operations,
but we will need their names for later stages.*/
SymbolHashTable *freeMacrosFromTable(SymbolHashTable*);

/*Recursive function to free macros and their lines.*/
Macro *freeMacros(Macro *macptr);
