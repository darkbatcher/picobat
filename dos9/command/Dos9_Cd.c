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

#include "Dos9_Cd.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* FIXME : Make it more compatible with cmd.exe
   In fact, cmd.exe supports different paths on differents drives
   through variables like %=x:% where x is the name of the drive.
   So that the /d switch is *really* usefull.
*/

int Dos9_CmdCd(char* lpLine)
{
	char* lpNext;
	ESTR* lpEsDir=Dos9_EsInit();

	if (!(lpLine=strchr(lpLine, ' '))) {
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, FALSE);
		goto error;
	}

	if ((lpNext=Dos9_GetNextParameterEs(lpLine, lpEsDir))) {

		if (!strcmp(Dos9_EsToChar(lpEsDir), "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_CD);
			goto error;

		} else if (!stricmp(Dos9_EsToChar(lpEsDir), "/d")) {

			lpLine=lpNext;

		}

		while (*lpLine==' ' || *lpLine=='\t') lpLine++;

		Dos9_GetEndOfLine(lpLine, lpEsDir);

		lpLine=Dos9_EsToChar(lpEsDir);

		lpNext=NULL;

		while (*lpLine) {

			switch(*lpLine) {
				case '\t':
				case ' ':

					if (!lpNext) lpNext=lpLine;
					break;

				default:
					lpNext=NULL;
			}

			lpLine++;

		}

		if (lpNext) *lpNext='\0';

		errno=0;

		lpLine=Dos9_EsToChar(lpEsDir);

		DOS9_DBG("Changing directory to : \"%s\"\n", lpLine);

		chdir(lpLine);

		if (errno ==  0) {

			/* update the current directory buffer */

			Dos9_UpdateCurrentDir();

		} else {

			/* do not perform errno checking
			   as long as the most important reason for
			   chdir to fail is obviously the non existence
			   or the specified directory

			   However, it appears that this is inconsistant
			   using windows as it does not returns on failure
			   every time a non-existing folder is passed to the
			   function, tried with '.. ' on my system

			*/

			Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, lpLine, FALSE);
			goto error;

		}

	} else {

		puts(Dos9_GetCurrentDir());

	}

	Dos9_EsFree(lpEsDir);

	DOS9_DBG("Returning from \"cd\" on success\n");
	return 0;

error:

	Dos9_EsFree(lpEsDir);
	return -1;
}

