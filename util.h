/*Defines a boolean*/
typedef enum {
    False = 0,
    True = 1
} Bool;

/*Finds a string in a string array*/
int findInStringArray(char*, char**, int);

/*Checks if a string can be used as a legal label*/
Bool isLabelLegal(char*);

/*Checks if a string is a preserved word*/
Bool isPreservedWord(char*);

/*Counts words in a line*/
int countWordsInLine(char*);