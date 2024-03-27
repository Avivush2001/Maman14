#define CHECK_ERROR_STAGE_TWO(_condition, _possibleError) if (_condition){\
flag = errorST;\
fprintf(stderr, _possibleError);\
}

/*Main Stage Two Function*/
Bool stageTwo(char *);

