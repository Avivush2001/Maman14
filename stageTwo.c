#include "data.h"


extern HashTable symbolHashTable, macroHashTable;
extern BinaryWord *memoryHead, *memoryTail;
extern int IC, DC;
static void encodeBinaryWordToFile(FILE *, char*);
StageTwoFlags updateMemory() {
    StageTwoFlags flag = updateSuccess;
    BinaryWord *p = memoryHead;
    Symbol *label;
    char *possibleLabel;
    int i;
    while (p != NULL) {
        possibleLabel = p->possibleLabel;
        if(possibleLabel == NULL) {
            p = p->nextWord;
            continue;
        }
        if((i = lookUpTable(&symbolHashTable, possibleLabel)) == NOT_FOUND || lookUpTable(&macroHashTable, possibleLabel) != NOT_FOUND) {
            flag = errorIllegalLabelST;
            break;
        } 
        label = symbolHashTable.items[i].item;
        if (label->attr == constant || label->attr == undefined) {
            flag = errorIllegalLabelST;
            break;
        }
        if (label->attr == external) {
            p = p->nextWord;
            continue;
        }
        insertIntoBinaryWord(p, label->value, 0, 12);
        p = p->nextWord;
    }
    return flag;
}

fileFlag createEntryFile(char *fileName)
{
    int i, counter = 0;
    char *newName = newFileName(fileName, ".ent");
    FILE *nfp;
    Symbol *symb;
    fileFlag flag = success;
    if(newName == NULL)
        flag = failure;
    else
    {
        OPEN_NEW_FILE
        if(nfp == NULL)
            flag = errorCreatingFile;
        else
        {
            for(i = 0; i < HASHSIZE; i++)
            {
                if((symb = symbolHashTable.items[i].item) != NULL)
                {
                    if(symb->entry)
                    {
                        fprintf(nfp, "%s\t%04d\n", symb->symbol, symb->value);
                        counter++;
                    }
                }
            }
            fclose(nfp);
            if(counter == 0)
            {
                i = remove(newName);
                if(i != 0)
                    flag = errorDeletingFile;
            }
        }
        free(newName);
    } 
    return flag;
}

fileFlag createExternFile(char *fileName)
{
    int i, counter = 100;
    char *newName =  newFileName(fileName, ".ext"), *possibleLabel;
    FILE *nfp;
    Symbol *label;
    fileFlag flag = success;
    BinaryWord *p = memoryHead;
    OPEN_NEW_FILE
    if(nfp == NULL)
        flag = errorCreatingFile;
    else
    {
        while (p != NULL) {
            possibleLabel = p->possibleLabel;
            if(possibleLabel == NULL) {
                p = p->nextWord;
                counter++;
                continue;
            }
            i = lookUpTable(&symbolHashTable, possibleLabel);
            label = symbolHashTable.items[i].item;
            if (label->attr == external) {
                fprintf(nfp, "%s\t%04d\n", possibleLabel, counter);
            }
            p = p->nextWord;
            counter++;
        }
        fclose(nfp);
        if(counter == 100)
        {
            i = remove(newName);
            if(i != 0)
                flag = errorDeletingFile;
        }
    }
    free(newName);
    return flag;
}

fileFlag createObFile(char *fileName) 
{
    int i = 100;
    char *newName = newFileName(fileName, ".ob");
    FILE *nfp;
    fileFlag flag = success;
    BinaryWord *p = memoryHead;
    if(newName == NULL)
        flag = failure;
    else
    {
        OPEN_NEW_FILE
        if(nfp == NULL)
            flag = errorCreatingFile;
        else 
        {
            fprintf(nfp, "\t%d %d\n", IC, DC);
            while (p != NULL) {
                fprintf(nfp, "%04d\t", i);
                encodeBinaryWordToFile(nfp, p->bits);
                p = p->nextWord;
                i++;
            }
            fclose(nfp);
        }
        free(newName);
    }
    return flag;
}

static void encodeBinaryWordToFile(FILE *nfp, char* bits) {
    int i, sum;
    for(i = 0; i < WORD_LENGTH; i+=2) {
        sum = 0;
        if (bits[i] == '1') {
            sum += 2;
        }
        if (bits[i+1] == '1') {
            sum += 1;
        }
        switch (sum) {
            case 0:
                putc('*',nfp);
                break;
            case 1:
                putc('#',nfp);
                break;
            case 2:
                putc('%',nfp);
                break;
            case 3:
                putc('!',nfp);
                break;
            default:
                break;
        }
    }
    putc('\n',nfp);
}
