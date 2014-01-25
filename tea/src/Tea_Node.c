#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tea.h"
#include "libDos9.h"

const char* lpDelimsInlineOpen[]={"\\{","{", NULL};
const char* lpDelimsInlineClose[]={"\\}","}", NULL};
const char* lpDelimsInlineLink[]={"\\|","|", NULL};

TEANODE*    Tea_AllocTeaNode(void)
{
    TEANODE* lpTeaNode;

    if ((lpTeaNode=malloc(sizeof(TEANODE)))) {
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

void        Tea_FreeTeaNode(TEANODE* lpTeaNode)
{
    if (lpTeaNode->lpContent)
        free(lpTeaNode->lpContent);

    if (lpTeaNode->lpTarget)
        free(lpTeaNode->lpTarget);

    free(lpTeaNode);
}

TEANODE*    Tea_ParseStringNode(char* lpContent, LP_PARSE_HANDLER pHandler)
{
    TEANODE *lpTeaNode, *lpTeaNodeBegin;
    char *lpNextToken,
         *lpSearchBegin=lpContent,
         *lpContentSave;

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
        if (!(lpTeaNode->lpContent=strdup(lpContent))) {

            perror("TEA :: impossible d'allouer de la mémoire ");
            exit(-1);

        }

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

        lpContentSave=lpContent;

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

            if (!(lpTeaNode->lpContent=strdup(lpContentSave))) {

                perror("TEA :: impossible d'allouer de la mémoire ");
                exit(-1);

            }

        } else {
            /* le noeud est un lien */
            *lpNextToken='\0';
            lpTeaNode->iNodeType=TEA_NODE_LINK;

            if (!(lpTeaNode->lpTarget=strdup(lpContentSave))) {

                perror("TEA :: impossible d'allouer de la mémoire ");
                exit(-1);

            }

            if (!(lpTeaNode->lpContent=strdup(lpNextToken+1))) {

                perror("TEA :: impossible d'allouer de la mémoire ");
                exit(-1);

            }

        }

        pHandler(TEA_MSG_MAKE_NODE, lpTeaNode);


        /* on prépare la ligne suivante */
        lpSearchBegin=lpContent;

        lpTeaNode->lpTeaNodeNext=Tea_AllocTeaNode();
        lpTeaNode=lpTeaNode->lpTeaNodeNext;
    }

    if (!(lpTeaNode->lpContent=strdup(lpContent))) {

        perror("TEA :: impossible d'allouer de la mémoire ");
        exit(-1);

    }

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

TEANODE*    Tea_RemoveVoidNode(TEANODE* lpTeaNode)
{
    TEANODE *lpTeaBegin=lpTeaNode, *lpTeaPrevious=NULL;

    while (lpTeaNode) {

        if (*(lpTeaNode->lpContent)=='\0') {
            /* si le block est vide */
            if (lpTeaNode==lpTeaBegin) {
                /* si il s'agit du premier block du document */
                lpTeaBegin=lpTeaNode->lpTeaNodeNext;
                Tea_FreeTeaNode(lpTeaNode);

                /* on reprend la boucle au début */
                lpTeaNode=lpTeaBegin;
                continue;

            } else {
                lpTeaPrevious->lpTeaNodeNext=lpTeaNode->lpTeaNodeNext;

                Tea_FreeTeaNode(lpTeaNode);

                lpTeaNode=lpTeaPrevious->lpTeaNodeNext;

                continue;
            }

        }
        lpTeaPrevious=lpTeaNode;
        lpTeaNode=lpTeaNode->lpTeaNodeNext;
    }

    return lpTeaBegin;
}
