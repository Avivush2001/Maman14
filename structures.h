typedef enum {
    False = 0,
    True = 1
} Bool;

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

/*The field struct is used for a memory function to insert operation binary's.
Note that when using a field that isn't a label, equal 'name' to NULL*/
typedef struct {
    Addressing type;
    char *symbol;
    unsigned value : 12;
} Field;
/*
Add how to use the field struct
*/



