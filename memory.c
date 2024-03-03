#include "data.h"

BinaryWord *memory[ADDRESSES_ALLOWED]; /*REMEMBER TO INITIALIZE WITH NULLs*/
int IC = 0, DC = 0;
DataWord *headData = NULL, *tailData = NULL;
extern Operation operationsArr[];

MemoryFlags insertDataWord(int data) {
    MemoryFlags dataFlag;
    DataWord *dataWordd;
    dataFlag = getDataFlag(data);
    dataWordd = malloc(sizeof(DataWord));
    CHECK_MEMORY_ALLOC_ERROR(dataWordd, dataFlag);
    if (dataFlag == memoryAvailable) {
        dataWordd->data = data;
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
MemoryFlags insertOperation(int indexOfOp, Field *field1, Field *field2) {
    Addressing typeOfField1 = field1->type, typeOfField2 = field2->type;
    MemoryFlags insertionFlag;
    BinaryWord *newBinaryWord;
    OperationWord *newOperation;
    AddressWord *addr;
    RegisterWord regis = {field1->value, field2->value};
    int availableMemory, memoryNeeded = 1;
    availableMemory = ADDRESSES_ALLOWED - (IC + DC);
    newBinaryWord = malloc(sizeof(BinaryWord));
    GET_MEMORY_STATUS(insertionFlag)
    CHECK_MEMORY_ALLOC_ERROR(newBinaryWord, insertionFlag)
    if ((typeOfField1 == reg) && (typeOfField2 == reg)) memoryNeeded++;
    else {
        if (typeOfField1 == index) memoryNeeded+=2;
        else memoryNeeded++;
        if (typeOfField2 == index) memoryNeeded+=2;
        else memoryNeeded++;
    }

    return insertionFlag;
}


/*This assumes the operation word is correct.*/
MemoryFlags insertOpBin(OperationWord *op, BinaryWord *newBinaryWord) {
    newBinaryWord->parallelWord = op;
    newBinaryWord->bits[WORD_LENGTH] = '\0';
    insertIntoBinaryWord(newBinaryWord, 0, 0, 4);
    insertIntoBinaryWord(newBinaryWord, op->opcode, 4, 4);
    insertIntoBinaryWord(newBinaryWord, op->src, 8, 2);
    insertIntoBinaryWord(newBinaryWord, op->dst, 10, 2);
    insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
    memory[IC++] = newBinaryWord;
    return wordCreationSuccess;
}

MemoryFlags insertConstBin(unsigned co, BinaryWord *newBinaryWord) {
    newBinaryWord->parallelWord = co;
    newBinaryWord->bits[WORD_LENGTH] = '\0';
    insertIntoBinaryWord(newBinaryWord, co, 0, 12);
    insertIntoBinaryWord(newBinaryWord, immediate, 12, 2);
    memory[IC++] = newBinaryWord;
    return wordCreationSuccess;
}

MemoryFlags insertAddressBin(AddressWord *addr, BinaryWord *newBinaryWord) {
    newBinaryWord->parallelWord = addr;
    newBinaryWord->bits[WORD_LENGTH] = '\0';
    insertIntoBinaryWord(newBinaryWord, 0, 0, 12);
    insertIntoBinaryWord(newBinaryWord, index, 12, 2);
    memory[IC++] = newBinaryWord;
    return wordCreationSuccess;
}

MemoryFlags insertRegisterBin(RegisterWord *regis, BinaryWord *newBinaryWord) {
    newBinaryWord->parallelWord = regis;
    newBinaryWord->bits[WORD_LENGTH] = '\0';
    insertIntoBinaryWord(newBinaryWord, 0, 0, 6);
    insertIntoBinaryWord(newBinaryWord, regis->src, 6, 3);
    insertIntoBinaryWord(newBinaryWord, regis->dst, 9, 3);
    insertIntoBinaryWord(newBinaryWord, reg, 12, 2);
    memory[IC++] = newBinaryWord;
    return wordCreationSuccess;
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

MemoryFlags getDataFlag(int data) {
    MemoryFlags status;
    GET_MEMORY_STATUS(status)
    if (data > MAX_DATA || data < MIN_DATA) status = illegalData;
    return status;
}