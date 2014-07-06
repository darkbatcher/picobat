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

#include "Dos9_Setlocal.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "../../config.h"

int Dos9_CmdSetLocal(char* lpLine)
{
	char lpName[FILENAME_MAX];
	char* lpNext=lpLine+8;

	while ((lpNext=Dos9_GetNextParameter(lpNext, lpName, FILENAME_MAX))) {

		if (!strcmp(lpName, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_SETLOCAL);
			return 0;

		} else if (!stricmp(lpName, "ENABLEDELAYEDEXPANSION")) {

			bDelayedExpansion=TRUE;

		} else if (!stricmp(lpName, "ENABLEFLOATS")) {

			bUseFloats=TRUE;

		} else if (!stricmp(lpName, "CMDLYCORRECT")) {

#if !defined(DOS9_STATIC_CMDLYCORRECT)
			bCmdlyCorrect=TRUE;
#else
			Dos9_ShowErrorMessage(DOS9_UNABLE_SET_OPTION,
						"CMDLYCORRECT",
						FALSE
						);			
#endif 
		} else if (!stricmp(lpName, "CMDLYINCORRECT")) {
#if !defined(DOS9_STATIC_CMDLYCORRECT)
			bCmdlyCorrect=FALSE;
#else
			Dos9_ShowErrorMessage(DOS9_UNABLE_SET_OPTION,
						"CMDLYINCORRECT",
						FALSE);
#endif
		} else if (!stricmp(lpName, "DISABLEFLOATS")) {

			bUseFloats=FALSE;

		} else if (!stricmp(lpName, "DISABLEDELAYEDEXPANSION")) {

			bDelayedExpansion=FALSE;

		} else if (!stricmp(lpName, "ENABLEEXTENSIONS") || !stricmp(lpName, "DISABLEEXTENSION")) {

			/* provided for backward compatibility. The ENABLEEXTENSIONS
			   option was used to block some NT features to make scripts portables
			   to MS-DOS based prompt. This is not interesting anymore (at most
			   interest it too few people), so it is just ignored, since many NT
			   designed script use ENABLEEXTENSIONS to enable cmd.exe features
			 */

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpName, FALSE);
			return 1;

		}

	}

	return 0;
}
