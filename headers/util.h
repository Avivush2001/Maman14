typedef struct {
    Bool isNum; 
    int result;
} wholeNum;

/*Finds a string in the operations array*/
int findOperation(char*);

/*Look for a register*/
int findRegister(char*);

/*Look for an instruction*/
int findInstruction(char*);

/*Checks if a string can be used as a legal label*/
Bool isValidSymbol(char*);

/*Convert a string to an int*/
wholeNum string_to_int(const char *);

/*Check if a string is a legal array*/
Bool isLegalArray(const char *);

/*Get a new file name*/
char *newFileName(char *, char *);