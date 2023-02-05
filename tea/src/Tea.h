/*
 *
 *   TEA - A quick and simple text preprocessor
 *   Copyright (C) 2010-2016 DarkBatcher
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TEA_LIB_H
#define TEA_LIB_H

#include <libpBat.h>

#define TEA_NODE_PLAIN_TEXT (0)
#define TEA_NODE_LINK       (1)
#define TEA_NODE_EMPHASIS   (2)
#define TEA_BLOCK_PARAGRAPH (3)
#define TEA_BLOCK_CODE      (4)
#define TEA_BLOCK_HEADING   (5)

#define TEA_LIST_NO_MARK    (1)

#define TEA_MSG_READ_FILE   (1)
#define TEA_MSG_MAKE_NODE   (2)

typedef struct TEANODE {
	int iNodeType;
	char *lpContent;
	char *lpTarget;
	struct TEANODE* lpTeaNodeNext;
} TEANODE,*LPTEANODE;

typedef struct TEAPAGE {
	int iBlockType;
	int iBlockLevel;
	int iFlag;
	char* lpBlockContent;
	struct TEANODE* lpTeaNode; //
	struct TEAPAGE* lpTeaNext;
} TEAPAGE,*LPTEAPAGE;

/*
    the modifier functions
 */
typedef void(*LP_OUTPUT_HANDLER)(TEAPAGE*,FILE*,int,char**);
typedef void(*LP_PARSE_HANDLER)(int,void*);

extern ESTR* lpEsEncoding;

TEAPAGE*    Tea_PageLoad(const char* lpFilename,  LP_PARSE_HANDLER pHandler);
TEAPAGE*    Tea_ParseStringBlock(char* lpContent);

TEAPAGE*    Tea_AllocTeaPage(void);
void        Tea_FreeTeaPage(TEAPAGE* lpTeaPage);

int         Tea_PurifyPage(TEAPAGE* lpTeaPage);
int         Tea_SweepSpace(char* lpContent);
int         Tea_SweepCodeTabs(char* lpContent);

int         Tea_BreakParagraph(TEAPAGE* lpTeaPage);
TEAPAGE*    Tea_RemoveVoidBlocks(TEAPAGE* lpTeaPage);

int         Tea_MakeLevels(TEAPAGE* lpTeaPage);

void        Tea_ParseParagraphs(TEAPAGE* lpTeaPage, LP_PARSE_HANDLER pHandler);

int         Tea_PageFree(TEAPAGE* lpTeaPage);

char*       Tea_SeekNextDelimiter(const char* lpBeginPos, const char** lpToken, int*  iDelimPos);
char*       Tea_SeekNextClosingBrace(const char* lpBeginPos);

TEANODE*    Tea_AllocTeaNode(void);
void        Tea_FreeTeaNode(TEANODE* lpTeaNode);

TEANODE*    Tea_ParseStringNode(char* lpContent, LP_PARSE_HANDLER pHandler);
int         Tea_PurifyNode(TEANODE* lpTeaNode);
int         Tea_SweepSpaceNode(char* lpContent, int* iSweepBegin);
TEANODE*    Tea_RemoveVoidNode(TEANODE* lpTeaNode);

/* for teanode specific word */
size_t      Tea_GetWordLengthT(char* lpBegin, TEANODE* lpTeaNode);

/* for all-purpose words */
size_t      Tea_GetWordLength(char* lpBegin);

void        Tea_MakeMargin(size_t iLength, size_t* iLeft, FILE* pFile);

char*       Tea_OutputWord(char* lpBegin, FILE* pFile, size_t* iLeft);

char*       Tea_OutputLineT(char* lpBegin, FILE* pFile, TEANODE* lpTeaNode, size_t* iLeft);

char*       Tea_OutputLine(char* lpBegin, FILE* pFile, size_t* iLeft);

#ifndef WIN32
char* strupr(char*);
#endif

#endif // TEA_LIB_H
