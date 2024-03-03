#define CHECK_MEMORY_ALLOC_ERROR(_word, _flag) if (_word == NULL) \
_flag = memoryAllocationError;
#define GET_MEMORY_STATUS(_flag) if ((IC + DC) >= ADDRESSES_ALLOWED) _flag = memoryFull; \
else _flag = memoryAvailable;
#define TYPE_OF_FIELD_1 field1->type
#define TYPE_OF_FIELD_2 field2->type
#define INSERT_FIELD(_field) switch (_field->type) {\
                case immediate:\
                    insertionFlag = insertConstBin(_field->value);\
                    break;\
                case direct:\
                    insertionFlag = insertAddressBin(_field->symbol);\
                    break;\
                case index:\
                    insertionFlag = insertAddressBin(_field->symbol);\
                    insertionFlag = insertConstBin(_field->value);\
                    break;\
                case reg:\
                    insertionFlag = insertRegisterBin(0,_field->value);\
                    break;\
                default:\
                    break;\
            }

 typedef struct BinaryWord{
   char bits[WORD_LENGTH + 1];
   char * possibleLabel;
   struct BinaryWord *nextData;
 } BinaryWord;

 typedef enum {
   memoryFull,
   memoryAvailable,
   wordCreationSuccess,
   memoryAllocationError
 } MemoryFlags;

/*
Flags that these functions may return: memoryFull, wordCreationSuccess, memoryAllocationError
*/

/* */
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
This function gets an integer (can be char, doesn't matter),
creates a binary word for it, adds it to the data linked list, 
and returns a flag.

Assumes the data is correct and in the set range.
*/
MemoryFlags insertDataWord(int);