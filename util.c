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
static int findInStringArray(char*, char**, int);

static int findInStringArray(char* word, char *arr[], int size) {
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
int findOperation(char* word) {
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

int findRegister(char* word) {
    return findInStringArray(word, registersArr, REGISTERS_SIZE);
}
int findInstruction(char* word) {
     return findInStringArray(word, instructionArr, INSTRUCTIONS_SIZE);
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
    if (findOperation(label) != NOT_FOUND) ans = True;
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

/* Checks if a string is a whole number- if it is return the number as a int */
wholeNum string_to_int(const char *str)
{
    wholeNum num = {True,0};
    int sign = 1, i = 0, x = 0, digit_value; /*sign: 1 for positive, -1 for negative*/
  if (str == NULL || *str == '\0') {
    num.isNum = False;
  } else if (*str == '+') {
    str++; /*Skip the positive sign if present*/
  }
  else if (*str == '-') {
    sign = -1;
    str++; /*Skip the negative sign if present*/ 
  }
  
  while (*str && num.isNum) {
    
    if (!isdigit(*str) || i == 4) { /*Invalid character encountered*/
      num.isNum = False;
    }
    else {
            digit_value = *str - '0'; /*Convert ASCII digit to numeric value*/
            x = x * 10 + digit_value; /*Build the integer*/
            i++;
            str++;
        }
  }
  num.result = x * sign;
  return num;
}

Bool isLegalArray(const char *str)
{
    Bool flag = True;
    char *left = NULL, *right = NULL, temp[MAX_LABEL_SIZE];
    int i, size = strlen(str);
    if(str == NULL || str[0] == '\0')
        flag = False;

    else if(isalpha(str[0]) == 0)
        flag = False;
    
    else
    {
        left = strchr(str, '[');
        right = strchr(str, ']');
        if(left == NULL || right == NULL) /* There are no brackets in the string so definitely not array */
            flag = False;
        
        else if(left > right) /* '[' has to come before ']' */
            flag = False;
        
        else if(*(right + 1) != '\0') /* If it is an array the char ']' has to be the last in the string */
            flag = False; 
        
        else
        {
            i = size - strlen(left);
            strncpy(temp, str, i);
            temp[i] = '\0';
            if(isLabelLegal(temp) == False)
                flag = False;
            
            else
            {
                left++;
                if(left == right) /* empty brackets ('[]') */
                    flag = False;

                else
                {
                    size = strlen(left);
                    i = size - strlen(right);
                    strncpy(temp, left, i);
                    temp[i] = '\0';
                    if(isLabelLegal(temp) == False && string_to_int(temp).isNum == False)
                        flag = False;
                }
            }
        }
    }
    return flag;
}

char *newFileName(char *fname, char *suffix) 
{
    char *p;
    int nameSize, suffixSize;
    nameSize = strlen(fname);
    suffixSize = strlen(suffix);
    p = malloc(sizeof(char) * (nameSize + suffixSize + 1));
    EXIT_IF(p == NULL)
    p = strcpy(p, fname);
    p = strcat(p, suffix);
    
    return p;
}