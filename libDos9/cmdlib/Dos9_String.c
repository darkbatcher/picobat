/*
 *
 *   libDos9 - The Dos9 project
 *   Copyright (C) 2010-2017 Romain GARBI
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

#include "../libDos9.h"

LIBDOS9 char* Dos9_SkipBlanks(const char* lpCh)
{
	/* Most batch scripts uses only '\t' or ' ' as delimiters. However,
	   ',' and ';' are also valid. (even if the use of those is strongly
	   discouraged */

    while (Dos9_IsDelim(*lpCh))
        lpCh++;

    if (*lpCh=='^')
        lpCh++;

    return (char*)lpCh;
}

LIBDOS9 char* Dos9_SkipAllBlanks(const char* lpCh)
{
    while (Dos9_IsDelim(*lpCh)
           || *lpCh=='@' /* this is the silent character */
           )
        lpCh++;

    return (char*)lpCh;
}

LIBDOS9 char* Dos9_SearchChar(const char* lpCh, int cChar)
{

    char* lpNxt;

    while ((lpNxt=strchr(lpCh, cChar))) {

        /* if the we are at the beginnig, of the
           string, don't search anymore and return
           lpNext */

        if (lpNxt==lpCh)
            break;

        /* else, we are further in the line, so that
           the fact that *(lpNext-1) is not a buffer
           overrun can be assumed */

        /* if the character is not escaped, use it
           as the needed character */

        if (*(lpNxt-1)!='^')
            break;

        /* the character is escaped, just loop
           back till we found the right */

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBDOS9 char* Dos9_SearchLastChar(const char* lpCh, int cChar)
{
    char *lpLastMatch=NULL,
         *lpNxt;

    while ((lpNxt=strchr(lpCh, cChar))) {

        if (lpCh==lpNxt) {

            lpLastMatch=lpNxt;

        } else {

            if (*(lpNxt-1)!='^')
                lpLastMatch=lpNxt;

        }

        lpCh=lpNxt+1;

    }

    return (char*)lpLastMatch;
}

LIBDOS9 void Dos9_UnEscape(char* lpCh)
{
    char *lpUnEscapeCh=lpCh;

    while (TRUE) {

        if (*lpCh=='^') {

            /* this should be un escaped */

            lpCh++;


            /* if the caracter escaped is a line-feed,
               don't copy the line feed */

            if (*lpCh=='\n')
                lpCh++;

        }

        if (lpUnEscapeCh!=lpCh)
            *lpUnEscapeCh=*lpCh;

        if (*lpCh=='\0')
            return;

        lpUnEscapeCh++;
        lpCh++;

    }

}

LIBDOS9 char* Dos9_GetNextNonEscaped(const char* lpCh)
{

    if (*lpCh=='^')
        lpCh++;

    if (*lpCh)
        lpCh++;

    return (char*)lpCh;
}

LIBDOS9 char* Dos9_SearchToken(const char* lpCh, const char* lpDelims)
{

    char* lpNxt;

    while ((lpNxt=strpbrk(lpCh, lpDelims))) {

        if (lpNxt==lpCh)
            break;

        if (*(lpNxt-1)!='^')
            break;

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBDOS9 char* Dos9_SearchLastToken(const char* lpCh, const char* lpDelims)
{
    const char *lpNxt,
         *lpLastOccurence=NULL;


    while ((lpNxt=strpbrk(lpCh, lpDelims))) {

        if (lpCh==lpNxt) {

            lpLastOccurence=lpCh;

        } else {

            if (*(lpNxt-1)!='^')
                lpLastOccurence=lpNxt;

        }

        lpCh=lpNxt+1;

    }

    return (char*)lpLastOccurence;
}

LIBDOS9 char* Dos9_SearchChar_OutQuotes(const char* lpCh, int cChar)
{

    char *lpNxt,
    	  *lpNextQuote;

	int i;

	lpNextQuote=Dos9_SearchChar(lpCh, '"');

    while ((lpNxt=strchr(lpCh, cChar))) {

		if ((lpNxt >= lpNextQuote)
			&& (lpNextQuote)) {

			/* Check if the sign we have found is between quotation
			   marks. If it is, we must continue to find another
			   char that matches

			*/

			/* start with an odd number of quotation marks */
			i=1;

			while ((lpNextQuote <= lpNxt) && lpNextQuote) {

				lpNextQuote = Dos9_SearchChar(lpNextQuote+1, '"');

				/* Make i having two values :

					* 0 if the number of loops is even
					* 1 if the number of loops is odd

				 */

				i=(i+1)%2 ;

			}

			if ((lpNextQuote == NULL) && (i==0)) {

				/* no more quotes to be found and the last quote is
				   unpaired. Then just assume there's no more characters to
				   to be found. */

				/* Note : (i==0) may suggest that there is an odd number of
				   quotation marks. This is not true, in fact, the last loop
				   to get (lpNextQuote == NULL) is also counted by i, thus,
				   parity are reversed if (lpNextQuote == NULL) */

				return NULL;


			/* no if to check whether the lpNextQuote is NULL and i is 0,
			   because it is the case where it is not to be processed.
			   We just have to continue the loop since there's no more
			   quotations marks after lpNxt */

			} else if ((lpNextQuote != NULL) && (i==0)) {

				/* if we are between two quotes, restart loop and
				   change the last quote token */

				lpCh=lpNextQuote+1;

				lpNextQuote=Dos9_SearchChar(lpCh, '"');

				continue;

			}
		}


        /* if the we are at the beginnig, of the
           string, don't search anymore and return
           lpNext */

        if (lpNxt==lpCh)
            break;

        /* else, we are further in the line, so that
           the fact that *(lpNext-1) is not a buffer
           overrun can be assumed */

        /* if the character is not escaped, use it
           as the needed character */

        if (*(lpNxt-1)!='^')
            break;

        /* the character is escaped, just loop
           back until we find the right */

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBDOS9 char* Dos9_SearchToken_OutQuotes(const char* lpCh, const char* lpDelims)
{

    char *lpNxt,
		 *lpNextQuote;

	int i;

	lpNextQuote=Dos9_SearchChar(lpCh, '"');

    while ((lpNxt=strpbrk(lpCh, lpDelims))) {

		if ((lpNxt >= lpNextQuote)
			&& (lpNextQuote != NULL)) {

			/* Check if the sign we have found is between quotation
			   marks. If it is, we must continue to find another
			   char that matches

			*/

			/* start with an odd number of quotation marks */
			i=1;

			while ((lpNextQuote <= lpNxt) && lpNextQuote) {

				lpNextQuote = Dos9_SearchChar(lpNextQuote+1, '"');

				/* Make i having two values :

					* 0 if the number of loops is even
					* 1 if the number of loops is odd

				 */

				i=(i+1)%2 ;

			}

			if ((lpNextQuote == NULL) && (i==0)) {

				/* no more quotes to be found and the last quote is
				   unpaired. Then just assume there's no more characters to
				   to be found. */

				/* Note : (i==0) may suggest that there is an odd number of
				   quotation marks. This is not true, in fact, the last loop
				   to get (lpNextQuote == NULL) is also counted by i, thus,
				   parity are reversed if (lpNextQuote == NULL) */

				return NULL;


			/* no need to check whether the lpNextQuote is NULL and i is 0,
			   because it is the case where it is not to be processed.
			   We just have to continue the loop since there's no more
			   quotations marks after lpNxt */

			} else if ((lpNextQuote != NULL) && (i==0)) {

				/* if we are between two quotes, restart loop and
				   change the last quote token */

				lpCh=lpNextQuote+1;

				lpNextQuote=Dos9_SearchChar(lpCh, '"');

				continue;

			}
		}

        if (lpNxt==lpCh)
            break;

        if (*(lpNxt-1)!='^')
            break;

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBDOS9 char* Dos9_SearchToken_Hybrid(const char* pch, const char* delims, const char* qdelims)
{
    char *qres, *res;

    res = Dos9_SearchToken(pch, delims);
    qres = Dos9_SearchToken_OutQuotes(pch, qdelims);

    if ((res == NULL) && (qres == NULL))
        return NULL;

    if ((qres != NULL) && ((res == NULL)
         || (res > qres))) {

        return qres;

    } else {

        return res;

    }
}

LIBDOS9 void Dos9_StripEndDelims(char* str)
{
    char* last = NULL;

    while (*str) {

        if (!Dos9_IsDelim(*str))
            last = NULL;
        else if (last == NULL)
            last = str;

        str++;
    }

    if (last)
        *last = '\0';
}
