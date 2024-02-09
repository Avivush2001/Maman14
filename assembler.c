#include "data.h"

char * operationsArr[] = {"mov", "cmp", "add", "sub", "not", "clr","lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};
char * registersArr[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char * instructionArr[] = {".data",".string", ".entry" ,".extern", MACRO_DEF, MACRO_END};
int main() {
    SymbolHashTable *macroHashTable;
    FILE *fp, *nfp;
    macroHashTable = malloc(sizeof(SymbolHashTable));
    fp = fopen("test1PA.as", "r");
    nfp = preassembler(fp, "test1PA.am", macroHashTable);
    /*while (fgets(buff, MAX_LINE_LENGTH, fp) != NULL) {
        fputs(buff, nfp);
        free(buff);
        buff = MALLOC_LINE;
    }*/
    /*free(buff);*/
    fclose(fp);
    fclose(nfp);
    freeTableNames(macroHashTable);
    
    return 0;
}

/*int main() {
    SymbolHashTable macroHashTable;
    SymbolHashTable *macroTable;

    char *line, *field1, *field2; 

    int indexOfMacro, lineCounter, stringCounter;

    PreassemblerFlags contextFlag, errorFlagPA; 

    Bool freeField2, freeLine;

    FILE *nfp, *fp;


    Macro *macptr, *nextMac;

    macroTable = &macroHashTable;
    fp = fopen("test1PA.as", "r");
    nfp = fopen("test1PA.am", "w");
    macroTable = malloc(sizeof(SymbolHashTable));

    DEFAULT_CONTEXT_PA;
    errorFlagPA = allclearPA;


    initializeHashTable(macroTable);

    line = MALLOC_LINE;

    lineCounter = 1;
    indexOfMacro = NOT_FOUND;
    

    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL) {

        field1 = MALLOC_LABEL;
        field2 = MALLOC_LABEL;
        stringCounter = READ_FIRST_TWO_FIELDS;
        freeField2 = True;
        freeLine = True;
        contextFlag = lineContext( contextFlag, field1, &indexOfMacro, macroTable);
        printf("%s %s %d %d\n", field1, field2, indexOfMacro, contextFlag);

        switch (contextFlag) {

            case addLine: {        
                
                DEFAULT_CONTEXT_PA;
                fputs(line,nfp);
                break;
            }

            case macroCall: {
                macptr = MACRO_AT_INDEX;
                while(macptr->nextLine != NULL) {
                    fputs(macptr->line, nfp);
                    macptr = macptr->nextLine;
                }
                DEFAULT_CONTEXT_PA;
                break;
            }

            case macroDefinitionStarted: {
                contextFlag = canDefineMacro(field2, stringCounter, macroTable);
                if (contextFlag == macroDefinitionStarted) {
                    contextFlag = macroDefinitionOngoing;
                    indexOfMacro = insertToTable(macroTable, field2);
                    macptr = malloc(sizeof(Macro));
                    macptr->name = field2;
                    MACRO_NAME_AT_INDEX= field2;
                    MACRO_AT_INDEX = macptr;
                    freeField2 = False;
                }
                break;
            }

            case macroDefinitionOngoing: {
                macptr->line = line;
                nextMac = malloc(sizeof(Macro));
                nextMac->name = macptr->name;
                macptr->nextLine = nextMac;
                macptr = nextMac;
                freeLine = False;
                break;
            }

            case skipMacroDefinition: break;
            
            default: {
                DEFAULT_CONTEXT_PA;
                break;
            }
        }

        errorFlagPA = errorHandler(&contextFlag, errorFlagPA,lineCounter, "test");

        free(field1);
        if (freeField2) free(field2);
        if (freeLine) free(line);

        line = MALLOC_LINE;
        lineCounter++;
        
    }
    free(line);
    macroTable = freeMacrosFromTable(macroTable);
    fclose(fp);
    fclose(nfp);
    freeTableNames(macroTable);
    return 0;
    
}*/