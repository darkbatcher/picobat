/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Color.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"
#include "../errors/Dos9_Errors.h"

int Dos9_CmdPecho(char* lpLine)
{

	ESTR* lpEsParameter;

	lpLine += 5;

	if (*lpLine!=' '
		&& *lpLine!='\t'
	    && !ispunct(*lpLine)
	    && *lpLine!='\0') {

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpLine-4, FALSE);
		return DOS9_COMMAND_ERROR;

	}

	lpEsParameter=Dos9_EsInit();

    Dos9_GetEndOfLine(lpLine+1, lpEsParameter);

    if (!ispunct(*lpLine) &&
        !strcmp(lpEsParameter->str, "/?")) {

        Dos9_ShowInternalHelp(DOS9_HELP_PECHO);

    } else {

        Dos9_OutputPromptString(lpEsParameter->str);
        fputs(DOS9_NL, fOutput);

    }

	Dos9_EsFree(lpEsParameter);

	return 0;
}

int Dos9_CmdPrompt(char* lpLine)
{
    ESTR* prompt = Dos9_EsInit();

    Dos9_GetEndOfLine(Dos9_SkipBlanks(lpLine + 6), prompt);

    if (!strcmp(prompt->str, "/?"))
        Dos9_ShowInternalHelp(DOS9_HELP_PROMPT);
    else if (strlen(prompt->str) == 0)
        Dos9_SetEnv(lpeEnv, "PROMPT", "$P$G");
    else
        Dos9_SetEnv(lpeEnv, "PROMPT", prompt->str);

    Dos9_EsFree(prompt);

    return 0;
}

