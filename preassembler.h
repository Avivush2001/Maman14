typedef enum {
    readingLine,
    macroDefinitionStarted,
    macroDefinitionOngoing,
    macroDefinitionEnded,
    macroCall,
    addLine,
    allclearPA,
    errorPA
} PreassemblerFlags;

