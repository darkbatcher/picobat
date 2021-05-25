/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2015 DarkBatcher
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

/* RD [/S] [/Q] dir ...
   RMDIR [/S] [/Q] dir ...

   removes the 'dir' directory. It may also suppress severals directories specified in a row.

   /S : suppress subfolders and subfiles.
   /Q : Quiet for while browsing subfolders.

   */

int pBat_CmdRmdir(char* lpLine)
{
	ESTR *lpEstr=pBat_EsInit();

	int	mode=PBAT_SEARCH_DEFAULT | PBAT_SEARCH_NO_PSEUDO_DIR,
		param=PBAT_ASK_CONFIRMATION,
		choice=0,
		n=0,
		status = 0,
		i;

    char **name,
         **tmp;

    size_t namesize = 64;

    FILELIST *next,
             *dir,
             *files = NULL;

    if (!strnicmp(lpLine, "RD", 2)) {

        lpLine +=2; /* RD */

    } else {

        lpLine +=5; /* RMDIR */
    }

    if ((name = malloc(namesize * sizeof(char*))) == NULL) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                        __FILE__ "/pBat_CmdRmdir()", 0);
        status = PBAT_FAILED_ALLOCATION;

        goto end;

    }

	while ((lpLine=pBat_GetNextParameterEs(lpLine, lpEstr))) {

		if (!strcmp(pBat_EsToChar(lpEstr), "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_RD);
			goto end;

		} else if (!stricmp(pBat_EsToChar(lpEstr),"/S")) {

			/* FIXME : Empty the given directory and then try to suppress
			   the directory */

			mode|=PBAT_SEARCH_RECURSIVE;

		} else if (!stricmp(pBat_EsToChar(lpEstr), "/Q")) {

			param = 0;
			choice = PBAT_ASK_ALL;

		} else {

            if (n  == namesize) {
                /* It is time to realloc name array, we ran out of space */
                namesize *= 2;

                if ((tmp = realloc(name, namesize * sizeof(char*))) == NULL) {

                    pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                                    __FILE__ "/pBat_CmdRmdir()", 0);
                    status = PBAT_FAILED_ALLOCATION;

                    goto end;

                }

                name = tmp;

            }

            if ((name[n] = pBat_FullPathDup(lpEstr->str)) == NULL) {

                pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                                __FILE__ "/pBat_CmdRmdir()", 0);
                status = PBAT_FAILED_ALLOCATION;

                goto end;

            }

            n ++;

		}

	}

    if (!(mode & PBAT_SEARCH_RECURSIVE) && n == 1) {

        /* This is simple case where we just need to remove the dir */
        choice = PBAT_ASK_ALL;
        pBat_CmdRmdirFile(name[0], param, &choice);

    } else if (n == 0) {

        pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "RD/RMDIR", FALSE);
        status = PBAT_EXPECTED_MORE;

        goto end;

    } else {

        for (i=0; i < n; i++) {

            if (!(dir = pBat_GetMatchFileList(name[i], mode))) {

                pBat_ShowErrorMessage(PBAT_NO_MATCH,
                                        name[i],
                                        FALSE);
                status = PBAT_NO_MATCH;

                goto end;
            }

            if (files == NULL) {

                files = dir;
                next = dir;

            } else {

                next->lpflNext = dir;

            }

            while (next->lpflNext)
                next=next->lpflNext;

        }

        pBat_AttributesSplitFileList(PBAT_CMD_ATTR_DIR,
                                        files,
                                        &dir,
                                        &files
                                        );

        next = files;

        while (next) {


            status |= pBat_CmdDelFile(next->lpFileName, pBat_GetFileMode(next),
                                                                    param, &choice);

            next = next->lpflNext;

        }

        next = dir;

        while (next) {

            status |= pBat_CmdRmdirFile(next->lpFileName, param, &choice);
            next = next->lpflNext;

        }

        pBat_FreeFileList(dir);
        pBat_FreeFileList(files);

    }

end:
    if (name) {

        for (i=0;i < n; i++)
            free(name[i]);

        free(name);

    }

	pBat_EsFree(lpEstr);
	return status;

}

int pBat_CmdRmdirFile(char* dir, int param, int* choice)
{
    int res = *choice;

    if ((res == 0) && (param & PBAT_ASK_CONFIRMATION)) {

				res=pBat_AskConfirmation(PBAT_ASK_YNA
				                             | PBAT_ASK_INVALID_REASK
				                             | PBAT_ASK_DEFAULT_N, NULL,
				                             lpRmdirConfirm,
				                             dir
				                            );
        if (res == PBAT_ASK_ALL)
            *choice = res;

    }

    if ((res == PBAT_ASK_ALL) || (res == PBAT_ASK_YES)) {

        return pBat_Rmdir(dir);

    }

    return 0;

}

int pBat_Rmdir(const char* dir)
{

    if (rmdir(dir)) {

        pBat_ShowErrorMessage(PBAT_UNABLE_RMDIR
                                | PBAT_PRINT_C_ERROR, dir, 0);

        return PBAT_UNABLE_RMDIR;

    }

    return 0;

}
