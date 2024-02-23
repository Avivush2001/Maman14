typedef struct {
    char name[4];
    Bool allowedSrc[4];
    Bool allowedDst[4];
    unsigned fields :2;
} Operation;

typedef struct {
    char *name;
    int value;
    Bool entry;
    Attribute attr;
} Symbol;




