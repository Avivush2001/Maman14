#include "data.h"
#define MACRO_DEF "mcr"
#define MACRO_END "endmcr"

States preAssemblerState = waitingForFile;

FILE *preassembler(FILE*);
States lookForMacro(char*,int*);

void foundMacro(char*);

FILE *preassembler(FILE *fp) {
    char *line;
    line = malloc(sizeof(char) * MAX_LINE_LENGTH);
    while((line = fgets(line, MAX_LINE_LENGTH, fp)) != EOF) {
        preAssemblerState = readingLine;

    }
}

States lookForMacro(char *line,int *index) {
    char *firstWord;
    firstWord = readFirstWord(line, index);
    if (firstWord == line) return emptyLine;
    if (strcmp(firstWord,MACRO_DEF) == False) return foundMacroDefinition;
    if (strcmp(firstWord,MACRO_END) == False) return macroDefinitionEnded;
    /*
        ADD A COMPARISON WITH THE MACRO TABLE HERE
    */
}

char *readFirstWord(char *line, int *index) {
    int i, size;
    char *word;
    i = 0;
    while(isalpha(*line) == 0) {
        line++;
        i++;
    }
    if(*line == '\0') return line; /*Handle case of empty line*/
    /* If it found atleast one letter, it will find its size than will copy it to the pointer */
    for(size = 0; isgraph(line[size]) != 0; size++);
    word = malloc(sizeof(char) * (size+1));
    word = strncpy(word, line, size);
    word[size] = '\0';
    *index = i;
    return word;
}