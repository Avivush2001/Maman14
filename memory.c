#include "data.h"


/*The memory is represented by a linked list, mainly to save on storage.*/
BinaryWord *memoryHead = NULL, *memoryTail = NULL;
/*Data words are also in a linked list, and added to the rest of the memory at the end of stage 2, before the .ob file creation.*/
BinaryWord *headData = NULL, *tailData = NULL;
/*The IC and DC counters*/
int IC = 0, DC = 0;

/******************Memory Management Functions***************/

/*Simple function to initialize the memory*/
void initializeMemory(){
    IC = 0;
    DC = 0;
    headData = NULL; 
    tailData = NULL;
    memoryHead = NULL;
    memoryTail = NULL;
}

/*Simple function to free the memory*/
void freeMemory() {
    BinaryWord *mp = memoryHead;
    while ((mp) != NULL) {
        memoryHead = memoryHead->nextWord;
        free(mp);
        mp = memoryHead;
    }
    initializeMemory();
}

/*Simple function to add the data to the rest of the memory*/
void addDataToMemory() {
    memoryTail->nextWord = headData;
    memoryTail = tailData;
}

/******************Memory Insertion Functions***************/

/*
Important note about all of the functions in this section:
They all return a memory flag, if the flag given is an error
of some sort, it is safe to assume that no "clean up" needed in the memory, but those errors
should be handled by the first stage of the assembler.
*/

/*
This function gets data, turns it into a binary word and adds it to the data linked list.
The function assumes the data inserted is correct, and in the set range.
*/
MemoryFlags insertDataWord(Data *dataStruc) {
    MemoryFlags dataFlag;
    BinaryWord *dataWordd;
    GET_MEMORY_STATUS(dataFlag)
    dataWordd = malloc(sizeof(BinaryWord));
    CHECK_MEMORY_ALLOC_ERROR(dataWordd, dataFlag);
    if (dataFlag == memoryAvailable) {
        dataWordd->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(dataWordd, dataStruc->value, 0, 14);
        if (headData == NULL){
            headData = dataWordd;
        } else tailData->nextWord = dataWordd;
        tailData = dataWordd;
        dataFlag = wordCreationSuccess;
        dataWordd->nextWord = NULL;
        dataWordd->possibleLabel = NULL;
        DC++;
    }
    return dataFlag;
}

/*
The function is to be used like this: during code processing in the first stage,
for each field in the line of code create a field struct. (doesn't need to be allocated, can be initialized at the start of the function since we won't
need to reuse it)
Give this function the appropriate fields, they can be NULL, but the function of fields assumes the number of non NULL pointers is correct for the opcode given.

Note that insertionFlag will equal each insertion function call, to catch cases malloc failed. If one of them failed the rest will fail either way if one malloc call failed.
*/
MemoryFlags insertOperation(int opcode, Field *field1, Field *field2) {
    MemoryFlags insertionFlag = memoryAvailable;

    /*Based on the 2 fields, check if there is enough memory to insert them.*/
    insertionFlag = getInsertionFlag(field1,field2);
    if (insertionFlag == memoryAvailable){

        /*In case field2 is NULL the function handles this as an operation without any fields.*/
        if (field2 == NULL)
            insertionFlag = insertOpBin(opcode,0,0);

        /*In case field1 is NULL the function handles this as an operation just one field.*/
        else if (field1 == NULL) {
            insertionFlag = insertOpBin(opcode,0,TYPE_OF_FIELD_2);
            switch (field2->type) {
                case immediate:
                    insertionFlag = insertConstBin(field2->value);\
                    break;
                case direct:
                    insertionFlag = insertAddressBin(field2->symbol);\
                    break;
                case index:
                    insertionFlag = insertAddressBin(field2->symbol);\
                    insertionFlag = insertConstBin(field2->value);\
                    break;
                case reg:
                    insertionFlag = insertRegisterBin(field2->value,0);\
                    break;
                default:
                    break;
            }
        }

        /*None is NULL*/
        else {
            insertionFlag = insertOpBin(opcode,TYPE_OF_FIELD_1,TYPE_OF_FIELD_2);

            /*Handle the case of two addressing types equal to reg*/
            if ((TYPE_OF_FIELD_1 == reg) && (TYPE_OF_FIELD_2 == reg)) 
                insertionFlag = insertRegisterBin(field1->value,field2->value);
            else {
                INSERT_FIELD1
                switch (field2->type) {
                case immediate:
                    insertionFlag = insertConstBin(field2->value);\
                    break;
                case direct:
                    insertionFlag = insertAddressBin(field2->symbol);\
                    break;
                case index:
                    insertionFlag = insertAddressBin(field2->symbol);\
                    insertionFlag = insertConstBin(field2->value);\
                    break;
                case reg:
                    insertionFlag = insertRegisterBin(0,field2->value);\
                    break;
                default:
                    break;
            }
            }
        }      
    }
    return insertionFlag;
}

/*
These next 4 functions Work basically the same. The needed to be different to handle different types of input.
The initialize a new binary word and the flag, set the last char in the binary word char array to \0, set
the rest of the bits are set as needed for each type of word inserted (An operation, a constant, an address and a register).
Nullify the possibleLabel (unless it is an address word, for that it will save the pointer given) and the nextWord pointers, and add it to the linked list.
*/

MemoryFlags insertOpBin(int opcode, int src, int dst) {
    INIT_BINARY_INSERTION
    if (insertionFlag == memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 4);
        insertIntoBinaryWord(newBinaryWord, opcode, 4, 4);
        insertIntoBinaryWord(newBinaryWord, src, 8, 2);
        insertIntoBinaryWord(newBinaryWord, dst, 10, 2);
        insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
        newBinaryWord->possibleLabel = NULL;
        newBinaryWord->nextWord = NULL;
        if (memoryHead == NULL){
            memoryHead = newBinaryWord;
        } else memoryTail->nextWord = newBinaryWord;
        memoryTail = newBinaryWord;
        IC++;
        insertionFlag = wordCreationSuccess;
    }
    
    return insertionFlag;
}

MemoryFlags insertConstBin(unsigned co) {
    INIT_BINARY_INSERTION
    if (insertionFlag == memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, co, 0, 12);
        insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
        newBinaryWord->possibleLabel = NULL;
        newBinaryWord->nextWord = NULL;
        ADD_TO_MEMORY
        insertionFlag = wordCreationSuccess;
    }
    return insertionFlag;
}

MemoryFlags insertAddressBin(char *symbol) {
    INIT_BINARY_INSERTION
    if (insertionFlag == memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 12);
        insertIntoBinaryWord(newBinaryWord, index, 12, 2);
        newBinaryWord->possibleLabel = symbol;
        newBinaryWord->nextWord = NULL;
        ADD_TO_MEMORY
        insertionFlag = wordCreationSuccess;
    }
    return insertionFlag;
}

MemoryFlags insertRegisterBin(int reg1, int reg2) {
    INIT_BINARY_INSERTION
    if (insertionFlag == memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 6);
        insertIntoBinaryWord(newBinaryWord, reg1, 6, 3);
        insertIntoBinaryWord(newBinaryWord, reg2, 9, 3);
        insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
        newBinaryWord->possibleLabel = NULL;
        newBinaryWord->nextWord = NULL;
        ADD_TO_MEMORY
        insertionFlag = wordCreationSuccess;
    }
    return insertionFlag;
}

/******************Memory Utility Functions***************/

/*
The function gets the word, unsigned data, an index from which we will insert into the BinarY word,
And the number of bits of the unsigned data's bit field.
The Algorithm to conVert into a binary word Is Very simple:

*note the algorithm only works if the number is positive, and since data is unsigned
it will be considered as such.

Take the number, check if number-2^i is bigger or equals to 0.
if it is, place '1' in bit i and number equals the result, else place '0' in bit i.

For example lets say data is initialized with -7, i.e. data = -7, but since its unsigned it actually is 9 (when the bit
field of data is 4)
so we perform:  9-8 = 1 bits[0] = '1'
                1-4 = -3 bits[1] = '0'
                1-2 = -1 bits[2] = '0'
                1-1 = 0 bits[3] = '1'
which is -7 in two's compliment with 4 bit.
*/
void insertIntoBinaryWord(BinaryWord *newBinaryWord, unsigned data, int in, int bits) {
    int i, power;
    for (i = 0; i < bits; i++) {
        power = pow(2,bits - i - 1);
        if ((int)(data-power) >= 0) {
            data -= power;
            newBinaryWord->bits[i+in] = '1';
        } else newBinaryWord->bits[i+in] = '0';
    }
}


/*
Checks if we have enough memory to perform an operation insertion.
*/
MemoryFlags getInsertionFlag(Field *field1, Field *field2) {
    MemoryFlags status;
    int availableMemory, memoryNeeded = 1, fields = 2;
    availableMemory = ADDRESSES_ALLOWED - (IC + DC);
    GET_MEMORY_STATUS(status)
    if(field1 == NULL) fields = 1;
    if(field2 == NULL) fields = 0;
    switch (fields) {
        case 1:
            if (TYPE_OF_FIELD_2 == index) memoryNeeded+=2;
            else memoryNeeded++;
            break;
        case 2:
            if ((TYPE_OF_FIELD_1 == reg) && (TYPE_OF_FIELD_2 == reg)) memoryNeeded++;
            else {
                if (TYPE_OF_FIELD_1 == index) memoryNeeded+=2;
                else memoryNeeded++;
                if (TYPE_OF_FIELD_2 == index) memoryNeeded+=2;
                else memoryNeeded++;
            }
        default:
            break;
    }
    if (memoryNeeded > availableMemory) status = memoryFull;
    return status;
}
/*
DEBUGGING FUNCTION
*/
void printMemory() {
    int i = 100;
    BinaryWord *p = memoryHead;
    while (p != NULL) {
        printf("%d %s %s\n",i++, p->bits, p->possibleLabel);
        p = p->nextWord;
    }
}