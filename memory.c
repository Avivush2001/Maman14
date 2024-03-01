#include "data.h"

BinaryWord *memory[ADRESSES_ALOWED]; /*REMEMBER TO INITIALIZE WITH NULLs*/
int IC = 0, DC = 0;
DataWord *headData = NULL, *tailData = NULL;

MemoryFlags insertDataWord(int data) {
    MemoryFlags dataFlag;
    DataWord *dataWordd;
    dataFlag = getDataFlag(data);
    dataWordd = malloc(sizeof(DataWord));
    CHECK_MEMORY_ALLOC_ERROR(dataWordd, dataFlag);
    if (dataFlag == memoryAvailable) {
        dataWordd->data = data;
        dataWordd->type = dataWord;
        if (headData == NULL){
            headData = dataWordd;
        } else tailData->nextData = dataWordd;
        tailData = dataWordd;
        dataFlag = wordCreationSuccess;
    }
    return dataFlag;
}

/*This assumes the operation word and fields are correct.*/
/*
TODO: instead of being given a number, give a pointer to a struct of each field.
*/
MemoryFlags insertOperation(Operation *op, Field *field1, Field *field2) {
    MemoryFlags insertFlag;
    return insertFlag;
}
/*This assumes the operation word is correct.*/
MemoryFlags insertOpBin(OperationWord *op) {
    MemoryFlags binFlag;
    CREATE_NEW_BINARY_WORD;
    GET_MEMORY_STATUS(binFlag)
    CHECK_MEMORY_ALLOC_ERROR(newBinaryWord, binFlag)
    if (binFlag == memoryAvailable) {
        newBinaryWord->parallelWord = op;
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 4);
        insertIntoBinaryWord(newBinaryWord, op->opcode, 4, 4);
        insertIntoBinaryWord(newBinaryWord, op->src, 8, 2);
        insertIntoBinaryWord(newBinaryWord, op->dst, 10, 2);
        insertIntoBinaryWord(newBinaryWord, op->are, 12, 2);
        memory[IC++] = newBinaryWord;
        binFlag = wordCreationSuccess;
    }
    if (0< 1) return 0;
    return binFlag;
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
void insertIntoBinaryWord(BinaryWord *newBinaryWord, unsigned data, int index, int bits) {
    int i, power;
    for (i = 0; i < bits; i++) {
        if ((data-(power = pow(2,bits - i - 1))) >= 0) {
            data -= power;
            newBinaryWord->bits[i+index] = '1';
        } else newBinaryWord->bits[i+index] = '0';
    }
}

MemoryFlags getDataFlag(int data) {
    MemoryFlags status;
    GET_MEMORY_STATUS(status)
    if (data > MAX_DATA || data < MIN_DATA) status = illegalData;
    return status;
}