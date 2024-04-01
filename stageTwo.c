#include "data.h"


extern HashTable symbolHashTable;
extern BinaryWord *memoryHead, *memoryTail;
extern int IC, DC;

/*Main Stage Two Function*/
Bool stageTwo(char *);
/*Encodes a binary word and inserts it in the OB file.*/
static void encodeBinaryWordToFile(FILE *, char*);

/*Updates the symbol hash table and checks for undefined labels.*/
static Bool updateLabels();

/*Create an entries file.*/
static Bool createEntryFile(char *);

/*Create an externals file.*/
static Bool createExternFile(char *);

/*Create the ob file.*/
static Bool createObFile (char *);

/*
Main stage two function, pretty un-complex
compared to the other main function. It just
starts each step and makes sure each step passed.
*/
Bool stageTwo(char *fileName)
{
    Bool flag = updateLabels();
    if(flag)
        flag = createObFile(fileName);
    if(flag)
        flag = createEntryFile(fileName);
    if(flag)
        flag = createExternFile(fileName);
    return flag;
}

/*
It iterates through each memory word, and if a binary word has a label, add its value in the table.
If the symbol stored in the word isn't in the table, or is either a constant / it is undefined (entry label left undefined),
it also updates memory words to external
it will print an error, change the flag to false, but it will continue to scan the rest of the memory.
*/
static Bool updateLabels() {
    Bool flag = True;
    BinaryWord *p = memoryHead;
    Symbol *label;
    char *possibleLabel;
    int i, lineCounter = 100; /*Note it is named lineCounter for the macro.*/
    while (p != NULL) {
        possibleLabel = p->possibleLabel;
        if(possibleLabel == NULL) {
            p = p->nextWord;
            lineCounter++;
            continue;
        }
        if((i = lookUpTable(&symbolHashTable, possibleLabel)) == NOT_FOUND) {
            flag = False;
            PRINT_ERROR("Second", "Unknown Label at this address!\n")
            p = p->nextWord;
            lineCounter++;
            continue;
        } 
        label = symbolHashTable.items[i].item;
        if (label->attr == constant || label->attr == undefined) {
            flag = False;
            PRINT_ERROR("Second", "Undefined Entry Label, or used a constant at this address!\n")
            p = p->nextWord;
            lineCounter++;
            continue;
        }
        if (label->attr == external) {
            insertIntoBinaryWord(p, E, 12, 2); /*Change the ARE of the binary word to E*/
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

/*
It creates the new file, and iterates through the symbol hash table to look for external labels.
If found it will add it to the file with its value.
*/
static Bool createEntryFile(char *fileName)
{
    int i, counter = 0;
    char *newName = newFileName(fileName, ".ent");
    FILE *nfp;
    Symbol *symb;
    Bool flag = True;
    OPEN_NEW_FILE
    if(nfp == NULL) {
        flag = False;
        printf("Failed to create entry file for %s\n", fileName);
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
        if(!counter) /*there are no entry labels so delete the entry file as it's not needed */
        {
            if(remove(newName)) 
                printf("Failed to delete empty entry file for %s\n", fileName);
            
        }
    }
    free(newName);
    
    return flag;
}

/*
It creates the new file, and iterates through the memory to look for external labels.
If found it will add it to the file with the correct address.
*/
static Bool createExternFile(char *fileName)
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
        printf("Failed to create extern file for %s\n", fileName);
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
        if(counter == 100) /*there are no extern labels so delete the extern file as it's not needed */
        {
            if(remove(newName)) 
                printf("Failed to delete empty extern file for %s\n", fileName);
            
        }
    }
    free(newName);     
    
    return flag;
}

/*
It creates the new file, and encodes each binary word in the memory.
*/
static Bool createObFile(char *fileName) 
{
    int i = 100;
    char *newName = newFileName(fileName, ".ob");
    FILE *nfp;
    Bool flag = True;
    BinaryWord *p = memoryHead;
    OPEN_NEW_FILE
    if(nfp == NULL) {
        flag = False;
        printf("Failed to create ob file for %s\n", fileName);
    } else {
        fprintf(nfp, "\t%d %d\n", IC, DC); /*Header for the ob file*/
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

/*
First the sum is set to 0. It checks the characters in intervals of two characters,
the first character representing 2^1 and the second 2^0. It converts it to
an integer in the range of 0-3, and is stored in the sum. Then a switch case puts the char in the file
and in the end it inserts a new line.
*/
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
