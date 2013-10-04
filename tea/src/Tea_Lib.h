#ifndef TEA_LIB_H
#define TEA_LIB_H

#define TEA_NODE_PLAIN_TEXT 0
#define TEA_NODE_LINK 1
#define TEA_NODE_EMPHASIS 2

#define TEA_BLOCK_PARAGRAPH 0
#define TEA_BLOCK_CODE 1
#define TEA_BLOCK_HEADING 2

typedef struct TEANODE {
    int iNodeType;
    char *lpContent;
    char *lpTarget;
    struct TEANODE* lpTeaNodeNext;
} TEANODE,*LPTEANODE;

typedef struct TEAPAGE {
    int iBlockType;
    char* lpBlockContent;
    struct TEANODE* lpTeaNode; //
    struct TEAPAGE* lpTeaNext;
} TEAPAGE,*LPTEAPAGE;

typedef char* (*LP_LINK_MODIFIER)(char*, char*);
typedef char* (*LP_EMPHASIS_MODIFIER)(char*);
typedef char* (*LP_HEADING_MODIFIER)(char*);
typedef int (*LP_FREE_MODIFIER)(char*);

typedef struct TEAMODIFIERS {
    LP_LINK_MODIFIER lpLinkModifier;
    LP_EMPHASIS_MODIFIER lpEmphasisModifier;
    LP_HEADING_MODIFIER lpHeadingModifier;
    LP_FREE_MODIFIER lpLinkFree;
    LP_FREE_MODIFIER lpEmphasisFree;
    LP_FREE_MODIFIER lpHeadingFree;
} TEAMODIFIERS, *LPTEAMODIFIERS;


TEAPAGE*    Tea_PageLoad(const char* lpFilename, const TEAMODIFIERS* lpTeaMod);
TEAPAGE*    Tea_ParseStringBlock(char* lpContent, const TEAMODIFIERS* lpTeaMod);

TEAPAGE*    Tea_AllocTeaPage(void);
void        Tea_FreeTeaPage(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod);

int         Tea_PurifyPage(TEAPAGE* lpTeaPage);
int         Tea_SweepSpace(char* lpContent);
int         Tea_SweepCodeTabs(char* lpContent);

int         Tea_BreakParagraph(TEAPAGE* lpTeaPage);
TEAPAGE*    Tea_RemoveVoidBlocks(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod);

void        Tea_ParseParagraphs(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod);

int         Tea_PageFree(TEAPAGE* lpTeaPage, const TEAMODIFIERS* lpTeaMod);

char*       Tea_SeekNextDelimiter(const char* lpBeginPos, const char** lpToken, int*  iDelimPos);
char*       Tea_SeekNextClosingBrace(const char* lpBeginPos);

TEANODE*    Tea_AllocTeaNode(void);
void        Tea_FreeTeaNode(TEANODE* lpTeaNode, const TEAMODIFIERS* lpTeaMod);

TEANODE*    Tea_ParseStringNode(char* lpContent, const TEAMODIFIERS* lpTeaMod);
int         Tea_PurifyNode(TEANODE* lpTeaNode);
int         Tea_SweepSpaceNode(char* lpContent, int* iSweepBegin);
TEANODE*    Tea_RemoveVoidNode(TEANODE* lpTeaNode, const TEAMODIFIERS* lpTeaMod);


/* type for processing output */

typedef struct TEASTYLE {
    int iWidth; /* width of text */
    int iMarginWidth; /* the width of margin */
    int iIndentWidth; /* the width of indent */
} TEASTYLE;

typedef void (*LP_OUTPUT_MODIFIER)(FILE*, char*);

typedef struct TEAOUTPUTMODIFIERS {
    LP_OUTPUT_MODIFIER lpHeadingStart;
    LP_OUTPUT_MODIFIER lpHeadingEnd;
    LP_OUTPUT_MODIFIER lpCodeStart;
    LP_OUTPUT_MODIFIER lpCodeEnd;
    LP_OUTPUT_MODIFIER lpParagraphStart;
    LP_OUTPUT_MODIFIER lpParagraphEnd;
    LP_OUTPUT_MODIFIER lpEmphasisStart;
    LP_OUTPUT_MODIFIER lpEmphasisEnd;
    LP_OUTPUT_MODIFIER lpLinkStart;
    LP_OUTPUT_MODIFIER lpLinkEnd;
} TEAOUTPUTMODIFIERS;

typedef struct TEAOUTPUT {
    TEAOUTPUTMODIFIERS teaOutputMod;
    TEASTYLE teaCodeStyle;
    TEASTYLE teaParagraphStyle;
    char* lpBlockSpacing;
} TEAOUTPUT;

extern int teaIsEncodingUtf8;

int         Tea_OutputPage(FILE* pOutput, TEAOUTPUT* lpTeaOutput, TEAPAGE*TeaPage);
int         Tea_OutputParagraph(FILE* pOutput, TEASTYLE* lpTeaStyle, TEAOUTPUTMODIFIERS* lpTeaMods, TEANODE* lpTeaNode);
int         Tea_OutputCode(FILE* pOutput, TEASTYLE* lpTeaStyle, char* lpContent);
void        Tea_OutputRunModifier(LP_OUTPUT_MODIFIER lpModifier, FILE* pOutput, char* lpContent);
char*       Tea_OutputGetNextChar(char* lpContent);

int         Hlp_GetNextNodeWLenght(TEANODE* lpNode);

int         Tea_IsCharUtf8FolowingByte(char* lpChar);


#endif // TEA_LIB_H
