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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Setlocal.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

#include "../../config.h"

/* SETLOCAL [/Q] [ENABLEDELAYEDEXPANSION|DISABLEDELAYEDEXPANSION]
        [CMDLYCORRECT | CMDLYINCORRECT]

    Toogle interpreter options. while saving environment variable
    to be restored during the next call of ENDLOCAL

        - /Q do not save variables for the next call to endlocal
*/

int pBat_CmdSetLocal(char* lpLine)
{
	char lpName[FILENAME_MAX];
	char* lpNext=lpLine+8;
	int push = 1,
        status = PBAT_NO_ERROR;

	while ((lpNext=pBat_GetNextParameter(lpNext, lpName, FILENAME_MAX))) {

		if (!strcmp(lpName, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_SETLOCAL);
			return 0;

		} else if (!stricmp(lpName, "/Q")) {

		    push = 0;

        } else if (!stricmp(lpName, "ENABLEDELAYEDEXPANSION")) {

			bDelayedExpansion=TRUE;

		} else if (!stricmp(lpName, "DISABLEDELAYEDEXPANSION")) {

			bDelayedExpansion=FALSE;

		} else if (!stricmp(lpName, "CMDLYCORRECT")) {

#if !defined(PBAT_STATIC_CMDLYCORRECT)
			bCmdlyCorrect=TRUE;
#else
			pBat_ShowErrorMessage(PBAT_UNABLE_SET_OPTION,
						"CMDLYCORRECT",
						FALSE
						);

            return PBAT_UNABLE_SET_OPTION;
#endif
		} else if (!stricmp(lpName, "CMDLYINCORRECT")) {
#if !defined(PBAT_STATIC_CMDLYCORRECT)
			bCmdlyCorrect=FALSE;
#else
			pBat_ShowErrorMessage(PBAT_UNABLE_SET_OPTION,
						"CMDLYINCORRECT",
						FALSE);

            return PBAT_UNABLE_SET_OPTION;
#endif
		}  else if (!stricmp(lpName, "ENABLEEXTENSIONS")
             || !stricmp(lpName, "DISABLEEXTENSION")) {

			/* provided for backward compatibility. The ENABLEEXTENSIONS
			   option was used to block some NT features to make scripts portables
			   to MS-DOS based prompt. This is not interesting anymore (at most
			   interest it too few people), so it is just ignored, since many NT
			   designed script use ENABLEEXTENSIONS to enable cmd.exe features
			 */

		} else {

			pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
                                    lpName, FALSE);

			return PBAT_UNEXPECTED_ELEMENT;

		}

	}

	if (push)
        pBat_PushEnvStack();

	return iErrorLevel;
}

/* ENDLOCAL

    Restore values saved by SETLOCAL

 */
int pBat_CmdEndLocal(char* lpLine)
{
    lpLine += 8;

    lpLine = pBat_SkipBlanks(lpLine);

    if (!strnicmp(lpLine, "/?", 2)) {

        pBat_ShowInternalHelp(PBAT_HELP_ENDLOCAL);
        return 0;

    }

    pBat_PopEnvStack();

    return iErrorLevel; /* leave errorlevel unaffected */
}
