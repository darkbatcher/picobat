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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Commands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "Dos9_Rmdir.h"
#include "Dos9_Del.h"

#include "Dos9_Ask.h"

int Dos9_CmdDel(char* lpLine)
{
	char *lpToken;
	ESTR *lpEstr=Dos9_EsInit();

    char **name,
         **tmp;

    size_t namesize = 64;

	char  param=0;

	short attr=DOS9_CMD_ATTR_ALL;

	int flag=DOS9_SEARCH_NO_PSEUDO_DIR,
	    choice = DOS9_ASK_ALL,
	    status = 0,
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

        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                        __FILE__ "/Dos9_CmdDel()", 0);
        status = -1;
        goto end;

    }

	while ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		lpToken=Dos9_EsToChar(lpEstr);

		if (!stricmp(lpToken, "/P")) {

			/* Ask before suppressing */
			param|=DOS9_ASK_CONFIRMATION;

		} else if (!stricmp(lpToken, "/F")) {

			/* do not ignore read-only files */
			param|=DOS9_DELETE_READONLY;

		} else if (!stricmp(lpToken, "/S")) {

			/*  recursive */
			flag|=DOS9_SEARCH_RECURSIVE;
			param|=DOS9_ASK_CONFIRMATION;

		} else if (!stricmp(lpToken, "/Q")) {

			/* no confirmation with use of joker */
			param|=DOS9_DONT_ASK_GENERIC;

		} else if (!strnicmp(lpToken, "/A", 2)) {

			/* attributes managment */
			lpToken+=2;
			if (*lpToken==':')
                lpToken++;

			attr=Dos9_MakeFileAttributes(lpToken);

		} else if (!strcmp("/?", lpToken)) {

			Dos9_ShowInternalHelp(DOS9_HELP_DEL);
			goto end;

		} else {

            if (n == namesize) {
                /* we dont have enough space in the array, thus try to
                   realloc */

                namesize *= 2;

                if ((tmp = realloc(name, namesize * sizeof(ESTR*))) == NULL) {

                    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                        __FILE__ "/Dos9_CmdDel()", 0);
                    status = -1;
                    goto end;

                }

                name = tmp;

            }

            if ((name[n] = Dos9_FullPathDup(lpToken)) == NULL) {

                Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                        __FILE__ "/Dos9_CmdDel()", 0);
                status = -1;
                goto end;

            }

            n++;

		}

	}

	if (n == 0) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "DEL/ERASE", FALSE);
        status = -1;
        goto end;

	}

	for  (i=0; i < n; i++) {

        if (!(next = Dos9_GetMatchFileList(name[i], flag))) {

            Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                    name[i],
                                    FALSE);
            status = -1;

            goto end;
        }

        if (strpbrk(name[i], "*?") != NULL)
            param |= DOS9_USE_JOKER;

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
        if (!(param & DOS9_DELETE_READONLY))
            attr |= DOS9_CMD_ATTR_READONLY_N | DOS9_CMD_ATTR_READONLY;

        if (!(flag  & DOS9_SEARCH_RECURSIVE))
            attr|=DOS9_CMD_ATTR_DIR | DOS9_CMD_ATTR_DIR_N;

    }

	if ((param & DOS9_USE_JOKER)
	    && !(param & DOS9_DONT_ASK_GENERIC)) {
        /* If one of the path given included Joker and /Q has not been
            specified, ask the user for the file to *really delete */
		param|=DOS9_ASK_CONFIRMATION;
		choice = 0;
	}

    Dos9_AttributesSplitFileList(attr,
                                 list,
                                 &list,
                                 &end
                                 );
    Dos9_FreeFileList(end);

    Dos9_AttributesSplitFileList(DOS9_ATTR_NO_DIR,
                                 list,
                                 &list,
                                 &next
                                 );

    Dos9_FreeFileList(next);

    end = list;
    while (end) {

        if (!(param & DOS9_DELETE_READONLY)
            && (Dos9_GetFileMode(end) & DOS9_FILE_READONLY)) {

            /* Skip read-only files if the user did not specified 'force' */
            end = end->lpflNext;
            continue;

        }

        status |= Dos9_CmdDelFile(end->lpFileName, Dos9_GetFileMode(end),
                                                                param, &choice);
        end = end->lpflNext;

    }

end:

    if (name) {

        for (i=0; i < n; i++)
            free(name[i]);

        free(name);

    }

    Dos9_FreeFileList(list);

	Dos9_EsFree(lpEstr);
	return status;
}

int Dos9_CmdDelFile(char* file, int fmode, int param, int* choice)
{
    int res = *choice;

    if ((res == 0) && (param & DOS9_ASK_CONFIRMATION)) {

				res=Dos9_AskConfirmation(DOS9_ASK_YNA
				                             | DOS9_ASK_INVALID_REASK
				                             | DOS9_ASK_DEFAULT_N,
				                             lpDelConfirm,
				                             file
				                            );
        if (res == DOS9_ASK_ALL)
            *choice = res;

    }

    if (fmode & DOS9_FILE_READONLY) {

        /* if the file is read-only, windows will not let you delete it,
           for this, adjusting permissions is somehow needed.

           This does not apply to unix like files systems, that "manage"
           deleting permission by parent directory permissions */
        Dos9_SetFileMode(file, fmode & ~DOS9_FILE_READONLY);

    }

    if ((res == DOS9_ASK_ALL) || (res == DOS9_ASK_YES)) {

        return Dos9_DelFile(file);

    }

    return 0;
}

int Dos9_DelFile(const char* file)
{
    if (remove(file)) {

        Dos9_ShowErrorMessage(DOS9_UNABLE_DELETE | DOS9_PRINT_C_ERROR,
                                file,
                                0);

        return -1;

    }

    return 0;
}


