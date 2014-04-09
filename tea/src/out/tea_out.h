/*
 *
 *   TEA - A quick and simple text preprocessor
 *   Copyright (C) 2010-2014 DarkBatcher
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

#ifndef TEA_MODIFIERS_H
#define TEA_MODIFIERS_H

#define TEA_HLP_HEADER_START  0x01
#define TEA_HLP_TEXT_START    0x02
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

/* text output functions */

void Tea_TextOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv);
void Tea_TextParseHandler(int iMsg ,void* lpData);
void Tea_TextPlainParseHandler(int iMsg, void* lpData);

void Tea_TextAnsiOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv);
void Tea_TextAnsiParseHandler(int iMsg, void* lpData);


void Tea_TextOutputStaticBlock(size_t iMargin, size_t iNewLineMargin, size_t iLength, char* lpBlock, char* lpNl, FILE* pFile);
void Tea_TextOutputParagraph(size_t iMargin, size_t iFirstLine, size_t iLength, TEANODE* lpTeaNode, char* lpNl, FILE* pFile);
void Tea_TextAnsiOutputParagraph(size_t iMargin, size_t iFirstLine, size_t iLength, TEANODE* lpTeaNode, char* lpNl, FILE* pFile);

/* html output functions */

void Tea_HtmlOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv);
void Tea_HtmlParseHandler(int iMsg ,void* lpData);

void Tea_HtmlOutputParagraph(TEANODE* lpTeaNode, FILE* pFile);
void Tea_HtmlEscapeChar(ESTR* lpEsStr);

void Tea_HtmlGetTitle(TEAPAGE* lpTeaPage, ESTR* lpEsStr);
void Tea_HtmlLoadFile(ESTR* lpEsReturn, char* lpFileName);

/* hlp output functions */
void Tea_HlpOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv);
void Tea_HlpParseHandler(int iMsg ,void* lpData);

void Tea_HlpMakeHeader(TEAPAGE* lpTeaPage, FILE* pFile);

#endif // TEA_MODIFIERS_H
