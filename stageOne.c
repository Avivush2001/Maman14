#include "data.h"
extern HashTable macroHashTable;
extern HashTable symbolHashTable;
extern int IC, DC;
StageOneFlags stageOne(FILE *fp, char *fileName) {
    char line[MAX_LINE_LENGTH], possibleOperation[MAX_LABEL_SIZE] , possibleLabel[MAX_LABEL_SIZE+1];
    int lineCounter, opcode;
    FILE *nfp;
    Symbol *symp;
    StageOneFlags contextFlag, errorFlagSO; 
    DEFAULT_CONTEXT_SO
    errorFlagSO = allclearSO;
    OPEN_NEW_FILE
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL && errorFlagSO != errorEncounteredSO) {

    }
}