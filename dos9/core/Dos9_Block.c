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
 */

 #include <libDos9.h>

 #include "Dos9_Core.h"

 //#define DOS9_DBG_MODE
 #include "Dos9_Debug.h"

char* Dos9_GetNextBlockBegin(char* lpCh) {

    return Dos9_GetNextBlockBeginEx(lpCh, FALSE);

}

/* this is to detect nearest block openning with
   a parenthesis to lpCh. it assumes it is launched
   a command begin */
char* Dos9_GetNextBlockBeginEx(char* lpCh, int bIsBlockCmd)
{
    char *lpNextParent,
         *lpNext;

    if (!(lpNextParent=Dos9_SearchChar(lpCh, '('))) {

        /* there's no parent at all, so skip now */
        return NULL;

    }

    if (bIsBlockCmd) {

        /* check if there is a new openning right there */

        if ((lpNextParent=Dos9_SearchToken(lpCh, "(\n"))) {

            if (*lpNextParent=='(')
                return lpNextParent;


        }

        return NULL;

    }

    while (TRUE) {

        /* skip all characters before the command */

        //printf("[New token]=\"%s\"\n", lpCh);

        lpCh=Dos9_SkipAllBlanks(lpCh);

        //printf("\t=\"%s\"\n", lpCh);

        /* we are now next to the command name, try if it
           is a FOR or a IF or a top level block. */

        //printf("Examining : \"%s\"\n", lpCh);

        bIsBlockCmd=FALSE;

        if (!strnicmp(lpCh, "FOR", 3)) {

            lpCh+=3;

            if (*lpCh==' '
                || *lpCh=='\t')
                bIsBlockCmd=TRUE;


        } else if (!strnicmp(lpCh, "IF",2)) {

            lpCh+=2;

            if (*lpCh==' '
                || *lpCh=='\t')
                bIsBlockCmd=TRUE;

        } else if (*lpCh=='(') {

            /* this is a top level block, return now */
            return lpCh;

        }

        if (bIsBlockCmd) {

            //printf("This is a block command\n");

            /* look for a parenthesis that is not escaped */
            lpNextParent=Dos9_SearchChar(lpCh, '(');

            /* no more parenthesis */
            if (!lpNextParent) {

                return NULL;

            }

        }

        lpNext=Dos9_SearchToken(lpCh, "&|\n");

        if (!lpNext) {

            if (bIsBlockCmd) {

                /* well the line is obviously finished and there
                   a block right there, so return the position  */

                return lpNextParent;

            } else {

                return NULL;

            }

        } else if (bIsBlockCmd) {


            if (lpNextParent < lpNext) {

                return lpNextParent;

            }

        }

        lpCh=lpNext+1;

        if (*lpCh=='&'
            || *lpCh=='|')
            lpCh++;

    }

    /* we get at the end of the line without finding a block */

    return NULL;
}

 /* Gets the end of block
    if lpCh does not point to a '(' character, the return value is NULL
    if not end can be found the return value is NULL
 */
char* Dos9_GetNextBlockEnd(char* lpCh)
{

    char *lpBlockBegin,
         *lpNextEnd;

    /* there is no block opened return  NULL */
    if (*lpCh!='(') {

        DOS9_DBG("Not a block at \"%s\"\n",
                 lpCh);

        return NULL;

    }

    lpCh++;

    if (!(lpNextEnd=Dos9_SearchChar(lpCh, ')'))) {

        /* there is no closing parenthesis no more
           so that the block is malformed, return NULL */

        DOS9_DBG("Can't find ')' in \"%s\"\n",
                 lpCh);

        return NULL;

    }

    if (!(lpBlockBegin=Dos9_GetNextBlockBegin(lpCh))) {

        DOS9_DBG("Did not find '(' in \"%s\"\n",
                 lpCh);

        return lpNextEnd;

    }

    if (lpNextEnd > lpBlockBegin) {

        /* there's a block within the parenthesis and
           the one we guessed to be the closing parenthesis
           We have to make little of recusion */

        /* The function Dos9_GetBlockLine end takes account of
           the fact that serveral block in a line are valid.
           ie :

                if 1 equ 1 (
                    do-something
                ) else (
                    do-something
                )

            is a block despite "else" would not be recognize as
            block command (that are "for" and "if" and top-level
            opening "(" )

        */

        DOS9_DBG("looking eob at \"%s\"\n",
                 lpBlockBegin);

        do {

            if (!(lpNextEnd=Dos9_GetBlockLineEnd(lpBlockBegin))) {

                /* the block is misformed, return NULL */

                DOS9_DBG("The block is misformed...\n");

                return NULL;

            }

            lpNextEnd++;

            /* now look for a closing parenthesis */

            DOS9_DBG("Looking for ')' at \"%s\"\n",
                     lpNextEnd);

            /* loop until the block is up */

            if (!(lpCh=Dos9_SearchChar(lpNextEnd, ')'))) {

                /* the block is misformed */

                DOS9_DBG("The block is misformed...\n");

                return NULL;

            }

            if (!(lpBlockBegin=Dos9_GetNextBlockBeginEx(lpNextEnd, FALSE))) {

                /* if there is no trailing block, just
                   return the last parenthesis */

                return lpCh;

            }

            /* loop until the block is finished */

        } while (lpCh > lpBlockBegin);

        return lpCh;

    } else {

        /* its all-right since the block stands outside the next
           block openning, return the parenthesis we guessed */

        DOS9_DBG("Found lowest-level block"
                 "lpBlockBegin=\"%s\"\n"
                 "lpNextEnd=\"%s\"\n",
                 lpBlockBegin,
                 lpNextEnd
                 );

        return lpNextEnd;

    }


}

/* Get the end of a line, taking account of multiples
   blocks on a single line */
char* Dos9_GetBlockLineEnd(char* lpCh)
{

    char* lpNextBlock=NULL;

    if (*lpCh!='(')
        return NULL;

    do {

        /* if we are looping to find the
           end of the block */

        if (lpNextBlock)
            lpCh=lpNextBlock;

        /* look for the end of this specific block */

        DOS9_DBG("----------------------\n"
                 "S-block=\"%s\"\n"
                 "----------------------\n",
                 lpCh);

        if (!(lpCh=Dos9_GetNextBlockEnd(lpCh))) {

            /* the block is malformed */
            return NULL;

        }

        DOS9_DBG("----------------------\n"
                 "E-block=\"%s\"\n"
                 "----------------------\n",
                 lpCh);

        /* check if there is another block right there
           on the same line */

        lpNextBlock=Dos9_GetNextBlockBeginEx(lpCh, TRUE);


    } while (lpNextBlock);

    /* we found the right one */

    return lpCh;

}
