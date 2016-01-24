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

#include <libDos9.h>
#include "tea_out.h"
#include "../Tea.h"

void Tea_TextAnsiOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv)
{
	while (lpTeaPage) {

		switch (lpTeaPage->iBlockType) {

		case TEA_BLOCK_CODE:

			/* set color to green and set intensity */
			fprintf(pFile, "\033[32m\033[1m");

			Tea_TextOutputStaticBlock(TEA_TEXT_CODE_INDENT+TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel),
			                          TEA_TEXT_CODE_NINDENT+TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel),
			                          TEA_TEXT_WIDTH,
			                          lpTeaPage->lpBlockContent,
			                          "\n",
			                          pFile);

			fprintf(pFile, "\033[39m\033[0m");

			break;

		case TEA_BLOCK_HEADING:

			/* set color to purple and set intensity */
			fprintf(pFile, "\033[35m\033[1m");

			Tea_TextOutputStaticBlock(TEA_TEXT_HEAD_INDENT,
			                          0,
			                          TEA_TEXT_WIDTH,
			                          lpTeaPage->lpBlockContent,
			                          "\n",
			                          pFile);


			fprintf(pFile, "\033[39m\033[0m");

			break;

		case TEA_BLOCK_PARAGRAPH:
			Tea_TextAnsiOutputParagraph(TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel+1),
			                            TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel+1)
			                            - (((lpTeaPage->iBlockLevel!=0) && !(lpTeaPage->iFlag & TEA_LIST_NO_MARK)) ? 2 : 0),
			                            TEA_TEXT_WIDTH,
			                            lpTeaPage->lpTeaNode,
			                            "\n",
			                            pFile);

			break;
		}

		fputs("\n\n", pFile);

		lpTeaPage=lpTeaPage->lpTeaNext;

	}
}

void Tea_TextAnsiOutputParagraph(size_t iMargin, size_t iFirstLine, size_t iLength, TEANODE* lpTeaNode, char* lpNl, FILE* pFile)
{
	size_t iLeft;
	int    bFirstLine=TRUE;
	int    iNoMargin=FALSE;
	char   *lpBlock;

	if ((iMargin >= iLength)
	    || (iFirstLine >= iLength)) {

		fprintf(stderr, "Error : Incoherent margin parameters.\n");
		exit(-1);

	}

	while (lpTeaNode) {

		lpBlock=lpTeaNode->lpContent;

		switch(lpTeaNode->iNodeType) {

		case TEA_NODE_EMPHASIS:
			fprintf(pFile, "\033[36m\033[1m");
			break;

		case TEA_NODE_LINK:
			fprintf(pFile, "\033[34m\033[1m");
			break;

		}

		while (lpBlock) {

			if (iNoMargin) {

				iNoMargin=FALSE;

			} else {

				iLeft=iLength;

				if (*lpBlock=='\n')
					lpBlock++;

				if (bFirstLine) {

					bFirstLine=FALSE;
					Tea_MakeMargin(iFirstLine, &iLeft, pFile);

				} else {

					fputs(lpNl, pFile);
					Tea_MakeMargin(iMargin, &iLeft, pFile);

				}

			}

			lpBlock=Tea_OutputLineT(lpBlock, pFile, lpTeaNode, &iLeft);

		}

		if (lpTeaNode->iNodeType
		    !=TEA_NODE_PLAIN_TEXT)
			fprintf(pFile, "\033[39m\033[0m");

		iNoMargin=TRUE;
		lpTeaNode=lpTeaNode->lpTeaNodeNext;

	}

}


void Tea_TextAnsiParseHandler(int iMsg, void* lpData)
{
	return;
}
