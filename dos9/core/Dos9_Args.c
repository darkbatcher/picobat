/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <string.h>
#include <libDos9.h>

#include "Dos9_Core.h"

LIBDOS9 char* Dos9_GetNextParameter(char* lpLine, char* lpResponseBuffer, int iLength)
/* determines wheter a paramater follows the position lpLinLIBDOS9LIBDOS9LIBDOS9LIBDOS9e.
 *
 * lpLine : A pointer to where to seek a parameter
 * lpResponse : A buffer to store the parameter
 * iLenght : The maximum length to be stored in lpResponse
 *
 * return : the part after the parameter or null if no parameter has been found
 *
 */
 {
     ESTR* lpParameter=Dos9_EsInit();

     lpLine=Dos9_GetNextParameterEs(lpLine, lpParameter);
     strncpy(lpResponseBuffer, Dos9_EsToChar(lpParameter), iLength);
     lpResponseBuffer[iLength-1]='\0';

     Dos9_EsFree(lpParameter);
     return lpLine;
 }

/* the returning pointer is on the parenthesis */
LIBDOS9 char* Dos9_GetNextBlock(char* lpLine, BLOCKINFO* lpbkInfo)
{
    char* lpBlockBegin;
    int iParentheseLvl=0;

    while (*lpLine=='\t' || *lpLine==' ') lpLine++;

    if (*lpLine=='(') {

        /* the block might begin here */

        lpLine++;
        lpBlockBegin=lpLine;

        while (*lpLine && (*lpLine!=')' || iParentheseLvl)) {

            /* parenthesis may contain enclosed parenthesis levels,
               skip them */

            if (*lpLine=='(') iParentheseLvl++;
            if (*lpLine==')' && iParentheseLvl>0) iParentheseLvl--;
            lpLine++;

        }

    } else {

        /* The blocks continues until the next carriage return */
        lpBlockBegin=lpLine;
        while (*lpLine && *lpLine!='\n') lpLine++;
    }

    lpbkInfo->lpBegin=lpBlockBegin;
    lpbkInfo->lpEnd=lpLine;

    return lpLine;

}

LIBDOS9 char* Dos9_GetNextBlockEs(char* lpLine, ESTR* lpReturn)
{
    char* lpNext;
    BLOCKINFO bkInfo;

    size_t iNbBytes;

    lpNext = Dos9_GetNextBlock(lpLine, &bkInfo);


    iNbBytes = bkInfo.lpEnd - bkInfo.lpBegin;

    Dos9_EsCpyN(lpReturn, bkInfo.lpBegin, iNbBytes);

    Dos9_DelayedExpand(lpReturn, bDelayedExpansion);

    return lpNext+1;

}

LIBDOS9 char* Dos9_GetNextParameterEs(char* lpLine, ESTR* lpReturn)
{
     int iSeekQuote=FALSE;
     int i=0;
     char* lpStartParameter=NULL;

     while (*lpLine==' ' || *lpLine=='\t') lpLine++;

     if (*lpLine=='"') {
         iSeekQuote=1;
         lpLine++;
     } else if (*lpLine=='\'') {
         iSeekQuote=2;
         lpLine++;
     }

     if (!*lpLine) return NULL;

     lpStartParameter=lpLine;

     for (;*lpLine;lpLine++) {

         i++;

         if (((*lpLine=='"') && (iSeekQuote==1)) || (*lpLine=='\'' && iSeekQuote == 2)) {
             /* if we were seeking a quote */

             if (*(lpLine+1)!='\t' && *(lpLine+1)!=' ') continue; /* if the following character is neither a
                                                                    a space nor a tab, then it continues */
             break;
         }

         if ((*lpLine==' ') & (!iSeekQuote)) break; /* if the programs seeks only a space
                                                       then it breaks */

     }

     if (!(*lpLine=='\0') || iSeekQuote) {

        if (*lpLine)
            lpLine++;
        i--; /* i is obviously strictly greater
                than 1 */

     }

     Dos9_EsCpyN(lpReturn, lpStartParameter, i);

     Dos9_DelayedExpand(lpReturn, bDelayedExpansion); /* Note : delayed expansion means here all vars expansion
                                                        peformed after the line first buffering from the file,
                                                        thus, it includes also expansion of special-local vars,
                                                        such as %1 */

     return lpLine;
}

LIBDOS9 int   Dos9_GetParamArrayEs(char* lpLine, ESTR** lpArray, size_t iLenght)
/*
    gets command-line argument in an array of extended string
*/
{
    size_t iIndex=0;
    ESTR* lpParam=Dos9_EsInit();
    ESTR* lpTemp=Dos9_EsInit();
    char* lpNext;

    while ((iIndex < iLenght) && (lpNext = Dos9_GetNextParameterEs(lpLine, lpParam))) {

        while (*lpLine=='\t' || *lpLine==' ') lpLine++;

        if (*lpLine == '"') {

            /* if the first character are '"', then
               report it back in the command arguments,
               since some microsoft commands would not
               work without these */

            Dos9_EsCpy(lpTemp, "\"");
            Dos9_EsCatE(lpTemp, lpParam);
            Dos9_EsCat(lpTemp, "\"");

            Dos9_EsCpyE(lpParam, lpTemp);

        }


        lpArray[iIndex]=lpParam;

        Dos9_DelayedExpand(lpParam, bDelayedExpansion);

        lpParam=Dos9_EsInit();

        lpLine=lpNext;

        iIndex++;
    }

    Dos9_EsFree(lpParam);
    Dos9_EsFree(lpTemp);


    while (iIndex < iLenght) {

        lpArray[iIndex] = NULL;

        iIndex++;

    }

    return 0;
}

LIBDOS9 char* Dos9_GetEndOfLine(char* lpLine, ESTR* lpReturn)
/* this returns fully expanded line from the lpLine Buffer */
{

    Dos9_EsCpy(lpReturn, lpLine); /* Copy the content of the line in the buffer */
    Dos9_DelayedExpand(lpReturn, bDelayedExpansion); /* Expands the content of the specified  line */

    return NULL;
}
