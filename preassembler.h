typedef enum {
    waitingForFile,
    readingLine,
    emptyLine,
    foundMacroDefinition,
    foundMacro,
    savingMacro,
    macroDefinitionEnded,
    placingMacro,
    noMacroFound,
    finished
} States;