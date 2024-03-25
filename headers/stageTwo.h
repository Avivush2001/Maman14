#define CHECK_ERROR_STAGE_TWO(_condition, _possibleError) if (_condition){\
flag = errorST;\
fprintf(stderr, _possibleError);\
}


Bool stageTwo(char *);
Bool updateMemory();
Bool createEntryFile(char *);
Bool createExternFile(char *);
Bool createObFile (char *);
