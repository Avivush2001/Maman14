/*
This is a header file for more general structures and enums that we weren't
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
    constant,
    undefined
} Attribute;

/*Addressing methods*/
typedef enum {
    immediate = 0,
    direct = 1,
    index = 2,
    reg = 3
} Addressing;

/*
A struct that defines an operations.
has a name, 2 arrays of booleans to indicate which source or destination operands are allowed
and an integer that holds the fields expected
*/
typedef struct {
    char name[4];
    Bool allowedSrc[4];
    Bool allowedDst[4];
    unsigned fields :2;
} Operation;

/*
Defines a symbol. Can be either a label or a constant.
It has an attribute, the string of the symbol, a value, an array length (in case it is a data label) and a boolean
to check if it is entry.
*/
typedef struct {
    Attribute attr;
    char *symbol;
    int value;
    int arrayLength;
    Bool entry;
} Symbol;

/*A struct to define an operand, explanation about its use is given.*/
typedef struct {
    Addressing type;
    char *symbol;
    unsigned value : 12;
} Field;
/*
GUIDE how to fill different types of fields
Let "field" be a pointer to a Field struct.

Constants: 
examples: #-5, #3, #sz
field->type = immediate
field->symbol = NULL
field_value = the_value_after_# (if it is a constant look up the value in the symbol table)

Labels: 
examples: END MAIN
field->type = direct
field->symbol = Label (just the name)
field_value = 0

Arrays: 
examples: STR[5]
field->type = index
field->symbol = Label (just the name)
field_value = the_value_in_brackets (if it is a constant look up the value in the symbol table)

Registers:
examples: r1, r0,...
field->type = reg
field->symbol = NULL
field_value = the_number_of_the_register (is returned when searching for the register in the register array)
*/