#include "data.h"
FILE *preassembler(FILE*,char*);
PreassemblerFlags lookForMacro(char*, int*);
SymbolHashTable macroTable;

/*Needed to be global so the rest of the program can recognize it.*/


/***************TODO LIST****************/
/*
-Add function prototypes
-Add Better Documantation
-try to split the main preassembler function to different functions
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
*/

/*
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
2 - Read the first word in the line and store its adress in a pointer.
3 - Is the context flag isn't skipping a macro definition? if not skip to step 7.
4 - Save the current context flag in the previous context flag, and get the new context flag
    using it and the first word we got earlier. If the new context throws out an error, go to step 6
5 - Switch-Case statement, depends on the context given earlier it does an action and updates
    the context flag if needed. If the context flag isn't giving out an error go to step 1.
6 - An error has occured during macro definition. Using the previous context 
    print an error messege and, update the error flag and the context flag 
    to skip a macro definition. Go to step one.
7 - If the first word equals 'endmcr' update the context flag. Go to step 1.
8 - If the error flag is on, close the file we created and remove it.
*/
FILE *preassembler(FILE *fp, char *fileName) {
    /******Initializations*******/

    /*
    Pointers to the line, 
    to the first word in the line, 
    to the macro name.
    (Haiku for no reason)
    */
    char *line, *firstWord, *macroName; 
    
    /*
    Index of the macro we are defining / inserting into the file, 
    a line counter for error handling,
    */
    int indexOfMacro, lineCounter;
    

    /*
    Flags for the context of the current line,
    an error flag for when an error was encountered atleast once
    a flag for the previous flag for error handling
    */
    PreassemblerFlags prevFlag, contextFlag, errorFlagPA; 
    
    /*File pointer to the new file.*/
    FILE *nfp;

    /*Pointers to the current macro item in the list and to the next one*/
    Macro *macptr, *nextMac;

    /*Pointer to the macroTable*/
    SymbolHashTable *macroTableptr;
    /*Create a file with a new suffix.*/
    char *newName;
    newName = newFileName(fileName,'am');
    fp = fopen(newName, "w");
    
    /*Flag setups*/
    contextFlag = readingLine;
    errorFlagPA = allclearPA;

    /*Initialize the table*/
    macroTableptr = initializeHashTable(&macroTable);

    /*Line memory allocation*/
    line = malloc(sizeof(char) * MAX_LINE_LENGTH);
    
    /*Intigers setup*/
    lineCounter = 1;
    indexOfMacro = NOT_FOUND;
    
    /*Main loop*/
    while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF && lineCounter <= RAM_SIZE) {
        firstWord = readFirstWord(line);
        if (contextFlag != skipMacroDefinition) {
            prevFlag = contextFlag;
            contextFlag = lineContext(line, firstWord, contextFlag, &indexOfMacro);
        }
        /*get its context*/
        
        switch (contextFlag) {
            /*Add line to file*/
            case addLine: {                
                contextFlag = readingLine;
                fputs(line,nfp);
                break;
            }
            /*Add all the lines from the macro into the file using indexOfMacro*/
            case macroCall: {
                macptr = macroTable.items[indexOfMacro].item;
                while(macptr->nextLine != NULL) {
                    fputs(macptr->line, nfp);
                    macptr = macptr->nextLine;
                }
                contextFlag = readingLine;
                break;
            }
            /*Add the macro to the table*/
            case macroDefinitionStarted: {
                macroName = getMacroName(line);
                prevFlag = contextFlag;
                if (isLabelLegal(macroName)) {
                    contextFlag = macroDefinitionOngoing;
                    indexOfMacro = insertToTable(&macroTable, macroName);
                    macptr = malloc(sizeof(Macro));
                    macptr->name = macroName;
                    macroTable.items[indexOfMacro].item = macptr;
                } else contextFlag = errorPA;
                break;
            }
            /*Add the line to the macro*/
            case macroDefinitionOngoing: {
                macptr->line = line;
                nextMac = malloc(sizeof(Macro));
                nextMac->name = macroName;
                macptr->nextLine = nextMac;
                macptr = nextMac;
                break;
            }
            /*Dont do anything*/
            default: {
                contextFlag = readingLine;
                break;
            }
        }
        /*If error encountered, print error and change the flag.*/
        if (contextFlag == errorPA) {
            fprintf(stderr,"Error encountered at line %d in file %s: ",lineCounter, fileName);
            /*REWRITE TO IF STATEMENTS*/
            switch (prevFlag) {
                case macroDefinitionStarted: {
                    fprintf(stderr,"Tried to define a macro with an illegal macro name.\n");
                    free(macroName);
                    break;
                }
                default: {
                    fprintf(stderr,"Tried to define a macro while the macro table is full.\n");
                    break;
                }
                contextFlag = skipMacroDefinition;
            }
            errorFlagPA = errorPA;
        }

        if(strcmp(firstWord, MACRO_END) != 0) contextFlag = readingLine;
        free(firstWord);
        if (contextFlag != macroDefinitionOngoing) free(line);
        /*Line memory allocation*/
        line = malloc(sizeof(char) * MAX_LINE_LENGTH);
        lineCounter++;
    }
    if (errorFlagPA == errorPA) {
        fclose(nfp);
        remove(newName);
        nfp = fp;
    }
    /*Add a method at the end of the pre assembler to free all of the macro items 
    and their lines*/
    free(line);
    free(newName);
    return nfp;
}
/*
Get the context of the line. The function checks for each relevent flags
if relevent parameters of the line are present. If they are the newFlag won't be an error
*/
PreassemblerFlags lineContext(char *line, char *firstWord, PreassemblerFlags currentFlag, int *indexOfMacro) {
    PreassemblerFlags newFlag;
    newFlag = errorPA;
    /*REWRITE TO IF STATEMENTS*/
    switch (currentFlag) {
        /*Reading a line*/
        case readingLine: {
            /*If firstWord is a macro definition*/
            if(!strcmp(firstWord,MACRO_DEF)) {
                if (macroTable.flag == hashTableFree)
                newFlag = macroDefinitionStarted;
            } else newFlag = checkForMacroCall(firstWord, indexOfMacro);
            break;
        }
        /*A macro definition has been started*/
        default: {
            /*If the macro definition ended*/
            if (!strcmp(firstWord,MACRO_END)) {
                newFlag = macroDefinitionEnded;
            } else newFlag = macroDefinitionOngoing;
            break;
        }

        return newFlag;
    }

}
/*More sophesticated method to check for Macro Call*/
PreassemblerFlags checkForMacroCall(char *word, int *indexOfMacro) {
    PreassemblerFlags newFlag;  
    if ((*indexOfMacro = lookUpTable(&macroTable, word)) == NOT_FOUND) newFlag = addLine;
    else if (isLabelLegal(word)) newFlag = skipUndefinedMacro;
    else newFlag = macroCall;
    
    return newFlag;
}
/*check if a label is not a macro*/

char *getMacroName(char *line) {
    char *name;
    int size;
    line = strstr(line, MACRO_DEF) + 4;
    name = readFirstWord(line);
    return name;
}

char *readFirstWord(char *line) {
    int size;
    char *word;
    word;
    while(isalpha(*line) == 0 && *line != '\0') {
        line++;
    }
    for(size = 0; isgraph(line[size]) != 0; size++);
    word = malloc(sizeof(char) * (size+1));
    word = strncpy(word, line, size);
    word[size] = '\0';
    /* If it found atleast one letter, it will find its size than will copy it to the pointer */
    
    return word;
}
