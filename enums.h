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
    reg = 3,
    dataWord
} Addressing;