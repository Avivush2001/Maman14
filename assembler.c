#include "data.h"


/*Global Assembler Tables*/
extern HashTable symbolHashTable, macroHashTable;

/* int main() {
    
    FILE *fp;
    PreassemblerFlags flagPA;
    initializeHashTable(&macroHashTable);
    initializeHashTable(&symbolHashTable);
    fp = fopen("testOriginal.as", "r");
    flagPA = preassembler(fp, "testOriginal");
    if (flagPA == errorEncounteredPA) {
        remove("testOriginal.am");
    }
    fclose(fp);
    fp = fopen("testOriginal.am", "r");
    stageOne(fp);
    fclose(fp);
    freeTableNames(&macroHashTable);
    freeTableNames(&symbolHashTable);
    return 0;
} */
int main(int argc, char **argv) {
    FILE *fp;
    char *fileName, *ogFileName;
    Bool continueFlag;
    if (argc == 1) {
        fprintf(stderr, "No arguments were given\n");
        exit(1);
    }
    while (argc != 1) {
        initializeMemory();
        continueFlag = True;
        ogFileName = argv[argc-1];
        initializeHashTable(&macroHashTable);
        initializeHashTable(&symbolHashTable);
        printf("%s assembly started...\n", ogFileName);
        fileName = newFileName(ogFileName, ".as");
        fp = fopen(fileName, "r");
        CHECK_CONTINUE((preassembler(fp, ogFileName) != allclearPA))
        free(fileName);
        fclose(fp);

        if(continueFlag) {
            fileName = newFileName(ogFileName, ".am");
            fp = fopen(fileName, "r");
            free(fileName);
            CHECK_CONTINUE((stageOne(fp) != allclearSO))
            fclose(fp);
        }
        if (continueFlag) {
            CHECK_CONTINUE((!stageTwo(ogFileName)))
        }
        if (continueFlag)
            printf("%s successfully assembled! Continuing to next file...\n", ogFileName);
        else printf("%s failed assembly! Continuing to next file...\n", ogFileName);
        argc--;
        freeSymbols();
        freeTableNames(&macroHashTable);
        freeTableNames(&symbolHashTable);
    }
    printf("Assembler operation finished!\n");
    return 0;
}


