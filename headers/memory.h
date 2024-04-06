/*
    Header file for the memory module.
    The is implemented using 2 linked lists, one for operations and one
    for data, later being combined after the first stage.
    Each binary word in the linked list has a char array of bits,
    a pointer to a possible label so the stage 2 function knows which memory words
    to change (For later handling out of bound arrays, each binary word that isn't a label or an index of an array
    has a pointer to "0")
*/
#define CHECK_MEMORY_ALLOC_ERROR(_word, _flag) if (_word == NULL) \
_flag = memoryAllocationError;
#define GET_MEMORY_STATUS(_flag) if ((IC + DC) >= ADDRESSES_ALLOWED) _flag = memoryFull; \
else _flag = memoryAvailable;
#define TYPE_OF_FIELD_1 field1->type
#define TYPE_OF_FIELD_2 field2->type
#define INSERT_FIELD1 switch (field1->type) {\
                case immediate:\
                    insertionFlag = insertConstBin(field1->value, False);\
                    break;\
                case direct:\
                    insertionFlag = insertAddressBin(field1->symbol);\
                    break;\
                case index:\
                    insertionFlag = insertAddressBin(field1->symbol);\
                    insertionFlag = insertConstBin(field1->value, True);\
                    break;\
                case reg:\
                    insertionFlag = insertRegisterBin(field1->value,0);\
                    break;\
                default:\
                    break;\
            }
#define ADD_TO_MEMORY memoryTail->nextWord = newBinaryWord;\
        memoryTail = newBinaryWord;\
        IC++;
#define INIT_BINARY_INSERTION MemoryFlags insertionFlag;\
    BinaryWord *newBinaryWord;\
    GET_MEMORY_STATUS(insertionFlag)\
    if (insertionFlag == memoryAvailable){\
    newBinaryWord = malloc(sizeof(BinaryWord));\
    EXIT_IF(newBinaryWord == NULL)\
    }

/*Struct for binary words inserted in the memory*/
 typedef struct BinaryWord{
   char bits[WORD_LENGTH + 1];
   char * possibleLabel;
   struct BinaryWord *nextWord;
 } BinaryWord;

/*defines data struct for data insertions*/
 typedef struct {
    unsigned value : 14;
 }Data;

/*Memory flags*/
 typedef enum {
   memoryFull,
   memoryAvailable,
   wordCreationSuccess,
   memoryAllocationError
 } MemoryFlags;

/*Simple function to initialize the memory*/
void initializeMemory();

/*Simple function to free the memory*/
void freeMemory();

/*Simple function to add the data to the rest of the memory*/
void addDataToMemory();

/*
The function gets the index of the operation we are inserting (basically the opcode)
and 2 pointers to Field structs. The function returns a flag and adds the binary words of the operation
to the memory.

This function assumes the operation word and fields are correct.
Also note that it assumes that if the operation has just one field,
field1 should point to NULL. If the operation needs fields, both
fields should point to NULL.
*/
MemoryFlags insertOperation(int, Field *, Field *);

/*
This function gets a data,
creates a binary word for it, adds it to the data linked list, 
and returns a flag.

Assumes the data is correct and in the set range.
*/
MemoryFlags insertDataWord(Data *);



/*
The function gets the word, unsigned data, an index from which we will insert into the Binary word,
And the number of bits of the unsigned data's bit field.
*/
void insertIntoBinaryWord(BinaryWord *, unsigned, int, int);

/*Debugging function to print the memory*/
void printMemory();