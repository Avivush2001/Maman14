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
    errorMacroNameIllegal,
    errorMacroNameAlreadyDefined,
    errorMacroHashTableFull,
    errorEncounteredPA,
    errorNoMacroNameGiven

} PreassemblerFlags;



