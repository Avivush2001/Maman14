#include "data.h"

BinaryWord *memmory[ADRESSES_ALOWED];
int IC = 0, DC = 0;
DataWord *headData = NULL, *tailData = NULL;

MemmoryFlags insertDataWord(int data) {
    MemmoryFlags dataFlag;
    DataWord *dataWordd;
    dataFlag = getDataFlag(data);
    dataWordd = malloc(sizeof(DataWord));
    CHECK_MEMMORY_ALLOC_ERROR(dataWordd, dataFlag);
    if (dataFlag == memmoryAvailable) {
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
/*HANDLE CASE OF SYMBOL*/
MemmoryFlags insertOperation(OperationWord *op, int field1, int field2, int fields) {
    MemmoryFlags insertFlag;
    insertFlag = insertOpBin(op);
    if (insertFlag == wordCreationSuccess) {
        switch (fields) {
            case 1:
                break;
            case 2:
                break;
            default:
                break;
        }
    }
    return insertFlag;
}
/*This assumes the operation word is correct.*/
MemmoryFlags insertOpBin(OperationWord *op) {
    MemmoryFlags binFlag;
    CREATE_NEW_BINARY_WORD;
    GET_MEMMORY_STATUS(binFlag)
    CHECK_MEMMORY_ALLOC_ERROR(newBinaryWord, binFlag)
    if (binFlag == memmoryAvailable) {
        newBinaryWord->parallelWord = op;
        newBinaryWord->bits[WORD_LENGTH] = '\0';
        insertIntoBinaryWord(newBinaryWord, 0, 0, 4);
        insertIntoBinaryWord(newBinaryWord, op->opcode, 4, 4);
        insertIntoBinaryWord(newBinaryWord, op->src, 8, 2);
        insertIntoBinaryWord(newBinaryWord, op->dst, 10, 2);
        insertIntoBinaryWord(newBinaryWord, op->are, 12, 2);
        memmory[IC++] = newBinaryWord;
        binFlag = wordCreationSuccess;
    }
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

MemmoryFlags getDataFlag(int data) {
    MemmoryFlags status;
    GET_MEMMORY_STATUS(status)
    if (data > MAX_DATA || data < MIN_DATA) status = illegalData;
    return status;
}