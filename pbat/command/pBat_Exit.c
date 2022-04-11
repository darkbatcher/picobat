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

#include "pBat_Exit.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

/* exit [/b] [code]

    Exit the interpretor.

        - /b : Exit only the current execution level (subroutine)

        - code : A code to be returned upon exit

    if code is not specified, leave ERRORLEVEL unaffected.

 */

int pBat_CmdExit(char* lpLine)
{
    ESTR* param=pBat_EsInit_Cached();
    char* ntoken;
    int ret=iErrorLevel;

	if ((ntoken=pBat_GetNextParameterEs(lpLine+4, param))) {

		if (!stricmp(param->str, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_EXIT);
			pBat_EsFree_Cached(param);
			return 0;

		} else if (!stricmp(param->str, "/b")) {

            /* /b allow resuming to upper execution level, with or without
               specifying a return value in ERRORLEVEL */

            if ((ntoken = pBat_GetNextParameterEs(ntoken, param))) {

                if (*ntoken) {

                    pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, ntoken, 0);
                    pBat_EsFree_Cached(param);
                    return PBAT_UNEXPECTED_ELEMENT;

                }

                ret = atoi(param->str);

            }

            pBat_EsFree_Cached(param);
            bAbortCommand = PBAT_ABORT_EXECUTION_LEVEL;
            return ret;

		} else {

			ret = atoi(ntoken);

		}
	}

    /* Exit from the interpretor */
    pBat_Exit();

    /* End thread or  the program */
	if (bIgnoreExit)
        pBat_EndThread((void*)ret);
    else {
        exit(ret);
    }

    /* this statement is never reached \o/ */
	return 0;
}
