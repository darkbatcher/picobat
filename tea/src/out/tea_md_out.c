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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef WIN32
#include <strings.h>
#endif

#include <libpBat.h>
#include "../Tea.h"
#include "tea_out.h"

void Tea_MDOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv)
{
    int first = 1;

	while (lpTeaPage) {

		switch (lpTeaPage->iBlockType) {

		case TEA_BLOCK_CODE:

			Tea_TextOutputStaticBlock(4+4*(lpTeaPage->iBlockLevel),
			                          4+4*(lpTeaPage->iBlockLevel),
			                          0xFFFF,
			                          lpTeaPage->lpBlockContent,
			                          "\n",
			                          pFile);

			break;

		case TEA_BLOCK_HEADING:

            if (first) {
                fprintf(pFile, "# %s", lpTeaPage->lpBlockContent);
            } else {
                fprintf(pFile, "## %s", lpTeaPage->lpBlockContent);
            }

			break;

		case TEA_BLOCK_PARAGRAPH:
            if (*(lpTeaPage->lpTeaNode->lpContent) == '-')
                *(lpTeaPage->lpTeaNode->lpContent) = '*';

			Tea_TextOutputParagraph(2*(lpTeaPage->iBlockLevel),
			                        2*(lpTeaPage->iBlockLevel)
			                        - (((lpTeaPage->iBlockLevel!=0) && !(lpTeaPage->iFlag & TEA_LIST_NO_MARK)) ? 2 : 0),
			                        TEA_TEXT_WIDTH,
			                        lpTeaPage->lpTeaNode,
			                        "\n",
			                        pFile);

		}

		fputs("\n\n", pFile);
        first = 0;
		lpTeaPage=lpTeaPage->lpTeaNext;

	}
}

void Tea_MDEscape(char** pp)
{
    ESTR* estr= pBat_EsInit();
    char* new;

    pBat_EsCpy(estr, *pp);
    pBat_EsReplace(estr, "_", "\\\\_");
    pBat_EsReplace(estr, "*", "\\\\*");
    pBat_EsReplace(estr, "]", "\\\\]");
    pBat_EsReplace(estr, "[", "\\\\[");
    pBat_EsReplace(estr, "(", "\\\\(");
    pBat_EsReplace(estr, ")", "\\\\)");
    pBat_EsReplace(estr, "|", "\\\\|");
    pBat_EsReplace(estr, "~", "\\\\~");
    pBat_EsReplace(estr, "#", "\\\\#");

    if ((new = strdup(estr->str))) {
        free(*pp);
        *pp = new;
    }

    pBat_EsFree(estr);
}

void Tea_MDParseHandler(int iMsg ,void* lpData)
{
	ESTR* lpEsStr;
	TEANODE* lpTeaNode;

	if (iMsg==TEA_MSG_READ_FILE) {
        return;
	}

	lpEsStr=pBat_EsInit();

	lpTeaNode=(TEANODE*)lpData;

	switch(lpTeaNode->iNodeType) {

	case TEA_NODE_EMPHASIS:
        Tea_MDEscape(&(lpTeaNode->lpContent));

		pBat_EsCat(lpEsStr, "**");
		pBat_EsCat(lpEsStr, lpTeaNode->lpContent);
		pBat_EsCat(lpEsStr, "**");
		free(lpTeaNode->lpContent);

		if (!(lpTeaNode->lpContent=strdup(pBat_EsToChar(lpEsStr)))) {

			perror("TEA : Unable to allocate memory ");
			exit(-1);

		}

		break;

	case TEA_NODE_LINK:
        Tea_MDEscape(&(lpTeaNode->lpContent));
        Tea_MDEscape(&(lpTeaNode->lpTarget));

        pBat_EsCpy(lpEsStr, "[");
		pBat_EsCat(lpEsStr, lpTeaNode->lpContent);
		pBat_EsCat(lpEsStr, "]");
		pBat_EsCat(lpEsStr, "(");
		pBat_EsCat(lpEsStr, lpTeaNode->lpTarget);
		pBat_EsCat(lpEsStr, ")");

		free(lpTeaNode->lpContent);

		if (!(lpTeaNode->lpContent=strdup(pBat_EsToChar(lpEsStr)))) {

			perror("TEA :: impossible d'allouer de la m�moire ");
			exit(-1);
		}

		break;

    case TEA_NODE_PLAIN_TEXT:
        Tea_MDEscape(&(lpTeaNode->lpContent));

	}

	pBat_EsFree(lpEsStr);
}
