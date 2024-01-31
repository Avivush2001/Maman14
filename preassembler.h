typedef enum {
    waitingForFile,
    readingLine,
    emptyLine,
    foundMacroDefinition,
    savingMacro,
    macroDefinitionEnded,
    placingMacro,
    finished
} States;