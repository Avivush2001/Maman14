#include "data.h"
/***************TODO LIST****************/
/*
-Add function prototypes
-try to split the main preassembler function to different functions
-Add free the macro function
*/

/*
Instructions for macros and the pre assembler:
-No nested macro definitions and no need to check for them.
-No calling to macro that doesn't exist in the file yet and no need to check it.
-Every macro definition will be closed with 'endmcr' and no need to check it.

Note: using the current algorithem, in a line where a macro is defined or a definition is ending,
every word written after them is discaredand ignored. Example:

mcr macro something
    ...
endmcr something

The 'something' is ignored.
//////////////////////////////
This is the main pre-assembler function.

Flags and their meaning:
Context Flag - Holds the state of the pre assembler and 
    gives the function context about a previous action of the progrem and what
    to do next.
Previous Context Flag - Holds the previous context flag.
    Mainly used for error handling.
Error Flag - Is updated only if an error occurs.

This is the algorithm:
1 - Read a line and store its adress in a pointer. If end of file is reached, go to
    the last step.
2 - Read the first two fields.
3 - Get the line context:
    addLine - go to step 4
    macroCall - go to step 5
    macroDefinitionStarted - go to step 6
    macroDefinitionOngoing - go to step 8
    skipMacroDefinition - go to step 1
    readingLine, skipunDefinedMacro, macroDefinitionEnded - change context flag
    to readingLine and go to step 1
4 - Add the line to the file, go to step 1.
5 - Get the macro from the table using the first field, add all the lines, go to step 1.
6 - Check for any possible error. If an error occured go to step 9.
7 - Insert the name to the macroTable, go to step 1.
8 - Insert the line to the macro using the index found earlier, go to step 1.
9 - Print an error message and change the context and error flags, go to step 1.
*/
FILE *preassembler(FILE *fp, char *fileName, SymbolHashTable *macroTable) {
    /******Initializations*******/
    char *line, *field1, *field2, *newName; 
    /*
    Index of the macro in the table that we are defining / inserting into the file, 
    counters for error handling.
    */
    int indexOfMacro, lineCounter, stringCounter;
    /*Flags as defined earlier*/
    PreassemblerFlags prevFlag, contextFlag, errorFlagPA; 
    /*Flags that signal either to free *field2 or *line.*/
    Bool freeField2, freeLine;
    /*File pointer to the new file.*/
    FILE *nfp;

    /*Pointers to the current macro item in the list and to the next one*/
    Macro *macptr, *nextMac;

    /*Create the .am file.*/
    fp = fopen(fileName, "w");
    
    /*Flag setups*/
    DEFAULT_CONTEXT_PA;
    errorFlagPA = allclearPA;

    /*Initialize the table*/
    macroTable = initializeHashTable(macroTable);
    /*Line memory allocation*/
    line = MALLOC_LINE;
    /*Intigers setup*/
    lineCounter = 1;
    indexOfMacro = NOT_FOUND;
    
    /*Main loop (step 1)*/
    while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) {
        /*Memory allocations and reading the first two fields (step 2)*/
        field1 = MALLOC_LABEL;
        field2 = MALLOC_LABEL;
        stringCounter = READ_FIRST_TWO_FIELDS;
        freeField2 = True;
        freeLine = True;
        /*Get the line's context (step 3)*/
        contextFlag = lineContext( contextFlag, field1, &indexOfMacro, macroTable);
        switch (contextFlag) {
            /*Add line to file (step 4)*/
            case addLine: {        
                        
                DEFAULT_CONTEXT_PA;
                fputs(line,nfp);
                break;
            }
            /*Add all the lines from the macro into the file using the 
            index we got when getting the line context indexOfMacro (step 5)*/
            case macroCall: {
                macptr = MACRO_AT_INDEX;
                while(macptr->nextLine != NULL) {
                    fputs(macptr->line, nfp);
                    macptr = macptr->nextLine;
                }
                DEFAULT_CONTEXT_PA;
                break;
            }
            /*Add the macro to the table (steps 6 and 7)*/
            case macroDefinitionStarted: {
                contextFlag = canDefineMacro(field2, stringCounter, macroTable);
                if (contextFlag == macroDefinitionStarted) {
                    contextFlag = macroDefinitionOngoing;
                    indexOfMacro = insertToTable(macroTable, field2);
                    macptr = malloc(sizeof(Macro));
                    macptr->name = field2;
                    MACRO_AT_INDEX = macptr;
                    freeField2 = False;
                }
                break;
            }
            /*Add the line to the macr (step 8)*/
            case macroDefinitionOngoing: {
                macptr->line = line;
                nextMac = malloc(sizeof(Macro));
                nextMac->name = macptr->name;
                macptr->nextLine = nextMac;
                macptr = nextMac;
                freeLine = False;
                break;
            }
            /*Dont do anything for skipMacroDefinition*/
            case skipMacroDefinition: break;
            /*Dont do anything but change the context for skipUndefinedMacro, macroDefinitionEnded*/
            default: {
                DEFAULT_CONTEXT_PA;
                break;
            }
        }
        /*Handle errors (step 9)*/
        errorFlagPA = errorHandler(&contextFlag, errorFlagPA,lineCounter, fileName);
        /*Free relevent memeory spaces*/
        free(field1);
        if (freeField2) free(field2);
        if (freeLine) free(line);
        /*Allocate new memory for the line and advance the line counter.*/
        line = MALLOC_LINE;
        lineCounter++;
    }
    /*Delete the file that was created if an error was encountered.*/
    if (errorFlagPA == errorEncounteredPA) {
        fclose(nfp);
        remove(fileName);
        nfp = fp;
    }
    free(line);
    macroTable = freeMacrosFromTable(macroTable);
    return nfp;
}
/*
Get the context of the line. The function is supposed to get one of these 3 flags:
skipMacroDefinition, readingLine, macroDefinitionOngoing.
Using the flag and the first field we got earlier, it returns a new flag,
and if it found a macro call indexOfMacro will be updated to that index.
*/
PreassemblerFlags lineContext(PreassemblerFlags currentFlag, char *field1, int *indexOfMacro, SymbolHashTable *macroTable) {
    PreassemblerFlags newFlag;
    newFlag = currentFlag;
    if (!strcmp(MACRO_DEF, field1)) newFlag = macroDefinitionStarted;

    switch (currentFlag) {
        /*If skipMacroDefinition is on check if the first field is the end of the macro.
        If it is it means the macro definition we are skipping ended. */
        case skipMacroDefinition:
            if (COMPARE_MACRO_END) newFlag = readingLine;
            break;
        /*If readingLine is on, check for macro call.*/
        case readingLine:
            newFlag = checkForMacroCall(field1, indexOfMacro,macroTable);
            break;
        /*If macroDefinitionOngoing is on, check if the first field is the end of the macro
        definition. If it is the definition ended, if it isn't it's still ongoing*/
        case macroDefinitionOngoing:
            if (COMPARE_MACRO_END) {
               newFlag = macroDefinitionEnded;
            } else newFlag = macroDefinitionOngoing;
            break;
    }
    return newFlag;
}
/*The function tries to find the macro in the table. If it found it, the flag will be
macroCall. If it didn't find it, but the label is legal (meaning it is a viable macro name)
then this is an undefined macro call, so skip it. otherwise add the line*/
PreassemblerFlags checkForMacroCall(char *field, int *indexOfMacro, SymbolHashTable *macroTable) {
    PreassemblerFlags newFlag;  
    if ((*indexOfMacro = lookUpTable(macroTable, field)) == NOT_FOUND) newFlag = addLine;
    else if (isLabelLegal(field)) newFlag = skipUndefinedMacro;
    else newFlag = macroCall;
    
    return newFlag;
}
/*Check for possible error before defining a macro.
Possible errors are:
no macro name given, label isn't legal, hashtable is full, macro name alread in the table.*/
PreassemblerFlags canDefineMacro(char *macroName, int stringCount, SymbolHashTable *macroTable) {
    PreassemblerFlags newFlag;
    newFlag = macroDefinitionStarted;
    if (stringCount != 2) 
        newFlag = errorNoMacroNameGiven;
    else if (!isLabelLegal(macroName)) 
        newFlag = errorMacroNameIllegal;
    else if (macroTable->flag == hashTableFull) 
        newFlag = errorMacroHashTableFull;
    else if (lookUpTable(macroTable, macroName) != NOT_FOUND) 
        newFlag = errorMacroNameAlreadyDefined;
    return newFlag;
    
}
/*Error handler. Using the context flag decide which error to display.*/
PreassemblerFlags errorHandler(PreassemblerFlags *contextFlag, PreassemblerFlags errorFlag, int lineCounter, char *fileName) {
    PreassemblerFlags newFlag;
    newFlag = errorFlag;
    switch (*contextFlag) {
        ERROR_CASE_PA(errorNoMacroNameGiven, "No macro name given.\n")
        ERROR_CASE_PA(errorMacroNameIllegal, "Macro name is illegal.\n")
        ERROR_CASE_PA(errorMacroHashTableFull, "Hash table is full.\n")
        ERROR_CASE_PA(errorMacroNameAlreadyDefined, "Macro name is already defined.\n")
    }
    return newFlag;
}

/*Frees the macros in the table, not the table itself.
We won't need the actual macros in the rest of the assembler's operations,
but we will need their names for later stages.*/
SymbolHashTable *freeMacrosFromTable(SymbolHashTable* macroTable) {
    int indexOfMacro;
    Macro *macptr;
    char *name;
    for (indexOfMacro = 0; indexOfMacro < HASHSIZE; indexOfMacro++) {
        if ((macptr = MACRO_AT_INDEX) != NULL) {
            name = macptr->name;
            macptr = freeMacros(macptr);
            free(macptr);
            MACRO_AT_INDEX = NULL;
        }
    }
}
/*Recursive functions to free macros and their lines.*/
Macro *freeMacros(Macro *macptr) {
    char *line;
    Macro *nextMac;
    line = macptr->line;
    nextMac = macptr->nextLine;
    if (line != NULL) {
        nextMac = freeMacros(nextMac);
        free(line);
        free(nextMac);
    }
    return macptr;
    
}