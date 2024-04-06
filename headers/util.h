/*
    Header for the utilities
*/

/*
Struct returned by stringToInt function.
The functions checks and converts a string
to an integers and returns a struct of this kind, with a boolean to indicate
if it was converted successfully 
*/
typedef struct {
    Bool isNum; 
    int result;
} WholeNum;

/*Finds a string in the operations array*/
int findOperation(char*);

/*Look for a register*/
int findRegister(char*);

/*Look for an instruction*/
int findInstruction(char*);

/*Checks if a string can be used as a legal label*/
Bool isValidSymbol(char*);

/*Convert a string to an int*/
WholeNum stringToInt(const char *);

/*Check if a string is a legal array*/
Bool isLegalArray(const char *);

/*Get a new file name*/
char *newFileName(char *, char *);

/*Calculate the base 10 value of the last 12 bits of a binary word*/
int baseTwoToTen(char *);