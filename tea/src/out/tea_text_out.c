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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libDos9.h>
#include "../Tea.h"
#include "tea_out.h"

void Tea_TextOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv)
{

    while (lpTeaPage) {

        switch (lpTeaPage->iBlockType) {

            case TEA_BLOCK_CODE:

                Tea_TextOutputStaticBlock(TEA_TEXT_CODE_INDENT+TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel),
                                          TEA_TEXT_CODE_NINDENT+TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel),
                                          TEA_TEXT_WIDTH,
                                          lpTeaPage->lpBlockContent,
                                          "\n",
                                          pFile);

                break;

            case TEA_BLOCK_HEADING:
                strupr(lpTeaPage->lpBlockContent);

                Tea_TextOutputStaticBlock(TEA_TEXT_HEAD_INDENT,
                                          0,
                                          TEA_TEXT_WIDTH,
                                          lpTeaPage->lpBlockContent,
                                          "\n",
                                          pFile);


                break;

            case TEA_BLOCK_PARAGRAPH:
                Tea_TextOutputParagraph(TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel+1),
                                        TEA_TEXT_PAR_INDENT*(lpTeaPage->iBlockLevel+1)
                                        - (((lpTeaPage->iBlockLevel!=0) && !(lpTeaPage->iFlag & TEA_LIST_NO_MARK)) ? 2 : 0),
                                        TEA_TEXT_WIDTH,
                                        lpTeaPage->lpTeaNode,
                                        "\n",
                                        pFile);


        }

        fputs("\n\n", pFile);

        lpTeaPage=lpTeaPage->lpTeaNext;

    }
}

void Tea_TextOutputStaticBlock(size_t iMargin, size_t iNewLineMargin, size_t iLength, char* lpBlock, char* lpNl, FILE* pFile)
{
    size_t iLeft;
    int    bFirstLine=TRUE;

    if ((iMargin >= iLength)
        || (iNewLineMargin >= iLength)) {

        fprintf(stderr, "Error : Incoherent margin parameters.\n");
        exit(-1);

    }

    while (lpBlock) {

        iLeft=iLength;

        /* we produce the margin */
        if (bFirstLine) {

            bFirstLine=FALSE;

            if (*lpBlock=='\n')
                lpBlock++;

            Tea_MakeMargin(iMargin, &iLeft, pFile);

        } else {

            fputs(lpNl, pFile);

            if (*lpBlock=='\n') {

                lpBlock++;
                Tea_MakeMargin(iMargin, &iLeft, pFile);

            } else {

                Tea_MakeMargin(iNewLineMargin, &iLeft, pFile);

            }


        }

        lpBlock=Tea_OutputLine(lpBlock, pFile, &iLeft);

    }

}

void Tea_TextOutputParagraph(size_t iMargin, size_t iFirstLine, size_t iLength, TEANODE* lpTeaNode, char* lpNl, FILE* pFile)
{
    size_t iLeft,
           iLeftNl;
    int    bFirstLine=TRUE;
    int    iNoMargin=FALSE;
    char   *lpBlock,
           *lpNext;

    if ((iMargin >= iLength)
        || (iFirstLine >= iLength)) {

        fprintf(stderr, "TEA : Error : Incoherent margin parameters.\n");
        exit(-1);

    }

    while (lpTeaNode) {

        lpBlock=lpTeaNode->lpContent;

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

                iLeftNl=iLeft;

            }

            lpBlock=Tea_OutputLineT(lpBlock, pFile, lpTeaNode, &iLeft);

            if (iLeftNl==iLeft) {

                /* if nothing has been written since we wrote the
                   last line, it should be that the world is to
                   long too fit in a single line, thus, we should try
                   to split it in various lines */

                while (iLeft) {

                    lpNext=Dos9_GetNextChar(lpBlock);

                    while (lpBlock < lpNext) {

                        fputc(*lpBlock, pFile);
                        lpBlock++;

                    }

                    iLeft--;

                }

                while (*lpBlock==' ')
                    lpBlock++;

            }

        }

        iNoMargin=TRUE;
        lpTeaNode=lpTeaNode->lpTeaNodeNext;

    }

}

void Tea_TextParseHandler(int iMsg ,void* lpData)
{
    ESTR* lpEsStr;
    TEANODE* lpTeaNode;

    if (iMsg==TEA_MSG_READ_FILE)
        return;

    lpEsStr=Dos9_EsInit();
    lpTeaNode=(TEANODE*)lpData;

    switch(lpTeaNode->iNodeType) {

        case TEA_NODE_EMPHASIS:
            strupr(lpTeaNode->lpContent);
            break;

        case TEA_NODE_LINK:
            Dos9_EsCpy(lpEsStr, lpTeaNode->lpContent);
            Dos9_EsCat(lpEsStr, " (-> ``");
            Dos9_EsCat(lpEsStr, lpTeaNode->lpTarget);
            Dos9_EsCat(lpEsStr, "'') ");

            free(lpTeaNode->lpContent);

            if (!(lpTeaNode->lpContent=strdup(Dos9_EsToChar(lpEsStr)))) {

                perror("TEA :: impossible d'allouer de la mémoire ");
                exit(-1);
            }
            break;

    }

    Dos9_EsFree(lpEsStr);
}

void Tea_TextPlainParseHandler(int iMsg ,void* lpData)
{
    ESTR* lpEsStr;
    TEANODE* lpTeaNode;

    if (iMsg==TEA_MSG_READ_FILE)
        return;

    lpEsStr=Dos9_EsInit();

    lpTeaNode=(TEANODE*)lpData;

    switch(lpTeaNode->iNodeType) {

        case TEA_NODE_EMPHASIS:
            strupr(lpTeaNode->lpContent);
            break;

        case TEA_NODE_LINK:
            Dos9_EsCpy(lpEsStr, "_");
            Dos9_EsCat(lpEsStr, lpTeaNode->lpContent);
            Dos9_EsCat(lpEsStr, "_");
            free(lpTeaNode->lpContent);

            if (!(lpTeaNode->lpContent=strdup(Dos9_EsToChar(lpEsStr)))) {

                perror("TEA :: impossible d'allouer de la mémoire ");
                exit(-1);
            }

            break;

    }

    Dos9_EsFree(lpEsStr);
}
