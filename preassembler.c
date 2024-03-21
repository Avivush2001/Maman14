#include "data.h"
extern HashTable macroHashTable;
/*
Instructions for macros and the pre assembler:
-No nested macro definitions and no need to check for them.
-No calling to macro that doesn't exist in the file yet and no need to check it.
-Every macro definition will be closed with 'endmcr' and no need to check it.

Note: using the current algorithm, in a line where a macro is defined or a definition is ending,
every word written after them is discared and ignored. Example:

mcr macro something
    ...
endmcr something

The 'something' is ignored.
//////////////////////////////
This is the main pre-assembler function.

Flags and their meaning:
Context Flag - Holds the state of the pre assembler and 
    gives the function context about a previous action of the program and what
    to do next.
Error Flag - Is updated only if an error occurs.

This is the algorithm:
1 - Read a line and store its address in a pointer. If end of file is reached, go to
    the last step.
2 - Read the first two fields.
3 - Get the line context:
    addLine - go to step 4
    macroCall - go to step 5
    macroDefinitionStarted - go to step 6
    macroDefinitionOngoing - go to step 8
    skipMacroDefinition - go to step 1
    readingLinePA, skipUnDefinedMacro, macroDefinitionEnded - change context flag
    to readingLinePA and go to step 1
4 - Add the line to the file, go to step 1.
5 - Get the macro from the table using the first field, add all the lines, go to step 1.
6 - Check for any possible error. If an error occurred go to step 9.
7 - Insert the name to the macroTable, go to step 1.
8 - Insert the line to the macro using the index found earlier, go to step 1.
9 - Print an error message and change the context and error flags, go to step 1.
*/

/*Change the Function to not need the macro table symbol hash table pointer*/
PreassemblerFlags preassembler(FILE *fp, char *fileName) {
    
    /******Initializations*******/
    char line[MAX_LINE_LENGTH], str1[MAX_LABEL_SIZE], str2[MAX_LABEL_SIZE], *newName = newFileName(fileName, ".am"); 
    /*
    Index of the macro in the table that we are defining / inserting into the file, 
    counters for error handling.
    */
    int indexOfMacro, lineCounter, stringCounter;

    /*Flags as defined earlier*/
    PreassemblerFlags contextFlag, errorFlagPA; 


    /*File pointer to the new file.*/
    FILE *nfp;

    /*Pointers to the current macro item in the list and to the next one*/
    Macro *macptr, *nextMac;

    /*Create the .am file.*/
    OPEN_NEW_FILE

    /*Flag setups*/
    DEFAULT_CONTEXT_PA;
    errorFlagPA = allclearPA;

    /*Integers setup*/
    lineCounter = 1;
    indexOfMacro = NOT_FOUND;

    /*Main loop (step 1)*/
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {

        /*Reading the first two fields (step 2)*/
        stringCounter = READ_FIRST_TWO_FIELDS;

        /*Get the line's context (step 3)*/
         contextFlag = lineContextPA( contextFlag, str1, &indexOfMacro);
        switch (contextFlag) {

            /*Add line to file (step 4)*/
            case addLine:      
                DEFAULT_CONTEXT_PA;
                fputs(line,nfp);
                break;
            
            /*Add all the lines from the macro into the file using the 
            index we got when getting the line context indexOfMacro (step 5)*/
            case macroCall: 
                macptr = MACRO_AT_INDEX;
                while(macptr->nextLine != NULL) {
                    fputs(macptr->line, nfp);
                    macptr = macptr->nextLine;
                }
                DEFAULT_CONTEXT_PA;
                
                break;
            
            /*Add the macro to the table (steps 6 and 7)*/
            case macroDefinitionStarted: 
                contextFlag = canDefineMacro(str2, stringCounter);
                if (contextFlag == macroDefinitionStarted) {
                    EXIT_IF((macptr = MALLOC_MACRO) == NULL)
                    contextFlag = macroDefinitionOngoing;
                    indexOfMacro = insertToTable(&macroHashTable, str2);
                    macptr->name = str2;
                    MACRO_AT_INDEX = macptr;
                }
                break;
            
            /*Add the line to the macr (step 8)*/
            case macroDefinitionOngoing: 
                EXIT_IF(((nextMac = MALLOC_MACRO) == NULL) || ((macptr->line = MALLOC_LINE) == NULL))
                macptr->line = strcpy(macptr->line, line);
                nextMac->name = macptr->name;
                macptr->nextLine = nextMac;
                macptr = nextMac;
                macptr->nextLine = NULL;
                break;
            
            /*Dont do anything but change the context for macroDefinitionEnded*/
            case macroDefinitionEnded: 
                DEFAULT_CONTEXT_PA;
                break;
            /*Dont do anything for skipMacroDefinition and skipUndefinedMacro*/
            default: break;
        }
        lineCounter++;
        /*Handle errors (step 9)*/
        errorFlagPA = errorHandlerPA(&contextFlag, errorFlagPA,lineCounter, fileName);
    }
    free(newName);
    freeMacrosFromTable();
    fclose(nfp);
    return errorFlagPA;
}

/*
Get the context of the line. The function is supposed to get one of these 3 flags:
skipMacroDefinition, readingLinePA, macroDefinitionOngoing.
Using the flag and the first field we got earlier, it returns a new flag,
and if it found a macro call indexOfMacro will be updated to that index.
*/
PreassemblerFlags lineContextPA(PreassemblerFlags currentFlag, char *str1, int *indexOfMacro) {
    PreassemblerFlags newFlag;
    newFlag = currentFlag;
    switch (currentFlag) {

        /*If skipMacroDefinition is on check if the first field is the end of the macro.
        If it is it means the macro definition we are skipping ended. */
        case skipMacroDefinition:
            if (COMPARE_MACRO_END) newFlag = readingLinePA;
            break;

        /*If readingLinePA is on, check for macro call.*/
        case readingLinePA:
            if (!strcmp(MACRO_DEF, str1)) newFlag = macroDefinitionStarted;
            else newFlag = checkForMacroCall(str1, indexOfMacro);
            break;

        /*If macroDefinitionOngoing is on, check if the first field is the end of the macro
        definition. If it is the definition ended, if it isn't it's still ongoing*/
        case macroDefinitionOngoing:
            if (COMPARE_MACRO_END) {
               newFlag = macroDefinitionEnded;
            } else newFlag = macroDefinitionOngoing;
            break;
        default: break;
    }
    return newFlag;
}

/*The function tries to find the macro in the table. If it found it, the flag will be
macroCall. If it didn't find it, but the label is legal (meaning it is a viable macro name)
then this is an undefined macro call, so skip it. otherwise add the line*/
PreassemblerFlags checkForMacroCall(char *field, int *indexOfMacro) {
    PreassemblerFlags newFlag;
    if ((*indexOfMacro = lookUpTable(&macroHashTable, field)) != NOT_FOUND) newFlag = macroCall;
    else if (isLabelLegal(field)) newFlag = skipUndefinedMacro;
    else newFlag = addLine;
    
    return newFlag;
}

/*Check for possible error before defining a macro.
Possible errors are:
no macro name given, label isn't legal, hashtable is full, macro name already in the table.*/
PreassemblerFlags canDefineMacro(char *macroName, int stringCount) {
    PreassemblerFlags newFlag;
    newFlag = macroDefinitionStarted;

    if (stringCount != 2) 
        newFlag = errorNoMacroNameGiven;

    else if (!isLabelLegal(macroName)) 
        newFlag = errorMacroNameIllegal;

    else if (macroHashTable.flag == hashTableFull) 
        newFlag = errorMacroHashTableFull;

    else if (lookUpTable(&macroHashTable, macroName) != NOT_FOUND) 
        newFlag = errorMacroNameAlreadyDefined;

    return newFlag;
    
}

/*Error handler. Using the context flag decide which error to display.*/
PreassemblerFlags errorHandlerPA(PreassemblerFlags *contextFlag, PreassemblerFlags errorFlag, int lineCounter, char *fileName) {
    PreassemblerFlags newFlag;
    newFlag = errorFlag;
    switch (*contextFlag) {
        ERROR_CASE_PA(errorNoMacroNameGiven, "No macro name given.\n")
        ERROR_CASE_PA(errorMacroNameIllegal, "Macro name is illegal.\n")
        ERROR_CASE_PA(errorMacroHashTableFull, "Hash table is full.\n")
        ERROR_CASE_PA(errorMacroNameAlreadyDefined, "Macro name is already defined.\n")
        WARNING_CASE_PA(skipUndefinedMacro, "Tried to call to and undefined macro, call skipped.\n")
        default: break;
    }
    return newFlag;
}

/*Frees the macros in the taBle, not the tAble itself.
We won't need the actual macRos in the rest of the Assembler's operations,
but we will need to checK their names for later stages.*/
void freeMacrosFromTable() {
    int indexOfMacro;
    Macro *macptr;
    for (indexOfMacro = 0; indexOfMacro < HASHSIZE; indexOfMacro++) {
        if ((macptr = MACRO_AT_INDEX) != NULL) {
            macptr = freeMacros(macptr);
            free(macptr);
            MACRO_AT_INDEX = NULL;
        }
    }
}

/*Recursive function to free macros and their lines.*/
Macro *freeMacros(Macro *macptr) {
    char *line;
    Macro *nextMac;
    line = macptr->line;
    nextMac = macptr->nextLine;
    if (nextMac != NULL) {
        nextMac = freeMacros(nextMac);
        free(line);
        free(nextMac);
    }
    return macptr;
    
}