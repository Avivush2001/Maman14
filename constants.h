#define RAM_SIZE 4096
#define STARTING_ADRESS 100
#define ADRESSES_ALOWED RAM_SIZE-STARTING_ADRESS
#define MAX_LINE_LENGTH 81
#define MAX_LABEL_SIZE 32
#define WORD_LENGTH 14
#define NOT_FOUND -1
#define OPERATIONS_SIZE 16
#define REGISTERS_SIZE 8
#define INSTRUCTIONS_SIZE 6
#define MAX_DATA 8191
#define MIN_DATA -8192
#define MAX_CONST 2047
#define MIN_CONST -2048


#define MALLOC_LINE malloc(sizeof(char) * MAX_LINE_LENGTH)
#define MALLOC_LABEL malloc(sizeof(char) * MAX_LABEL_SIZE)
#define READ_FIRST_TWO_FIELDS sscanf(line, "%31s %31s", field1, field2)

#define PRINT_ERROR(_stage,_errorMessege) fprintf(stderr,"Error encountered during %s stage!\n", _stage);\
fprintf(stderr,"At line %d in file %s: %s",lineCounter, fileName, _errorMessege);

#define ERROR_CASE(_stage,_flag, _errorMessege) case _flag:\
PRINT_ERROR(_stage, _errorMessege);

#define PRINT_WARNING(_stage,_errorMessege) fprintf(stderr,"Warning encountered during %s stage!\n", _stage);\
fprintf(stderr,"At line %d in file %s: %s",lineCounter, fileName, _errorMessege);

#define WARNING_CASE(_stage,_flag, _errorMessege) case _flag:\
PRINT_WARNING(_stage, _errorMessege);
