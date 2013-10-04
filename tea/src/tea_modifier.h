#ifndef TEA_MODIFIERS_H
#define TEA_MODIFIERS_H

char*   tea_TextModifierLink(char* lpContent, char* lpTarget);

void   tea_HtmlHeadingStart(FILE* pOut, char* pTitle);
void   tea_HtmlHeadingEnd(FILE* pOut, char* pTitle);
void   tea_HtmlCodeStart(FILE* pOut, char* pTitle);
void   tea_HtmlCodeEnd(FILE* pOut, char* pTitle);
void   tea_HtmlParagraphStart(FILE* pOut, char* pTitle);
void   tea_HtmlParagraphEnd(FILE* pOut, char* pTitle);
void   tea_HtmlEmphasisStart(FILE* pOut, char* pTitle);
void   tea_HtmlEmphasisEnd(FILE* pOut, char* pTitle);
void   tea_HtmlLinkStart(FILE* pOut, char* lpTarget);
void   tea_HtmlLinkEnd(FILE* pOut, char* pTitle);

#define TEA_HLP_DEFAULT_COLOR 0x10 /* start of normal text */
#define TEA_HLP_TITLE_COLOR   0x11 /* start of title color */
#define TEA_HLP_CODE_COLOR    0x12 /* start of code color */
#define TEA_HLP_EM_COLOR      0x13 /* start of emphasis color */
#define TEA_HLP_LINK_COLOR    0x14 /* start of link color */

char*   tea_HlpModifierLink(char* lpContent, char* lpTarget);

void   tea_HlpHeadingStart(FILE* pOut, char* pTitle);
void   tea_HlpCodeStart(FILE* pOut, char* pTitle);
void   tea_HlpEmphasisStart(FILE* pOut, char* pTitle);
void   tea_HlpLinkStart(FILE* pOut, char* lpTarget);
void   tea_HlpEnd(FILE* pOut, char* lpTarget);


#endif // TEA_MODIFIERS_H
