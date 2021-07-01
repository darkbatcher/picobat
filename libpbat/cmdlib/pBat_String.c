/*
 *
 *   libpBat - The pBat project
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

#include "../libpBat.h"

LIBPBAT char* pBat_SkipBlanks(const char* lpCh)
{
	/* Most batch scripts uses only '\t' or ' ' as delimiters. However,
	   ',' and ';' are also valid. (even if the use of those is strongly
	   discouraged */

    while (pBat_IsDelim(*lpCh))
        lpCh++;

     /* if (*lpCh=='^')
       lpCh++; */

    return (char*)lpCh;
}

LIBPBAT char* pBat_SkipAllBlanks(const char* lpCh)
{
    while (pBat_IsDelim(*lpCh)
           || *lpCh=='@' /* this is the silent character */
           )
        lpCh++;

    return (char*)lpCh;
}

LIBPBAT char* pBat_SearchChar(const char* lpCh, int cChar)
{
    char* lpNxt;
    char ok;
    int i;

    while ((lpNxt=strchr(lpCh, cChar))) {

        /* if the character is not escaped, use it
           as the needed character */
        i = 1;
        ok = 1;

        while(lpNxt - i >= lpCh) {

            if (*(lpNxt - i) != '^')
                break;

            ok = !ok;
            i ++;

        }

        if (ok)
            break;

        /* the character is escaped, just loop
           back till we found the right */

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBPBAT char* pBat_SearchLastChar(const char* lpCh, int cChar)
{
    char *lpLastMatch=NULL,
         *lpNxt;
    char ok;
    int i;

    while ((lpNxt=strchr(lpCh, cChar))) {

        i = 1;
        ok = 1;

        while(lpNxt - i >= lpCh) {

            if (*(lpNxt - i) != '^')
                break;

            ok = !ok;
            i ++;

        }

        if (ok)
            lpLastMatch = lpNxt;

        lpCh=lpNxt+1;

    }

    return (char*)lpLastMatch;
}

LIBPBAT void pBat_UnEscape(char* lpCh)
{
    char *un = lpCh, *ch, *next;

    if ((ch = strchr(un, '^')) == NULL)
        return;

    un = ch ++;

    do {

        if (*ch == '\0'
            || (next = strchr(ch + 1, '^')) == NULL) {

            memmove(un, ch, strlen(ch) + 1);
            return;

        }

        memmove(un, ch, next-ch);

        un += next-ch;
        ch = next + 1;

    } while(1);
}

LIBPBAT char* pBat_GetNextNonEscaped(const char* lpCh)
{

    if (*lpCh=='^')
        lpCh++;

    if (*lpCh)
        lpCh++;

    return (char*)lpCh;
}

LIBPBAT char* pBat_SearchToken(const char* restrict lpCh, const char* restrict lpDelims)
{

    char* lpNxt;
    char ok;
    int i;

    while ((lpNxt=strpbrk(lpCh, lpDelims))) {

        i = 1;
        ok = 1;

        while(lpNxt - i >= lpCh) {

            if (*(lpNxt - i) != '^')
                break;

            ok = !ok;
            i ++;

        }

        if (ok)
            break;

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBPBAT char* pBat_SearchLastToken(const char* restrict lpCh, const char* restrict lpDelims)
{
    const char *lpNxt,
         *lpLastOccurence=NULL;

    char ok;
    int i;

    while ((lpNxt=strpbrk(lpCh, lpDelims))) {

        i = 1;
        ok = 1;

        while(lpNxt - i >= lpCh) {

            if (*(lpNxt - i) != '^')
                break;

            ok = !ok;
            i ++;

        }

        if (ok)
            lpLastOccurence = lpNxt;

        lpCh=lpNxt+1;

    }

    return (char*)lpLastOccurence;
}

LIBPBAT char* pBat_SearchChar_OutQuotes(const char* lpCh, int cChar)
{

    char *lpNxt,
    	  *lpNextQuote;

	int i;
    char ok;
    int j;

	lpNextQuote=pBat_SearchChar(lpCh, '"');

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

				lpNextQuote = pBat_SearchChar(lpNextQuote+1, '"');

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

				lpNextQuote=pBat_SearchChar(lpCh, '"');

				continue;

			}
		}


        /* if the we are at the beginnig, of the
           string, don't search anymore and return
           lpNext */

        j = 1;
        ok = 1;

        while(lpNxt - j >= lpCh) {

            if (*(lpNxt - j) != '^')
                break;

            ok = !ok;
            j ++;

        }

        if (ok)
            break;

        /* the character is escaped, just loop
           back until we find the right */

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBPBAT char* pBat_SearchToken_OutQuotes(const char* restrict lpCh, const char* restrict lpDelims)
{

    char *lpNxt,
		 *lpNextQuote;

	int i;
    char ok;
    int j;

	lpNextQuote=pBat_SearchChar(lpCh, '"');

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

				lpNextQuote = pBat_SearchChar(lpNextQuote+1, '"');

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

				/* Note : (i==0) may suggest that there is an even number of
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

				lpNextQuote=pBat_SearchChar(lpCh, '"');

				continue;

			}
		}

        j = 1;
        ok = 1;

        while(lpNxt - j >= lpCh) {

            if (*(lpNxt - j) != '^')
                break;

            ok = !ok;
            j ++;

        }

        if (ok)
            break;

        lpCh=lpNxt+1;

    }

    return (char*)lpNxt;

}

LIBPBAT char* pBat_SearchToken_Hybrid(const char* restrict pch, const char* restrict delims, const char* restrict qdelims)
{
    char *qres, *res;

    res = pBat_SearchToken(pch, delims);
    qres = pBat_SearchToken_OutQuotes(pch, qdelims);

    if ((res == NULL) && (qres == NULL))
        return NULL;

    if ((qres != NULL) && ((res == NULL)
         || (res > qres))) {

        return qres;

    } else {

        return res;

    }
}

LIBPBAT void pBat_StripEndDelims(char* str)
{
    char* last = NULL;

    while (*str) {

        if (!pBat_IsDelim(*str))
            last = NULL;
        else if (last == NULL)
            last = str;

        str++;
    }

    if (last)
        *last = '\0';
}
