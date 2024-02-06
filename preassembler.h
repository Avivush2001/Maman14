typedef enum {
    readingLine,
    macroDefinitionStarted,
    macroDefinitionOngoing,
    macroDefinitionEnded,
    macroCall,
    addLine,
    skipMacroDefinition,
    skipUndefinedMacro,
    allclearPA,
    errorPA
} PreassemblerFlags;



