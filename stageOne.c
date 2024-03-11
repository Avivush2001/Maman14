#include "data.h"



extern int IC = 0;
extern int DC = 0;
int L = 0;
char *flag;


















/* we are going to perform stage1 according to the algorithm described in the project instructions and save the symbols in a hash table*/

int stage1() 
{
    char *line;
    FILE *fp = fopen("test1PA.am", "r");
    if (fp == NULL)
    {
        printf("Error opening the assembly file.");
        fclose(fp);
    }
    else
    {
        SymbolHashTable *symbolTable = initializeHashTable(symbolTable);

        while(fgets(line, MAX_LINE_LENGTH, fp) && feof(fp) == 0)
        {
            
        }
        
    }
    
}


