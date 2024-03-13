typedef struct {
    Bool isNum; 
    int result;
} wholeNum;

/*Finds a string in a string array*/
int findInStringArray(char*, char**, int);

/*Finds a string in the operations array*/
int findInOperationsArray(char*);

/*Checks if a string can be used as a legal label*/
Bool isLabelLegal(char*);

/*Checks if a string is a preserved word*/
Bool isPreservedWord(char*);

/*Counts words in a line*/
int countWordsInLine(char*);

wholeNum string_to_int(const char *);
Bool isLegalArray(const char *);