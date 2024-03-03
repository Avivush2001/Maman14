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

/*The field struct is used for a memory function to insert operation binary's.
Note that when using a field that isn't a label, equal 'name' to NULL*/
typedef struct {
    Addressing type;
    char *name;
    unsigned value : 12;
} Field;



