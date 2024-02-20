

typedef enum {
    R0 = 0x0,
    R1 = 0x1,
    R2 = 0x2,
    R3 = 0x3,
    R4 = 0x4,
    R5 = 0x5,
    R6 = 0x6,
    R7 = 0x7
} Registers;

/*Attribute of the symbol.*/
typedef enum {
    code,
    data,
    external
} Attribute;

typedef enum {
    immediate = 0,
    direct = 1,
    index = 2,
    reg = 3,
    dataWord
} Addressing;

typedef enum {
    code12
} Opcodes;