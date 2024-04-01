#define RAM_SIZE 4096
#define STARTING_ADDRESS 100
#define ADDRESSES_ALLOWED RAM_SIZE-STARTING_ADDRESS
#define MAX_LINE_LENGTH 81
#define MAX_LABEL_SIZE 32
#define WORD_LENGTH 14
#define NOT_FOUND -1
#define OPERATIONS_SIZE 16
#define REGISTERS_SIZE 8
#define INSTRUCTIONS_SIZE 7
#define MAX_DATA 8191
#define MIN_DATA -8192
#define MAX_CONST 2047
#define MIN_CONST -2048
#define A 0
#define R 2
#define E 1
#define OPEN_NEW_FILE nfp = fopen(newName, "w");



#define CHECK_CONTINUE(_condition) if(continueFlag && _condition) continueFlag = False;

#define EXIT_IF(_bool) if (_bool) exit(EXIT_FAILURE);
#define MALLOC_LINE malloc(sizeof(char) * MAX_LINE_LENGTH)
#define MALLOC_LABEL malloc(sizeof(char) * MAX_LABEL_SIZE)
#define READ_FIRST_TWO_FIELDS sscanf(line, "%31s %31s", str1, str2)

#define PRINT_ERROR(_stage,_errorMessage) printf("Error encountered during %s stage!\n", _stage);\
printf("At %d: %s",lineCounter, _errorMessage);

#define ERROR_CASE(_stage,_flag, _errorMessage) case _flag:\
PRINT_ERROR(_stage, _errorMessage);

#define PRINT_WARNING(_stage,_errorMessage) printf("Warning encountered during %s stage!\n", _stage);\
printf("At line %d in file %s: %s",lineCounter, fileName, _errorMessage);

#define WARNING_CASE(_stage,_flag, _errorMessage) case _flag:\
PRINT_WARNING(_stage, _errorMessage);
