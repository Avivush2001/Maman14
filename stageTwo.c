#include "data.h"


extern HashTable symbolHashTable;
extern BinaryWord *memoryHead, *memoryTail;
extern int IC, DC;
static void encodeBinaryWordToFile(FILE *, char*);

Bool stageTwo(char *fileName)
{
    Bool flag = updateMemory();
    if(flag)
        flag = createObFile(fileName);
    if(flag)
        flag = createEntryFile(fileName);
    if(flag)
        flag = createExternFile(fileName);
    freeSymbols();
    return flag;
}

Bool updateMemory() {
    Bool flag = True;
    BinaryWord *p = memoryHead;
    Symbol *label;
    char *possibleLabel;
    int i, lineCounter = 1;
    while (p != NULL) {
        possibleLabel = p->possibleLabel;
        if(possibleLabel == NULL) {
            p = p->nextWord;
            lineCounter++;
            continue;
        }
        if((i = lookUpTable(&symbolHashTable, possibleLabel)) == NOT_FOUND) {
            flag = False;
            PRINT_ERROR("Second", "Unknown Label\n")
            p = p->nextWord;
            continue;
        } 
        label = symbolHashTable.items[i].item;
        if (label->attr == constant || label->attr == undefined) {
            flag = False;
            PRINT_ERROR("Second", "Undefined Entry Label, or used a constant\n")
            p = p->nextWord;
            continue;
        }
        if (label->attr == external) {
            p = p->nextWord;
            lineCounter++;
            continue;
        }
        insertIntoBinaryWord(p, label->value, 0, 12);
        p = p->nextWord;
        lineCounter++;
    }
    return flag;
}

Bool createEntryFile(char *fileName)
{
    int i, counter = 0;
    char *newName = newFileName(fileName, ".ent");
    FILE *nfp;
    Symbol *symb;
    Bool flag = True;
    OPEN_NEW_FILE
    if(nfp == NULL) {
        flag = False;
        fprintf(stderr,"Failed to create entry file for %s\n", fileName);
    }
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
        if(!counter)
        {
            if(remove(newName)) 
                fprintf(stderr,"Failed to delete empty entry file for %s\n", fileName);
            
        }
    }
    free(newName);
    
    return flag;
}

Bool createExternFile(char *fileName)
{
    int i, counter = 100;
    char *newName =  newFileName(fileName, ".ext"), *possibleLabel;
    FILE *nfp;
    Symbol *label;
    Bool flag = True;
    BinaryWord *p = memoryHead;
    OPEN_NEW_FILE
    if(nfp == NULL) {
        flag = False;
        fprintf(stderr,"Failed to create extern file for %s\n", fileName);
    }
    else
    {
        while (p != NULL) 
        {
            possibleLabel = p->possibleLabel;
            if(possibleLabel == NULL) 
            {
                p = p->nextWord;
                counter++;
                continue;
            }
            i = lookUpTable(&symbolHashTable, possibleLabel);
            label = symbolHashTable.items[i].item;
            if (label->attr == external) 
                fprintf(nfp, "%s\t%04d\n", possibleLabel, counter);
            p = p->nextWord;
            counter++;
        }
        fclose(nfp);
        if(counter == 100)
        {
            if(remove(newName)) 
                fprintf(stderr,"Failed to delete empty extern file for %s\n", fileName);
            
        }
    }
    free(newName);     
    
    return flag;
}

Bool createObFile(char *fileName) 
{
    int i = 100;
    char *newName = newFileName(fileName, ".ob");
    FILE *nfp;
    Bool flag = True;
    BinaryWord *p = memoryHead;
    OPEN_NEW_FILE
    if(nfp == NULL) {
        flag = False;
        fprintf(stderr,"Failed to create ob file for %s\n", fileName);
    } else {
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
