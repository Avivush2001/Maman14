#include "data.h"

extern char * operationsArr[];
extern char * registersArr[];
extern char * instructionArr[];
/*
THESE ARE TEMPORARY UPDATE THEM AND THEIR FUNCTIONS WITH THE PROPER STRUCTS
*/


/*
Given a string, a string array and its size, it checks if the string is in the array.
If it is 'index' is updated. If not index stays NOT_FOUND. Returns the index.
*/
int findInStringArray(char* word, char *arr[], int size) {
    int index, i;
    index = NOT_FOUND;
    for (i = 0; i < size; i++) {
        if (!strcmp(word, arr[i])) {
            index = i;
            break;
        }
    }
    return index;
}


/*
Given a string the function checks if it can be a legal label.
It first checks if the word is either: 
-preserved
-the first character is an alphabetical letter
-its size doesn't exceed the maximum allowed.
if one of them is true, the flag 'ans' turns to false.
If not, check the rest of the letters. If one of them isn't an alphabetical letter,
or a digit, 'ans' turns to false.
In the end ans is returned.

NOTE: the string must not include ':' at the end
and doesn't check any label / macro hashtable.
*/
Bool isLabelLegal(char *label) {
    Bool ans;
    int i, size;
    size = strlen(label);
    ans = True;
    if (isPreservedWord(label) || !isalpha(label[0]) || size > MAX_LABEL_SIZE) ans = False;
    else {
        for (i = 1; i < size; i++) {
            if (!isalpha(label[i]) && !isdigit(label[i])) {
                ans = False;
                break;
            }
        }
        }
    return ans;
}

/*
Given a string, it checks if it is present in one of the reserved word arrays.
If it is 'ans' is changed to False.
In the end ans is returned.
*/
Bool isPreservedWord(char *label) {
    Bool ans;
    ans = False;
    if (findInStringArray(label, operationsArr, OPERATIONS_SIZE) != NOT_FOUND) ans = True;
    else if (findInStringArray(label, registersArr, REGISTERS_SIZE) != NOT_FOUND) ans = True;
    else if (findInStringArray(label, instructionArr, INSTRUCTIONS_SIZE) != NOT_FOUND) ans = True;
    return ans;
}

/*
Given a string, it counts the words in that line that are seperated.
*/
int countWordsInLine(char *line) {
    int count, i;
    count = 0;
    for (i = 0; i < strlen(line); i++) {
        if (isgraph(line[i])) count++;
        while(isgraph(line[i])) i++;
    }
    return count;
}