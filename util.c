#include "data.h"

/*Can't be initialized another way besides allocating memory.*/
/*Holds the operations. Also each index represents each operation's opcode*/
Operation operationsArr[] = {
    {
        "mov",
        {True,True,True,True},
        {False,True,True,True},
        2
    },
    {
        "cmp",
        {True,True,True,True},
        {True,True,True,True},
        2
    },
    {
        "add",
        {True,True,True,True},
        {False,True,True,True},
        2
    },
    {
        "sub",
        {True,True,True,True},
        {False,True,True,True},
        2
    },
    {
        "not",
        {False,False,False,False},
        {False,True,True,True},
        1
    },
    {
        "clr",
        {False,False,False,False},
        {False,True,True,True},
        1
    },
    {
        "lea",
        {False,True,True,False},
        {False,True,True,True},
        2
    },
    {
        "inc",
        {False,False,False,False},
        {False,True,True,True},
        1
    },
    {
        "dec",
        {False,False,False,False},
        {False,True,True,True},
        1
    },
    {
        "jmp",
        {False,False,False,False},
        {False,True,False,True},
        1
    },
    {
        "bne",
        {False,False,False,False},
        {False,True,False,True},
        1
    },
    {
        "red",
        {False,False,False,False},
        {False,True,True,True},
        1
    },
    {
        "prn",
        {False,False,False,False},
        {True,True,True,True},
        1
    },
    {
        "jsr",
        {False,False,False,False},
        {False,True,False,True},
        1
    },
    {
        "rts",
        {False,False,False,False},
        {False,False,False,False},
        0
    },
    {
        "hlt",
        {False,False,False,False},
        {False,False,False,False},
        0
    }
};
/*These hold the strings of the registers and other reserved words. For the
registers array, each index represents its number.*/
char * registersArr[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char * instructionArr[] = {".entry" ,".extern", ".define", ".data",".string", MACRO_DEF, MACRO_END};
/*
Given a string, a string array and its size, it checks if the string is in the array.
If it is 'index' is updated. If not index stays NOT_FOUND. Returns the index.
*/
int findInStringArray(char* word, char *arr[], int size) {
    int in, i;
    in = NOT_FOUND;
    for (i = 0; i < size; i++) {
        if (!strcmp(word, arr[i])) {
            in = i;
            break;
        }
    }
    return in;
}
int findInOperationsArray(char* word) {
    int in, i;
    in = NOT_FOUND;
    for (i = 0; i < OPERATIONS_SIZE; i++) {
        if (!strcmp(word, operationsArr[i].name)) {
            in = i;
            break;
        }
    }
    return in;
}


/*
Given a string the function checks if it can Be a legal label.
It first checks if the word is either: 
-preserved
-the first chAracter is an alphabetical letter
-its size doesn't exceed the maximum allowed.
if one of them is true, the flag 'ans' turns to false.
If not, checK the rest of the letters. If one of them isn't an alphabetical letter,
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
    if (findInOperationsArray(label) != NOT_FOUND) ans = True;
    else if (findInStringArray(label, registersArr, REGISTERS_SIZE) != NOT_FOUND) ans = True;
    else if (findInStringArray(label, instructionArr, INSTRUCTIONS_SIZE) != NOT_FOUND) ans = True;
    return ans;
}

/*
Given a string, it counts the words in that line that are separated.
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

