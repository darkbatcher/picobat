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

#include "Dos9_Echo.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

int Dos9_CmdEcho(char* lpLine)
{

	ESTR* lpEsParameter;
	char* tmp,
		 	buf[1];

	lpLine += 4;

	if (*lpLine!=' '
		&& *lpLine!='\t'
	    && !ispunct(*lpLine)
	    && *lpLine!='\0') {

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpLine-4, FALSE);
		return DOS9_COMMAND_ERROR;

	}

	lpEsParameter=Dos9_EsInit();

	if (ispunct(*lpLine)) {

		Dos9_GetEndOfLine(lpLine+1, lpEsParameter);
		fputs(Dos9_EsToChar(lpEsParameter), fOutput);
        fputs(DOS9_NL, fOutput);


	} else if ((tmp = Dos9_GetNextParameterEs(lpLine, lpEsParameter))) {

		tmp = Dos9_GetNextParameter(tmp, buf, sizeof(buf));

		if (!stricmp(Dos9_EsToChar(lpEsParameter), "OFF") && tmp == NULL) {

			bEchoOn=FALSE;

		} else if (!stricmp(Dos9_EsToChar(lpEsParameter) , "ON") && tmp == NULL) {

			bEchoOn=TRUE;

		} else if (!strcmp(Dos9_EsToChar(lpEsParameter), "/?") && tmp == NULL) {

			Dos9_ShowInternalHelp(DOS9_HELP_ECHO);

		} else {

			Dos9_GetEndOfLine(lpLine+1, lpEsParameter);
			fputs(lpEsParameter->str, fOutput);
            fputs(DOS9_NL, fOutput);

		}

	} else {

		/* si rien n'est entré on affiche l'état de la commande echo */
		if (bEchoOn) fputs(lpMsgEchoOn, fOutput);
		else fputs(lpMsgEchoOff, fOutput);

        fputs(DOS9_NL, fOutput);

	}

	Dos9_EsFree(lpEsParameter);

	return 0;
}
