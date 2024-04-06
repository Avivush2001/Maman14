THIS READ ME FILE BRIEFLY EXPLAINS THE OPERATION OF THE ASSEMBLER

-The arguments given to the assembler have to be without a suffix

-Two special arguments "RUN_UNI_TEST" and "RUN_ALL_TESTS" run the original university test and a couple
    of other tests.

-Developed mainly on windows, ran and tested successfully on windows 64 bit, ubuntu 64 bit and the university VM 32 bit ubuntu.
    the test files and their outputs were generated on the university VM 32 bit ubuntu.

-Compile before use.

About the structure of the program: We separated tasks into their own files, and created for each one 
header file, and each header holds macros, structures, functions prototypes for the task (mainly used for the task but available for the rest of the program)