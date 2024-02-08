#include "data.h"


/*
    The function gets a file name and a suffix, assuming it is formatted like so: 'name.oldsuffix' and 'suffix'.
    It declares a file pointer and a char pointer designated to hold the new name.
    Then it creates the new file name and then the file, frees the char pointer and returns the file pointer.
*/
/* 
The function gets a suffix and a file name as described earlier, declares a char poiner and allocating
it enough space for the file name (without the old suffix) and for the new suffix.
Then it copies the file name (withous its old suffix) to the space we allocated, and concats it with the suffix,
and returning the pointer.
*/
char *newFileName(char *fname, char *suffix) {
    char *p;
    int nameSize, suffixSize;
    nameSize = getDotIndex(fname)+1;
    suffixSize = strlen(suffix);
    p = malloc(sizeof(char) * (nameSize+suffixSize));
    p = strncpy(p, fname, nameSize);
    p = strcat(p, suffix);
    return p;
}

/*
Utility function, gets the index of a dot in a file name.
*/
int getDotIndex(char *fname) {
    int i = 0;
    while(fname[i] != '.') i++;
    return i;
}

