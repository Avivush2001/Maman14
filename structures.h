
typedef struct {
    unsigned bit: 1;
} Bit;

typedef struct {
    Attribute attr;
    Bit bits[14];
    unsigned address;
 } Word;

struct Operation {
    unsigned code;
    AllowedAddressing allowedSrc;
    AllowedAddressing allowedDst;
    unsigned fields : 2;
} Mov, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Hlt;

typedef struct {
    Bit addrsMethod[4];
} AllowedAddressing;

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

typedef struct {
    char *name;
    unsigned range[2];
} Macro;