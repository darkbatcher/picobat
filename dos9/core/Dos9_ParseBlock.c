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

#include <strings.h>

#include <libDos9.h>

#include "Dos9_Core.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

/* Parse the blocks. The syntax for blocks is the following

    block : ( "if" | "for" ) ( text ( "(" block ")" ))* terminal
            "(" block ")" terminal
    text : [^)^(]*
    terminal : "&" | "|" | "\n"

*/


/* Get the total block line (a line).

   If pch directly points to a '(', this is strictly equivalent to
   Dos9_GetNextBlockEnd(). If pch does not point to a '(', then it
   looks for a full line of command taking account of blocks.

   ** Only returns null if a block happens to be malformed **
 */
char* Dos9_GetBlockLineEndEx(char* pch, int par_end)
{
    char* next;

    /* check this actually a block (ie. that it actually starts
       with a parenthesis or FOR or IF */

    /* Skip everything, including the optionnal @ character */
    pch = Dos9_SkipAllBlanks(pch);

    if (*pch == '(') {

        //(stderr, "**** Detected top level block=%s\n", pch);
        next = Dos9_GetNextBlockEnd(pch);
        //fprintf(stderr, "**** Detected top level end=%s\n", next);
        /* This is a top level block */
        if (next && *next)
            next ++;

        return next;

    } else if (strnicmp(pch, "if", 2)
               && strnicmp(pch, "for", 3)) {

        /* if there is no if or for, nor (, return the
           terminal character (here it is just \n to be able to
           check line integrity at reading) */

        if ((next = Dos9_SearchToken_Hybrid(pch, "\n",
                                           par_end ? ")&|" : "&|")) == NULL) {
            /* apparently, neither of theses, so the lines terminates
               near here by a '\0'. Thus look for the end of the string */

            while (*pch)
                pch++;

            return pch;

        }

        return next;


    }

    do {

        /* here, we can some random text followed by either of the
           following cases :

                '(' : Opening parenthesis meaning the begining of a
                      sub-block.

                ')' : Closing parenthesis meaning end of the current
                block

                Any of the terminals symbols, meaning the line has
                ended (either that the block was malformed or not)

         */

        if ((next = Dos9_SearchToken_Hybrid(pch, "\n",
                                                par_end ? "()&|" : "(&|")) == NULL) {
            /* apparently, neither of theses, so the lines terminates
               near here by a '\0'. Thus look for the end of the string */

            while (*pch)
                pch++;

            return pch;

        }


        switch (*next) {

            case '(':
                /* look for the sub-block end */

                /* the block is clearly malformed */
                //printf("Sub-block reached at :%s\n", next);
                if (((pch = Dos9_GetNextBlockEnd(next)) == NULL)
                    || (*pch != ')'))
                    return NULL;

                if (*pch)
                    pch ++;

                break;

            case ')':
            case '\n':
            case '&':
            case '|':
                return next;
        }

    } while (pch && *pch);

    /* block ends on a '\0' */
    return pch;
}



/* Get the end of a block.

    returns NULL if pch is not pointing to a '(' */
char* Dos9_GetNextBlockEnd(char* pch)
{
    if (*pch != '(') {

        /* this is not really a block, return NULL */
        return NULL;

    }

    pch ++;

    do {

        if ((pch = Dos9_GetBlockLineEndEx(pch, 1)) == NULL)
            return NULL;

        if (*pch !='\0' && *pch != ')')
            pch ++;

    } while (*pch && *pch != ')');

    /* arguably, this should finish finish with a ')' so this is
       obviously malformed*/
    if (*pch =='\0')
        return NULL;

    //printf("*** End block reached at : %s\n", pch);

    return pch;
}

/* Get the next starting block start

   return NULL if no block is encountered in the current block */
char* Dos9_GetNextBlockBeginEx(char* pch, int bIsBlockCmd)
{
    pch = Dos9_SearchToken_Hybrid(pch, "", "()");

    if ((pch != NULL) && (*pch == '('))
            return pch;

    return NULL;
}
