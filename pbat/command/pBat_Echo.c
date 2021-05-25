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

#include "pBat_Echo.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

int pBat_CmdEcho(char* lpLine)
{

	ESTR* lpEsParameter;
	char* tmp,
		 	buf[1];

	lpLine += 4;

	if (*lpLine!=' '
		&& *lpLine!='\t'
	    && !ispunct(*lpLine)
	    && *lpLine!='\0') {

		pBat_ShowErrorMessage(PBAT_COMMAND_ERROR, lpLine-4, FALSE);
		return PBAT_COMMAND_ERROR;

	}

	lpEsParameter=pBat_EsInit();

	if (pBat_LockMutex(&mEchoLock))
        pBat_ShowErrorMessage(PBAT_LOCK_MUTEX_ERROR,
                                        "pBat_Echo.c/pBat_CmdEcho()", -1);

	if (ispunct(*lpLine)) {

		pBat_GetEndOfLine(lpLine+1, lpEsParameter);
		fputs(pBat_EsToChar(lpEsParameter), fOutput);
        fputs(PBAT_NL, fOutput);


	} else if ((tmp = pBat_GetNextParameterEs(lpLine, lpEsParameter))) {

		tmp = pBat_GetNextParameter(tmp, buf, sizeof(buf));

		if (!stricmp(pBat_EsToChar(lpEsParameter), "OFF") && tmp == NULL) {

			bEchoOn=FALSE;

		} else if (!stricmp(pBat_EsToChar(lpEsParameter) , "ON") && tmp == NULL) {

			bEchoOn=TRUE;

		} else if (!strcmp(pBat_EsToChar(lpEsParameter), "/?") && tmp == NULL) {

			pBat_ShowInternalHelp(PBAT_HELP_ECHO);

		} else {

			pBat_GetEndOfLine(lpLine+1, lpEsParameter);
			fputs(lpEsParameter->str, fOutput);
            fputs(PBAT_NL, fOutput);

		}

	} else {

		if (bEchoOn) fputs(lpMsgEchoOn, fOutput);
		else fputs(lpMsgEchoOff, fOutput);

        fputs(PBAT_NL, fOutput);

	}

	if (pBat_ReleaseMutex(&mEchoLock))
        pBat_ShowErrorMessage(PBAT_RELEASE_MUTEX_ERROR,
                                        "pBat_Echo.c/pBat_CmdEcho()", -1);

	pBat_EsFree(lpEsParameter);

	return 0;
}
