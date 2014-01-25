#ifndef TEA_MODIFIERS_H
#define TEA_MODIFIERS_H

#define TEA_HLP_DEFAULT_COLOR 0x10 /* start of normal text */
#define TEA_HLP_TITLE_COLOR   0x11 /* start of title color */
#define TEA_HLP_CODE_COLOR    0x12 /* start of code color */
#define TEA_HLP_EM_COLOR      0x13 /* start of emphasis color */
#define TEA_HLP_LINK_COLOR    0x14 /* start of link color */

#define TEA_TEXT_WIDTH        79 /* the width of the text */
#define TEA_TEXT_CODE_INDENT  8 /* the witdh of code indent */
#define TEA_TEXT_CODE_NINDENT 6 /* the witdh of code indent */
#define TEA_TEXT_PAR_INDENT   5 /* the witdh of paragraph indent */
#define TEA_TEXT_HEAD_INDENT  0 /* the width of paragaph indent */

#include <libDos9.h>
#include "../Tea.h"

void Tea_TextOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv);
void Tea_TextParseHandler(int iMsg ,void* lpData);

void Tea_TextOutputStaticBlock(size_t iMargin, size_t iNewLineMargin, size_t iLength, char* lpBlock, char* lpNl, FILE* pFile);
void Tea_TextOutputParagraph(size_t iMargin, size_t iFirstLine, size_t iLength, TEANODE* lpTeaNode, char* lpNl, FILE* pFile);

void Tea_HtmlOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv);
void Tea_HtmlParseHandler(int iMsg ,void* lpData);

void Tea_HtmlOutputParagraph(TEANODE* lpTeaNode, FILE* pFile);
void Tea_HtmlEscapeChar(ESTR* lpEsStr);

void Tea_HtmlGetTitle(TEAPAGE* lpTeaPage, ESTR* lpEsStr);
void Tea_HtmlLoadFile(ESTR* lpEsReturn, char* lpFileName);

#endif // TEA_MODIFIERS_H
