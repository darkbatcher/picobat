#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>
#include "Tea.h"

size_t         Tea_GetWordLengthT(char* lpBegin, TEANODE* lpTeaNode)
{
    size_t iLength=0;

    while (*lpBegin!=' '
           && *lpBegin!='\n') {


        if (*lpBegin=='\0') {

            /* on passe au noeud suivant */

            do {

                lpTeaNode=lpTeaNode->lpTeaNodeNext;

                if (lpTeaNode)
                    lpBegin=lpTeaNode->lpContent;

            } while (lpTeaNode && *lpBegin=='\0');

            if (*lpBegin=='\0')
                goto Tea_GetWordLength_End;


        } else {

            lpBegin=Dos9_GetNextChar(lpBegin);

        }

        iLength++;


    }

    Tea_GetWordLength_End:

    return iLength;

}

size_t      Tea_GetWordLength(char* lpBegin)
{
    size_t iLength=0;

    while (*lpBegin
           && *lpBegin!=' '
           && *lpBegin!='\n') {

        lpBegin=Dos9_GetNextChar(lpBegin);
        iLength++;

    }

    return iLength;
}

void        Tea_MakeMargin(size_t iLength, size_t* iLeft, FILE* pFile)
{
    while (iLength) {

        putc(' ', pFile);

        iLength--;
        (*iLeft)--;

    }

}

char*       Tea_OutputWord(char* lpBegin, FILE* pFile, size_t* iLeft)
{
    while (*lpBegin
           && *lpBegin!=' '
           && *lpBegin!='\n'
           && *iLeft) {


        fputc(*lpBegin,pFile);
        lpBegin++;
        (*iLeft)--;

    }

    /* return that there's no more pending characters */
    if (!*lpBegin)
        return NULL;

    return lpBegin;
}

char*       Tea_OutputLineT(char* lpBegin, FILE* pFile, TEANODE* lpTeaNode, size_t* iLeft)
{
    size_t iNextWordLen;

    while (*lpBegin) {

        iNextWordLen=Tea_GetWordLengthT(lpBegin, lpTeaNode);

        /* the line is obviously far too big
           let the user do what he want with new lines
        */

        if (iNextWordLen >= *iLeft) return lpBegin;

        lpBegin=Tea_OutputWord(lpBegin, pFile, iLeft);

        /* if there is no pending character */
        if (!lpBegin)
            return NULL;

        /* if the line is finished, return */
        if (*lpBegin=='\n')
            return lpBegin;


        if (1 <= *iLeft) {

            fputc(' ', pFile);
            (*iLeft)--;

        }

        /* any way, the words are separated by spaces */
        lpBegin++;

    }

    return NULL;

}

char*       Tea_OutputLine(char* lpBegin, FILE* pFile, size_t* iLeft)
{
    size_t iNextWordLen;

    while (*lpBegin) {

        iNextWordLen=Tea_GetWordLength(lpBegin);

        /* the line is obviously far too big
           le the user do what he want with new lines
        */

        if (iNextWordLen >= *iLeft) return lpBegin;

        lpBegin=Tea_OutputWord(lpBegin, pFile, iLeft);

        /* if there is no pending character */
        if (!lpBegin)
            return NULL;

        /* if the line is finished, return */
        if (*lpBegin=='\n')
            return lpBegin;


        if (1 <= *iLeft) {

            fputc(' ', pFile);
            (*iLeft)--;

        }

        /* any way, the words are separated by spaces */
        lpBegin++;

    }

    return NULL;

}

