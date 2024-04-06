#include "data.h"


/*Global Assembler Tables*/
extern HashTable symbolHashTable, macroHashTable;

/*No reason to create a header file for one function*/
extern Bool stageTwo(char *, Bool);

static void assembler(int, char**);

int main(int argc, char **argv) {
    char *originalTest[] = {"RUN_UNI_TEST", "originalTest/ps"};
    char *allTests[] = {"RUN_ALL_TESTS",
        "tests/errorFile",
        "tests/someWeirdStuff",
        "tests/original"
    };
    char *ogFileName;

    ogFileName = argv[argc-1];
    if (argc == 1)
        printf( "No arguments were given\n");
    else if (argc == 2) {
        if (!strcmp(ogFileName, originalTest[0]))
            assembler(argc, originalTest);
        else if (!strcmp(ogFileName, allTests[0]))
            assembler(10, allTests);
        else assembler(argc, argv);
    } else assembler(argc, argv);
    
    
    return 0;
}

/*
NOTE: The variable 'fileName' holds the name *with a suffix* and should be freed when not needed
anymore or before changing the suffix.

NOTE: If an error is encountered at any stage the function of that stage is responsible to print an error, and return
an error flag so the assembler function can move on.

Main assembler function. First it initializes the memory, the hash tables, a flag and 
the name of the current file (note it expects the name to be without a suffix). 

It then opens the .as file, and starts the pre assembler function, closes the .as file and opens the .am file,
starts the first stage and closes the .am file, starts the second stage, prints a message about the success of this iteration, 
and frees memory.
*/
static void assembler(int argc, char **argv) {
    FILE *fp;
    char *fileName, *ogFileName;
    Bool failFlag;
    while (argc != 1) {

        /*INITIALIZATIONS OF THE ASSEMBLER*/
        initializeMemory();
        initializeHashTable(&macroHashTable);
        initializeHashTable(&symbolHashTable);
        failFlag = True;
        ogFileName = argv[argc-1];
        printf("\n%s assembly started...\n", ogFileName);
        /*END OF INITIALIZATIONS*/

        /*PREASSEMBLER CALL*/
        fileName = newFileName(ogFileName, ".as");
        fp = fopen(fileName, "r");
        if(fp == NULL) {
            failFlag = False;
            printf("Failed to open as file for %s\n", fileName);
        }
        failFlag = preassembler(fp, ogFileName) == allclearPA;
        free(fileName);
        fclose(fp);
        /*END OF PREASSEMBLER CALL*/

        /*OPENING THE AM FILE*/
        fileName = newFileName(ogFileName, ".am");
        fp = fopen(fileName, "r");
        if(fp == NULL) {
            failFlag = False;
            printf("Failed to open am file for %s\n", fileName);
        }
        free(fileName);

        /*HANDLING FIRST AND SECOND STAGE*/
        if(failFlag) {
            /*FIRST STAGE CALL*/
            failFlag = stageOne(fp) == allclearSO;
            fclose(fp);
            /*END OF FIRST STAGE CALL*/

            failFlag = stageTwo(ogFileName, failFlag);/*SECOND STAGE CALL*/
        }
        
        
        /*END OF ITERATION AND STARTS FREEING RESOURCES*/
        if (failFlag)
            printf("%s successfully assembled! Continuing to next file...\n", ogFileName);
        else printf("%s failed assembly! Continuing to next file...\n", ogFileName);
        argc--;
        freeMemory();
        freeSymbols();
        freeTableNames(&macroHashTable);
        freeTableNames(&symbolHashTable);
    }
    printf("\nAssembler operation finished!\n");
}
