typedef struct {
    Bool isNum; 
    int result;
} wholeNum;

/*Finds a string in a string array*/


/*Finds a string in the operations array*/
int findOperation(char*);

int findRegister(char*);
int findInstruction(char*);
/*Checks if a string can be used as a legal label*/
Bool isLabelLegal(char*);

/*Checks if a string is a preserved word*/
Bool isPreservedWord(char*);


wholeNum string_to_int(const char *);
Bool isLegalArray(const char *);