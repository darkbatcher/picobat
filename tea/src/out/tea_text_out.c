#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libDos9.h>
#include "../Tea.h"
#include "tea_out.h"

void Tea_TextOutputHandler(TEAPAGE* lpTeaPage, FILE* pFile, int i, char** argv)
{
    int iLastLevel=0;

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
                iLastLevel=0;

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

        printf("\n\n");

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
        if (*lpBlock!='\n' && !bFirstLine) {


            Tea_MakeMargin(iNewLineMargin, &iLeft, pFile);

        } else if (bFirstLine) {

            bFirstLine=FALSE;

            if (*lpBlock=='\n')
                lpBlock++;

            Tea_MakeMargin(iMargin, &iLeft, pFile);

        } else {

            if (*lpBlock=='\n')
                lpBlock++;

            fputs(lpNl, pFile);

            Tea_MakeMargin(iMargin, &iLeft, pFile);

        }

        lpBlock=Tea_OutputLine(lpBlock, pFile, &iLeft);

    }

}

void Tea_TextOutputParagraph(size_t iMargin, size_t iFirstLine, size_t iLength, TEANODE* lpTeaNode, char* lpNl, FILE* pFile)
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

        iNoMargin=TRUE;
        lpTeaNode=lpTeaNode->lpTeaNodeNext;

    }

}

void Tea_TextParseHandler(TEANODE* lpTeaNode)
{
    ESTR* lpEsStr=Dos9_EsInit();

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
