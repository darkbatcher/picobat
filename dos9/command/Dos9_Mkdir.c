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

#include "Dos9_Mkdir.h"

int Dos9_CmdMkdir(char* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();

	if (!(lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "MD/MKDIR", FALSE);
		goto error;

	}

	if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {
		if (!strcmp(Dos9_EsToChar(lpEstr), "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_MD);

		} else {

			#if defined(_POSIX_C_SOURCE)

			/* FIXME : Do not use systematically the 0777 rights
			   because it main simply not work if the user is not
			   rooted or something like this. */

			/* use POSIX-Conforming mkdir function */

			if (mkdir(Dos9_EsToChar(lpEstr), 0777)) {

			#elif defined(WIN32)

			/* use WINDOWS mkdir function */

			if (mkdir(Dos9_EsToChar(lpEstr))) {

			#endif // defined _POSIX_C_SOURCE

				Dos9_ShowErrorMessage(DOS9_MKDIR_ERROR, Dos9_EsToChar(lpEstr), FALSE);
				goto error;

			}


		}
	}

	Dos9_EsFree(lpEstr);
	return 0;

error:
	Dos9_EsFree(lpEstr);
	return -1;

}
