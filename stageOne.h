
/*Struct that represents an operation word.*/
typedef struct {
    Addressing type;
    unsigned unused :4;
    unsigned opcode :4;
    unsigned src :2;
    unsigned dst :2;
    unsigned are :2;
    void * nextWord;
    void * parWord;
 } OperationWord;
 
 typedef struct {
    Addressing type;
    unsigned constant :12;
    unsigned are :2;
    void * nextWord;
    void * parWord;
 } ConstWord;

 typedef struct {
    Addressing type;
    char *symbol;
    unsigned address :12;
    unsigned are :2;
    void * nextWord;
    void * parWord;
 } AddressWord;

 typedef struct {
    Addressing type;
    unsigned unused :4;
    unsigned src :3;
    unsigned dst :3;
    unsigned are :2;
    void * nextWord;
 } RegisterWord;

 typedef struct {
    Addressing type;
    unsigned data :14;
    void * nextWord;
 } DataWord;

 typedef struct {
    char bits[14];
    void * parallelWord;
 } Word;

typedef struct {
    char *name;
    Opcodes code; 
    Bool allowedSrc[4];
    Bool allowedDst[4];
    unsigned fields :2;
} Operation ;

typedef struct {
    char *name;
    int value;
    Bool entry;
    Attribute attr;
} Symbol;

typedef struct {
    Word words[ADRESSES_ALOWED];
} Memmory;

typedef struct {
    Memmory *memmory;
    SymbolHashTable *symbolTable;
    SymbolHashTable *macroTable;
} SystemTables;
