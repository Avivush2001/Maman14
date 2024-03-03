#include "data.h"



BinaryWord *memory[ADDRESSES_ALLOWED]; /*REMEMBER TO INITIALIZE WITH NULLs*/
int IC = 0, DC = 0;
BinaryWord *headData = NULL, *tailData = NULL;



/*********TODO LIST*******/
/*
- initialization function to the memory.
- free all memory function
- add all data to the memory function
- add a function that gets the Label 
*/

/*Assumes the data is correct and in the set range*/
MemoryFlags insertDataWord(int data) {
    MemoryFlags dataFlag;
    BinaryWord *dataWordd;
    GET_MEMORY_STATUS(dataFlag)
    dataWordd = malloc(sizeof(BinaryWord));
    CHECK_MEMORY_ALLOC_ERROR(dataWordd, dataFlag);
    if (dataFlag == memoryAvailable) {
        insertIntoBinaryWord(dataWordd, data, 0, 14);
        if (headData == NULL){
            headData = dataWordd;
        } else tailData->nextData = dataWordd;
        tailData = dataWordd;
        dataFlag = wordCreationSuccess;
        DC++;
    }
    return dataFlag;
}

/*This function assumes the operation word and fields are correct.
Also note that it assumes that if the operation has just one field,
field1 should point to NULL. If the operation needs fields, both
fields should point to NULL */
MemoryFlags insertOperation(int indexOfOp, Field *field1, Field *field2) {
    MemoryFlags insertionFlag = memoryAvailable;
    insertionFlag = getInsertionFlag(field1,field2);
    if (insertionFlag == memoryAvailable){
        if (field2 == NULL)
            insertionFlag = insertOpBin(indexOfOp,0,0);
        else if (field1 == NULL) {
            insertionFlag = insertOpBin(indexOfOp,0,TYPE_OF_FIELD_2);
            INSERT_FIELD(field2)
        }
        else {
            insertionFlag = insertOpBin(indexOfOp,TYPE_OF_FIELD_1,TYPE_OF_FIELD_2);
            if ((TYPE_OF_FIELD_1 == reg) && (TYPE_OF_FIELD_2 == reg)) 
                insertionFlag = insertRegisterBin(field1->value,field2->value);
            else {
                INSERT_FIELD(field1)
                INSERT_FIELD(field2)
            }
        }      
    }
    return insertionFlag;
}


/*This assumes the operation word is correct.*/
MemoryFlags insertOpBin(int opcode, int src, int dst) {
    BinaryWord *newBinaryWord;
    MemoryFlags insertionFlag = memoryAvailable;
    newBinaryWord = malloc(sizeof(BinaryWord));
    CHECK_MEMORY_ALLOC_ERROR(newBinaryWord, insertionFlag)
    if (insertionFlag = memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 4);
        insertIntoBinaryWord(newBinaryWord, opcode, 4, 4);
        insertIntoBinaryWord(newBinaryWord, src, 8, 2);
        insertIntoBinaryWord(newBinaryWord, dst, 10, 2);
        insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
        newBinaryWord->possibleLabel = NULL;
        newBinaryWord->nextData = NULL;
        memory[IC++] = newBinaryWord;
        insertionFlag = wordCreationSuccess;
    }
    
    return insertionFlag;
}

MemoryFlags insertConstBin(unsigned co) {
    BinaryWord *newBinaryWord;
    MemoryFlags insertionFlag = memoryAvailable;
    newBinaryWord = malloc(sizeof(BinaryWord));
    CHECK_MEMORY_ALLOC_ERROR(newBinaryWord, insertionFlag)
    if (insertionFlag = memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, co, 0, 12);
        insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
        newBinaryWord->possibleLabel = NULL;
        newBinaryWord->nextData = NULL;
        memory[IC++] = newBinaryWord;
        insertionFlag = wordCreationSuccess;
    }
    return insertionFlag;
}

MemoryFlags insertAddressBin(char *symbol) {
    BinaryWord *newBinaryWord;
    MemoryFlags insertionFlag = memoryAvailable;
    newBinaryWord = malloc(sizeof(BinaryWord));
    CHECK_MEMORY_ALLOC_ERROR(newBinaryWord, insertionFlag)
    if (insertionFlag = memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 12);
        insertIntoBinaryWord(newBinaryWord, index, 12, 2);
        newBinaryWord->possibleLabel = symbol;
        newBinaryWord->nextData = NULL;
        memory[IC++] = newBinaryWord;
        insertionFlag = wordCreationSuccess;
    }
    return insertionFlag;
}

MemoryFlags insertRegisterBin(int reg1, int reg2) {
    BinaryWord *newBinaryWord;
    MemoryFlags insertionFlag = memoryAvailable;
    newBinaryWord = malloc(sizeof(BinaryWord));
    CHECK_MEMORY_ALLOC_ERROR(newBinaryWord, insertionFlag)
    if (insertionFlag = memoryAvailable) {
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 6);
        insertIntoBinaryWord(newBinaryWord, reg1, 6, 3);
        insertIntoBinaryWord(newBinaryWord, reg2, 9, 3);
        insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
        newBinaryWord->possibleLabel = NULL;
        newBinaryWord->nextData = NULL;
        memory[IC++] = newBinaryWord;
        insertionFlag = wordCreationSuccess;
    }
    return insertionFlag;
}
/*
The function gets the word, unsigned data, an index from which we will insert into the binary word,
and the number of bits of the unsigned data's bit field.
The algorithm to convert into a binary word is very simple:

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
        if ((data-(power = pow(2,bits - i - 1))) >= 0) {
            data -= power;
            newBinaryWord->bits[i+in] = '1';
        } else newBinaryWord->bits[i+in] = '0';
    }
}



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