#define RAM_SIZE 4096
#define MAX_LINE_LENGTH 81
#define MAX_LABEL_SIZE 32
#define NOT_FOUND -1
#define OPERATIONS_SIZE 16
#define REGISTERS_SIZE 8
#define INSTRUCTIONS_SIZE 6
#define MALLOC_LINE malloc(sizeof(char) * MAX_LINE_LENGTH)
#define MALLOC_LABEL malloc(sizeof(char) * MAX_LABEL_SIZE)
#define READ_FIRST_TWO_FIELDS sscanf(line, "%31s %31s", field1, field2)
#define PRINT_ERROR(_stage,_errorMessege) fprintf(stderr,"Error encountered during %s stage!\n", _stage);\
fprintf(stderr,"At line %d in file %s: %s",lineCounter, fileName, _errorMessege);
#define ERROR_CASE(_stage,_flag, _errorMessege) case _flag:\
PRINT_ERROR(_stage, _errorMessege);
