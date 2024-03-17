#include "data.h"

extern HashTable macroHashTable;
extern HashTable symbolHashTable;
extern int IC, DC;
extern char * registersArr[], * instructionArr[];
extern Operation operationsArr[];

void CreateFiles(char *fileName)
{

}

fileFlag CreateEntryFile(char *fileName)
{
    int i, counter = 0;
    char *newName[strlen(fileName) + 4];
    FILE *fp;
    Symbol *symb;
    fileFlag flag = success;
    strcpy(newName, fileName);
    strcat(newName, ".ent");
    fp = fopen(newName, "w");
    if(fp == NULL)
        flag = errorCreatingFile;
    else
    {
        for(i = 0; i < HASHSIZE; i++)
        {
            if((symb = symbolHashTable.items[i].item) != NULL)
            {
                if(symb->entry == True)
                {
                    fprintf(fp, "%s\t%04d\n", symb->symbol, symb->value);
                    counter++;
                }
            }
        }
        fclose(fp);
        if(counter == 0)
        {
            i = remove(newName);
            if(i != 0)
                flag = errorDeletingFile;
        }
    }
    return flag;
}

fileFlag CreateExternFile(char *fileName)
{
    int i, counter = 0;
    char *newName[strlen(fileName) + 4];
    FILE *fp;
    Symbol *symb;
    fileFlag flag = success;
    strcpy(newName, fileName);
    strcat(newName, ".ext");
    fp = fopen(newName, "w");
    if(fp == NULL)
        flag = errorCreatingFile;
    else
    {
        for(i = 0; i < HASHSIZE; i++)
        {
            if((symb = symbolHashTable.items[i].item) != NULL)
            {
                if(symb->attr == external)
                {
                    fprintf(fp, "%s\t%04d\n", symb->symbol, symb->value);
                    counter++;
                }
            }
        }
        fclose(fp);
        if(counter == 0)
        {
            i = remove(newName);
            if(i != 0)
                flag = errorDeletingFile;
        }
    }
    return flag;
}