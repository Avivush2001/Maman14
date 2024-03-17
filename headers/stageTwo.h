typedef enum
{
    success,
    failure,
    errorCreatingFile,
    errorDeletingFile
} fileFlag;

fileFlag CreateEntryFile(char *fileName);
fileFlag CreateExternFile(char *fileName);
