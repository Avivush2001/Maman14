#define CHECK_MEMMORY_ALLOC_ERROR(_word, _flag) if (_word == NULL) \
_flag = memmoryAllocationError;
#define CREATE_NEW_BINARY_WORD BinaryWord *newBinaryWord = malloc(sizeof(BinaryWord))
#define GET_MEMMORY_STATUS(_flag) if ((IC + DC) >= RAM_SIZE) _flag = memmoryFull; \
else _flag = memmoryAvailable;



/*
The way I chose to handle the memmory is as follows:
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
    Addressing type;
    unsigned unused :4;
    unsigned opcode :4;
    unsigned src :2;
    unsigned dst :2;
    unsigned are :2;
 } OperationWord;
 
 /*Either a constant or an index of an array*/
 typedef struct {
    Addressing type;
    unsigned constant :12;
    unsigned are :2;
 } ConstWord;

 typedef struct {
    Addressing type;
    char *symbol;
    unsigned address :12;
    unsigned are :2;
 } AddressWord;

 typedef struct {
    Addressing type;
    unsigned unused :6;
    unsigned src :3;
    unsigned dst :3;
    unsigned are :2;
 } RegisterWord;

 typedef struct {
    Addressing type;
    unsigned data :WORD_LENGTH;
    DataWord *nextData;
 } DataWord;

 typedef struct {
    char bits[WORD_LENGTH + 1];
    void * parallelWord;
 } BinaryWord;

 typedef enum {
   memmoryFull,
   memmoryAvailable,
   illegalData,
   wordCreationSuccess,
   memmoryAllocationError
 } MemmoryFlags;

