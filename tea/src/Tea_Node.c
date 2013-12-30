#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tea_Lib.h"
#include "libDos9.h"

const char* lpDelimsInlineOpen[]={"\\{","{", NULL};
const char* lpDelimsInlineClose[]={"\\}","}", NULL};
const char* lpDelimsInlineLink[]={"\\|","|", NULL};

TEANODE*    Tea_AllocTeaNode(void)
{
    TEANODE* lpTeaNode;

    if (lpTeaNode=malloc(sizeof(TEANODE))) {
        lpTeaNode->iNodeType=TEA_NODE_PLAIN_TEXT;
        lpTeaNode->lpTeaNodeNext=NULL;
        lpTeaNode->lpContent=NULL;
        lpTeaNode->lpTarget=NULL;

        return lpTeaNode;

    } else {
        perror("Erreur ! allocation impossible : %s");
        exit(-1);
    }
}

void        Tea_FreeTeaNode(TEANODE* lpTeaNode, const TEAMODIFIERS* lpTeaMod)
{
    if (lpTeaNode->iNodeType == TEA_NODE_EMPHASIS && lpTeaMod->lpEmphasisFree) {
        lpTeaMod->lpEmphasisFree(lpTeaNode->lpContent);
    } else if (lpTeaNode->iNodeType == TEA_NODE_LINK && lpTeaMod->lpLinkFree) {
        lpTeaMod->lpLinkFree(lpTeaNode->lpContent);
    }
    free(lpTeaNode); /* on n'a besoin que de ça puisque il n'y a que ça
                        qui est alloué dynamiquement */
}

TEANODE*    Tea_ParseStringNode(char* lpContent, const TEAMODIFIERS* lpTeaMod)
{
    TEANODE *lpTeaNode, *lpTeaNodeBegin;
    char *lpNextToken, *lpSearchBegin=lpContent;
    int iTokenPos;

    /* on prépare le noeud pour le parsage */
    lpTeaNode=Tea_AllocTeaNode();
    lpTeaNodeBegin=lpTeaNode;

    while ((lpNextToken=Tea_SeekNextDelimiter(lpSearchBegin, lpDelimsInlineOpen, &iTokenPos))) {

        if (iTokenPos==0) {

            /* si le { est échappé, on passe au { suivant */
            lpSearchBegin=lpNextToken+2;
            continue;

        }

        /* on découpe la ligne */
        *lpNextToken='\0';
        lpTeaNode->lpContent=lpContent;

        /* on passe à l'inline suivant */
        lpContent=lpNextToken+1;
        lpSearchBegin=lpContent;

        while ((lpNextToken=Tea_SeekNextDelimiter(lpSearchBegin, lpDelimsInlineClose, &iTokenPos))) {
            if (iTokenPos==0) {

                /* si le { est échappé, on passe au { suivant */
                lpSearchBegin=lpNextToken+2;
                continue;

            } else {
                break;
            }
        }

        /* si le bloc n'est pas fermé */
        if (lpNextToken==NULL) {
            fprintf(stderr, "Erreur : Inline non termine : `%s'\n", lpContent);
            exit(-1);
        }

        /* on finit de stocker la ligne */
        *lpNextToken='\0';
        lpTeaNode->lpTeaNodeNext=Tea_AllocTeaNode();
        lpTeaNode=lpTeaNode->lpTeaNodeNext;
        lpTeaNode->lpContent=lpContent;

        /* on détermine si c'est un lien ou si c'est  juste une emphese */
        lpSearchBegin=lpContent;
        lpContent=lpNextToken+1;

        while ((lpNextToken=Tea_SeekNextDelimiter(lpSearchBegin, lpDelimsInlineLink, &iTokenPos))) {
            if (iTokenPos==0) {

                /* si le | est échappé, on passe au | suivant */
                lpSearchBegin=lpNextToken+2;
                continue;

            } else {
                break;
            }
        }

        if (lpNextToken==NULL) {
            /* le noeud est une emphase */
            lpTeaNode->iNodeType=TEA_NODE_EMPHASIS;
            if (lpTeaMod->lpEmphasisModifier) {
                lpTeaNode->lpContent=lpTeaMod->lpEmphasisModifier(lpTeaNode->lpContent);
            }
        } else {
            /* le noeud est un lien */
            *lpNextToken='\0';
            lpTeaNode->iNodeType=TEA_NODE_LINK;
            lpTeaNode->lpTarget=lpTeaNode->lpContent;
            lpTeaNode->lpContent=lpNextToken+1;
            if (lpTeaMod->lpLinkModifier) {
                lpTeaNode->lpContent=lpTeaMod->lpLinkModifier(lpTeaNode->lpContent, lpTeaNode->lpTarget);
            }
        }

        /* on prépare la ligne suivante */
        lpSearchBegin=lpContent;

        lpTeaNode->lpTeaNodeNext=Tea_AllocTeaNode();
        lpTeaNode=lpTeaNode->lpTeaNodeNext;
    }

    lpTeaNode->lpContent=lpContent;

    return lpTeaNodeBegin;
}

int         Tea_PurifyNode(TEANODE* lpTeaNode)
{
    int iSweepBegin=TRUE;

    while (lpTeaNode) {
        Tea_SweepSpaceNode(lpTeaNode->lpContent, &iSweepBegin);
        lpTeaNode=lpTeaNode->lpTeaNodeNext;
    }
    return 0;
}

int         Tea_SweepSpaceNode(char* lpContent, int* iSweepBegin)
{
    int iSpaceGroup=*iSweepBegin;
    char* lpNextContent=lpContent;

    while (*lpContent) {
        switch (*lpContent) {
            case '\\':
                if (*(lpContent+1)) {
                    lpContent++;
                    iSpaceGroup=FALSE;
                } else {
                    return 0;
                }
                break;

            case '\n':
            case '\t':
                *lpContent=' ';

            case ' ':
                if (iSpaceGroup) {
                    lpContent++;
                    continue;
                } else {
                    iSpaceGroup=TRUE;
                }
                break;

            default:
                iSpaceGroup=FALSE;
        }
        *lpNextContent=*lpContent;
        lpNextContent++;
        lpContent++;
    }
    *lpNextContent=*lpContent;
    *iSweepBegin=iSpaceGroup;
    return 0;
}

TEANODE*    Tea_RemoveVoidNode(TEANODE* lpTeaNode, const TEAMODIFIERS* lpTeaMod)
{
    TEANODE *lpTeaBegin=lpTeaNode, *lpTeaPrevious=NULL;

    while (lpTeaNode) {

        if (*(lpTeaNode->lpContent)=='\0') {
            /* si le block est vide */
            if (lpTeaNode==lpTeaBegin) {
                /* si il s'agit du premier block du document */
                lpTeaBegin=lpTeaNode->lpTeaNodeNext;
                Tea_FreeTeaNode(lpTeaNode, lpTeaMod);

                /* on reprend la boucle au début */
                lpTeaNode=lpTeaBegin;
                continue;

            } else {
                lpTeaPrevious->lpTeaNodeNext=lpTeaNode->lpTeaNodeNext;

                Tea_FreeTeaNode(lpTeaNode, lpTeaMod);

                lpTeaNode=lpTeaPrevious->lpTeaNodeNext;

                continue;
            }

        }
        lpTeaPrevious=lpTeaNode;
        lpTeaNode=lpTeaNode->lpTeaNodeNext;
    }

    return lpTeaBegin;
}
