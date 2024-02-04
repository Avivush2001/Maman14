typedef enum {
    False = 0,
    True = 1
} Bool;

enum Registers {
    R0 = 0x0,
    R1 = 0x1,
    R2 = 0x2,
    R3 = 0x3,
    R4 = 0x4,
    R5 = 0x5,
    R6 = 0x6,
    R7 = 0x7
};

typedef enum {
    _A = 1,
    _r = 3,
    _E = 2
} ARE;

typedef enum {
    entry,
    code,
    data,
    string,
    external
} Attribute;

typedef enum {
    immediate = 0,
    direct = 1,
    index = 2,
    reg = 3
} Addressing;