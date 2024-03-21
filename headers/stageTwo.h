typedef enum
{
    success,
    failure,
    errorCreatingFile,
    errorDeletingFile
} fileFlag;

typedef enum
{
    updateSuccess,
    errorIllegalLabelST
} StageTwoFlags;
StageTwoFlags updateMemory();
fileFlag createEntryFile(char *);
fileFlag createExternFile(char *);
fileFlag createObFile (char *);
