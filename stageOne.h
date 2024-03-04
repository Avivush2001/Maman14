#define DEFAULT_CONTEXT_SO contextFlag = readingLineSO;




typedef enum {
    readingLineSO,
    errorEncounteredSO,
    allclearSO
} StageOneFlags;


/*The field struct is used for a memory function to insert operation binary's.
Note that when using a field that isn't a label, equal 'name' to NULL*/
