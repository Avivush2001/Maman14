
typedef struct {
    unsigned bit: 1;
} Bit;

typedef struct {
    Attribute attr;
    Bit bits[14];
    unsigned address;
 } Word;


typedef struct {
    Bit addrsMethod[4];
} AllowedAddressing;

typedef struct {
    unsigned code;
    AllowedAddressing allowedSrc;
    AllowedAddressing allowedDst;
    unsigned fields;
} Operation ;



typedef struct {
    Addressing addr;
    unsigned value;
} Operand;

typedef struct {
    ARE are;
    struct Operation *operation;
    Operand src;
    Operand dst;
} Instruction;

typedef struct {
    char *name;
    int value;
    Attribute attr;
} Symbol;



