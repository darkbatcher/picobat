/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
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

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

int Dos9_GetParameterPointers(char** lpPBegin, char** lpPEnd, const char* lpDelims, const char* lpLine)
{
	const char  *lpBegin,
				*lpEnd;

	lpLine=Dos9_SkipBlanks(lpLine);

	if (!*lpLine) return FALSE;

	/* Here, we are on the first non-blank character
	   just search among the next characters the first
	   delimiter (or quote)*/

	lpBegin=lpLine;

	while ((lpEnd=Dos9_SearchToken(lpLine, lpDelims))) {

		if (*lpEnd=='"') {

			/* look for the next quote */
			lpEnd++;

			if ((lpLine=Dos9_SearchChar(lpEnd, '"'))) {

				lpLine++;
				continue;

			}

			/* If we have unpaired quotes, just get all remaining
			   contents */

			lpEnd=NULL;
			break;

		} else {

			/* if we have encountered a valid delimiter, just continue
			   and copy the argument */
			break;

		}

	}

	*lpPEnd = (char*)lpEnd;
	*lpPBegin = (char*)lpBegin;

	return TRUE;

}

char* Dos9_GetNextParameter(char* lpLine, char* lpResponseBuffer, int iLength)
/* determines whether a paramater follows the position lpLine.
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

/* the returning pointer is on the parenthesis
   note that the lpbkInfo->lpEnd may be NULL
   its means that the end of the block is the full
   line */
char* Dos9_GetNextBlock(char* lpLine, BLOCKINFO* lpbkInfo)
{
	char* lpBlockEnd;

	lpLine=Dos9_SkipBlanks(lpLine);

	lpBlockEnd=Dos9_GetNextBlockEnd(lpLine);

	if (*lpLine=='(')
		lpLine++;

	if (lpBlockEnd == NULL) {

		lpBlockEnd=lpLine;

		/* go to the end of line */
		while (*lpBlockEnd!='\n'
		       && *lpBlockEnd!='\0' )
			lpBlockEnd++;

	}

	lpbkInfo->lpBegin=lpLine;

	lpbkInfo->lpEnd=lpBlockEnd;

	return lpBlockEnd;

}

char* Dos9_GetBlockLine(char* pLine, BLOCKINFO* pBk)
{
    char *pEnd;

    pEnd = Dos9_GetBlockLineEnd(pLine);

    if (*pLine=='(')
        pLine++;

    pBk->lpEnd = pEnd;
    pBk->lpBegin = pLine;

    return pEnd;

}

char* Dos9_GetNextBlockEs(char* lpLine, ESTR* lpReturn)
{
	char* lpNext;
	BLOCKINFO bkInfo;

	size_t iNbBytes;

	lpNext = Dos9_GetNextBlock(lpLine, &bkInfo);

	iNbBytes = bkInfo.lpEnd - bkInfo.lpBegin;
	Dos9_EsCpyN(lpReturn, bkInfo.lpBegin, iNbBytes);

	/* replace delayed expansion */
	Dos9_DelayedExpand(lpReturn, bDelayedExpansion);

	/* remove escape characters */
	Dos9_UnEscape(Dos9_EsToChar(lpReturn));

	return lpNext+1;

}

char* Dos9_GetNextParameterEsD(char* lpLine, ESTR* lpReturn, const
				char* lpDelims)
/* This function returns the next parameter available on the
   command-line.

   It returns a pointer to the next parameter on the command
   line. If NULL is returned, then, there's no more parameters
   availiable */
{

	size_t iSize;

	char *lpBegin,
	     *lpEnd=NULL,
	     quote;

next:

	if (Dos9_GetParameterPointers(&lpBegin, &lpEnd, lpDelims, lpLine)
		==FALSE)
		return NULL;

    quote = *lpBegin;

	/* the following are gymnastics in order to remove to
	   which quotes should be remooved from the parmaters */
	if (lpEnd == NULL) {

		if (*lpBegin=='"') {

			lpBegin++;

			if ((lpLine=Dos9_SearchLastChar(lpBegin, '"'))) {

				if (*(lpLine+1)=='\0') {

					iSize=lpLine-lpBegin;
					Dos9_EsCpyN(lpReturn, lpBegin, iSize);

				} else {

					Dos9_EsCpy(lpReturn, lpBegin);

				}


			} else {

				Dos9_EsCpy(lpReturn, lpBegin);

			}

		} else {

			Dos9_EsCpy(lpReturn, lpBegin);

		}

		lpEnd=lpBegin;
		while (*lpEnd)
			lpEnd++;

	} else {

		iSize=lpEnd-lpBegin;

		if (*lpBegin=='"') {

			lpBegin++;
			iSize--;

			/* basically, if the end is '"', the previous
			   algorithm ensure us that lpEnd-lpBegin >= 1*/

			if (*(lpEnd-1)=='"') {

				iSize--;

			}

		}

		Dos9_EsCpyN(lpReturn, lpBegin, iSize);

	}

	if (*(lpReturn->str) == '^' && !*(lpReturn->str+1)) {

        lpLine = lpEnd;
        goto next;

	}

	/* expand delayed expand variable */
	Dos9_DelayedExpand(lpReturn, bDelayedExpansion);

	/* remove escape characters */
	Dos9_UnEscape(Dos9_EsToChar(lpReturn));

	return lpEnd;
}

struct PARAMLIST* Dos9_GetParamList(char* lpLine)
/*
    gets a list of parameters
*/
{
	ESTR* lpParam=Dos9_EsInit();
	ESTR* lpTemp=Dos9_EsInit();
	char* lpNext;
	struct PARAMLIST *ret = NULL, *item, *next;

	while ((lpNext = Dos9_GetNextParameterEs(lpLine, lpParam))) {

		while (*lpLine=='\t' || *lpLine==' ') lpLine++;

		if ((next = malloc(sizeof(struct PARAMLIST))) == NULL)
            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                    "Dos9_Args/Dos9_GetParamList()",
                                    -1);

        if (ret == NULL) {
            ret = (item = next);

        } else {

            item->next = next;
            item = next;

        }

        next->next = NULL;
        next->param = lpParam;

		Dos9_DelayedExpand(lpParam, bDelayedExpansion);

		lpParam=Dos9_EsInit();

		lpLine=lpNext;

	}

	Dos9_EsFree(lpParam);
	Dos9_EsFree(lpTemp);

	return ret;
}

void Dos9_FreeParamList(struct PARAMLIST* list)
{
    struct PARAMLIST* item;

    while (list) {

        Dos9_EsFree(list->param);
        item = list->next;
        free(list);
        list = item;

    }
}

LIBDOS9 char* Dos9_GetEndOfLine(char* lpLine, ESTR* lpReturn)
/* this returns fully expanded line from the lpLine Buffer */
{

	Dos9_EsCpy(lpReturn, lpLine); /* Copy the content of the line in the buffer */
	Dos9_DelayedExpand(lpReturn, bDelayedExpansion); /* Expands the content of the specified  line */

	Dos9_UnEscape(Dos9_EsToChar(lpReturn));

	return NULL;
}
