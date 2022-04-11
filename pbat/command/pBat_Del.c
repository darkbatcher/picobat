/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Commands.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

#include "pBat_Rmdir.h"
#include "pBat_Del.h"

#include "pBat_Ask.h"

int pBat_CmdDel(char* lpLine)
{
	char *lpToken;
	ESTR *lpEstr=pBat_EsInit_Cached();

    char **name,
         **tmp;

    size_t namesize = 64;

	char  param=0;

	short attr=PBAT_CMD_ATTR_ALL;

	int flag=PBAT_SEARCH_NO_PSEUDO_DIR
            | PBAT_SEARCH_DIR_MODE,
	    choice = PBAT_ASK_ALL,
	    status = PBAT_NO_ERROR,
	    n=0,
	    i;

	FILELIST *next=NULL,
             *end,
             *list=NULL;

    if (!strnicmp(lpLine, "DEL", 3)) {
        lpLine += 3; /* This is DEL */
    } else {
        lpLine += 5; /* This is erase */
    }

    /* start by trying to malloc memory for names */
    if ((name = malloc(namesize * sizeof(ESTR*))) == NULL) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                        __FILE__ "/pBat_CmdDel()", 0);
        status = PBAT_FAILED_ALLOCATION;
        goto end;

    }

	while ((lpLine=pBat_GetNextParameterEs(lpLine, lpEstr))) {

		lpToken=pBat_EsToChar(lpEstr);

		if (!stricmp(lpToken, "/P")) {

			/* Ask before suppressing */
			param|=PBAT_ASK_CONFIRMATION;

		} else if (!stricmp(lpToken, "/F")) {

			/* do not ignore read-only files */
			param|=PBAT_DELETE_READONLY;

		} else if (!stricmp(lpToken, "/S")) {

			/*  recursive */
			flag|=PBAT_SEARCH_RECURSIVE;
			param|=PBAT_ASK_CONFIRMATION;

		} else if (!stricmp(lpToken, "/Q")) {

			/* no confirmation with use of joker */
			param|=PBAT_DONT_ASK_GENERIC;

		} else if (!strnicmp(lpToken, "/A", 2)) {

			/* attributes managment */
			lpToken+=2;
			if (*lpToken==':')
                lpToken++;

			attr=pBat_MakeFileAttributes(lpToken);

		} else if (!strcmp("/?", lpToken)) {

			pBat_ShowInternalHelp(PBAT_HELP_DEL);
			goto end;

		} else {

            if (n == namesize) {
                /* we dont have enough space in the array, thus try to
                   realloc */

                namesize *= 2;

                if ((tmp = realloc(name, namesize * sizeof(ESTR*))) == NULL) {

                    pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                        __FILE__ "/pBat_CmdDel()", 0);
                    status = PBAT_FAILED_ALLOCATION;
                    goto end;

                }

                name = tmp;

            }

            if ((name[n] = pBat_FullPathDup(lpToken)) == NULL) {

                pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                        __FILE__ "/pBat_CmdDel()", 0);
                status = PBAT_FAILED_ALLOCATION;
                goto end;

            }

            n++;

		}

	}

	if (n == 0) {

        pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "DEL/ERASE", FALSE);
        status = -1;
        goto end;

	}

	for  (i=0; i < n; i++) {

        if (!(next = pBat_GetMatchFileList(name[i], flag))) {

            pBat_ShowErrorMessage(PBAT_NO_MATCH,
                                    name[i],
                                    FALSE);
            status = PBAT_NO_MATCH;

            goto end;
        }

        if (strpbrk(name[i], "*?") != NULL)
            param |= PBAT_USE_JOKER;

        if (list == NULL) {

            list = next;

        } else {

            end->lpflNext = next;

        }

        while (next->lpflNext)
            next = next ->lpflNext;

        end = next;

	}


    if (attr == 0) {
        /* Set the default attributes based for the search */
        if (!(param & PBAT_DELETE_READONLY))
            attr |= PBAT_ATTR_NO_READONLY;

        if (!(flag  & PBAT_SEARCH_RECURSIVE))
            attr|= PBAT_ATTR_NO_DIR;

    }

	if ((param & PBAT_USE_JOKER)
	    && !(param & PBAT_DONT_ASK_GENERIC)) {
        /* If one of the path given included Joker and /Q has not been
            specified, ask the user for the file to *really delete */
		param|=PBAT_ASK_CONFIRMATION;
		choice = 0;
	}

    pBat_AttributesSplitFileList(attr,
                                 list,
                                 &list,
                                 &end
                                 );
    pBat_FreeFileList(end);

    pBat_AttributesSplitFileList(PBAT_ATTR_NO_DIR,
                                 list,
                                 &list,
                                 &next
                                 );

    pBat_FreeFileList(next);

    end = list;
    while (end) {

        if (!(param & PBAT_DELETE_READONLY)
            && (pBat_GetFileMode(end) & PBAT_FILE_READONLY)) {

            /* Skip read-only files if the user did not specified 'force' */
            end = end->lpflNext;
            continue;

        }

        status |= pBat_CmdDelFile(end->lpFileName, pBat_GetFileMode(end),
                                                                param, &choice);
        end = end->lpflNext;

    }

end:

    if (name) {

        for (i=0; i < n; i++)
            free(name[i]);

        free(name);

    }

    pBat_FreeFileList(list);

	pBat_EsFree_Cached(lpEstr);
	return status;
}

int pBat_CmdDelFile(char* file, int fmode, int param, int* choice)
{
    int res = *choice;

    if ((res == 0) && (param & PBAT_ASK_CONFIRMATION)) {

				res=pBat_AskConfirmation(PBAT_ASK_YNA
				                             | PBAT_ASK_INVALID_REASK
				                             | PBAT_ASK_DEFAULT_N, NULL,
				                             lpDelConfirm,
				                             file
				                            );
        if (res == PBAT_ASK_ALL)
            *choice = res;

    }

    if (fmode & PBAT_FILE_READONLY) {

        /* if the file is read-only, windows will not let you delete it,
           for this, adjusting permissions is somehow needed.

           This does not apply to unix like files systems, that "manage"
           deleting permission by parent directory permissions */
        pBat_SetFileMode(file, fmode & ~PBAT_FILE_READONLY);

    }

    if ((res == PBAT_ASK_ALL) || (res == PBAT_ASK_YES)) {

        return pBat_DelFile(file);

    }

    return 0;
}

int pBat_DelFile(const char* file)
{
    if (remove(file)) {

        pBat_ShowErrorMessage(PBAT_UNABLE_DELETE | PBAT_PRINT_C_ERROR,
                                file,
                                0);

        return -1;

    }

    return 0;
}
