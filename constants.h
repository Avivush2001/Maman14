#define RAM_SIZE 4096
#define REG_SIZE 14
#define REG_COUNT 8
#define MAX_OP_SIZE 5

#define R0 0x0
#define R1 0x1
#define R2 0x2
#define R3 0x3
#define R4 0x4
#define R5 0x5
#define R6 0x6
#define R7 0x7

#define DATA ".data"
#define STRING ".data"
#define ENTRY ".data"
#define EXTERN ".extern"
#define DEFINE ".define"

#define _A 0x00
#define _R 0x01
#define _E 0x10

typedef enum
{
    aestrix = 0x00,
    hash = 0x01,
    precent = 0x10,
    exclamation = 0x11
} Cypher;

typedef struct 
{
    unsigned int opType;
    unsigned int dest : 2;
    unsigned int src : 2;
    unsigned int opCode : 2;
} operation;