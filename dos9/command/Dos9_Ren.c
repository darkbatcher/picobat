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

#include "Dos9_Ren.h"

#include "Dos9_Ask.h"

int Dos9_CmdRen(char* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();
	char lpFileName[FILENAME_MAX]= {0}, lpFileDest[FILENAME_MAX]= {0};
	char* lpToken;

	if (!strnicmp(lpLine, "RENAME", 6)) {

        lpLine += 6;

    } else if (!strnicmp(lpLine, "REN", 3)) {

        lpLine += 3;

    } else {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "REN / RENAME", FALSE);
		Dos9_EsFree(lpEstr);

		return DOS9_EXPECTED_MORE;

	}

	if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		strncpy(lpFileName, Dos9_EsToFullPath(lpEstr), FILENAME_MAX);
		lpFileName[FILENAME_MAX-1]='\0';
		/* can't assume that what was buffered is NULL-terminated
		   see the C-89,99,11 standards for further informations */

		strcpy(lpFileDest, lpFileName);

		if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

			if (strchr(lpEstr->str, '/') || strchr(lpEstr->str, '\\')) {

                Dos9_ShowErrorMessage(DOS9_MOVE_NOT_RENAME,
                                        lpEstr->str,
                                        0);

                Dos9_EsFree(lpEstr);
                return DOS9_MOVE_NOT_RENAME;

			}


			/* removing old filename */
			lpLine=strrchr(lpFileDest, '\\');
			lpToken=strrchr(lpFileDest, '/');

			if (lpToken>lpLine) {
				lpLine=lpToken;
			}

			if (lpLine) {
				lpLine++;
				*lpLine='\0';
			}

			/* cat with new name */
			strncat(lpFileDest, Dos9_EsToChar(lpEstr), FILENAME_MAX-strlen(lpFileDest));
			lpFileDest[FILENAME_MAX-1]='\0';
			/* can't assume that what was buffered is NULL-terminated
			   see the C-89,99,11 standards for further informations */

			if (rename(lpFileName, lpFileDest)) {

				Dos9_ShowErrorMessage(DOS9_UNABLE_RENAME, lpFileName, FALSE);
				Dos9_EsFree(lpEstr);
				return DOS9_UNABLE_RENAME;

			}

			return 0;
		}
	}

	Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "REN / RENAME", FALSE);
	Dos9_EsFree(lpEstr);

	return DOS9_EXPECTED_MORE;
}
