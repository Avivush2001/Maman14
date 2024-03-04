/*
This is a header file for more general structures and enums that I wasn't
sure where to put.
*/

typedef enum {
    False = 0,
    True = 1
} Bool;

/*Attribute of the symbol.*/
typedef enum {
    code,
    data,
    external,
    undefined
} Attribute;

typedef enum {
    immediate = 0,
    direct = 1,
    index = 2,
    reg = 3
} Addressing;

typedef struct {
    char name[4];
    Bool allowedSrc[4];
    Bool allowedDst[4];
    unsigned fields :2;
} Operation;

typedef struct {
    char *symbol;
    int value;
    Bool entry;
    Attribute attr;
} Symbol;

typedef struct {
    Addressing type;
    char *symbol;
    unsigned value : 12;
} Field;