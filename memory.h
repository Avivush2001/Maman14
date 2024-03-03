#define CHECK_MEMORY_ALLOC_ERROR(_word, _flag) if (_word == NULL) \
_flag = memoryAllocationError;
#define GET_MEMORY_STATUS(_flag) if ((IC + DC) >= ADDRESSES_ALLOWED) _flag = memoryFull; \
else _flag = memoryAvailable;



/*
The way I chose to handle the memory is as follows:
The struct that represents binary words are chars of ones and zeroes in length 14,
and they hold also hold a pointer to a parallel word.
The structs that represent those words are different for each word type:
Operations, Constants, labels, registers, data.
Each one of them has a type, and a series of unsigned ints, with different bit fields
to represent different things based on the type of the word.
In each word the bits adds up to 14.
The first stage functions "Converts" each line in the .am file to one of these
structs, and it is easier to do these words conversion in the first stage
instead of converting them straight to binary, which is what these functions
are for.
*/


/*Struct that represents an operation word.*/
typedef struct {
    unsigned opcode :4;
    unsigned src :2;
    unsigned dst :2;
 } OperationWord;
 
 /*Either a constant or an index of an array*/
 typedef struct {
    unsigned constant :12;
 } ConstWord;

 typedef struct {
    char *symbol;
    unsigned address :12;
 } AddressWord;

 typedef struct {
    unsigned src :3;
    unsigned dst :3;
 } RegisterWord;

 typedef struct DataWord {
    unsigned data :WORD_LENGTH;
    struct DataWord *nextData;
 } DataWord;

 typedef struct {
    char bits[WORD_LENGTH + 1];
    AddressWord * parallelWord;
 } BinaryWord;

 typedef enum {
   memoryFull,
   memoryAvailable,
   illegalData,
   wordCreationSuccess,
   memoryAllocationError
 } MemoryFlags;
/*
Flag that these function may return: illegalData, memoryFull, wordCreationSuccess, memoryAllocationError
*/
MemoryFlags insertOperation(int, Field *, Field *);

MemoryFlags insertDataWord(int);