/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Color.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"
#include "../errors/pBat_Errors.h"

int pBat_CmdPecho(char* lpLine)
{

	ESTR* lpEsParameter;

	lpLine += 5;

	if (*lpLine!=' '
		&& *lpLine!='\t'
	    && !ispunct(*lpLine)
	    && *lpLine!='\0') {

		pBat_ShowErrorMessage(PBAT_COMMAND_ERROR, lpLine-4, FALSE);
		return PBAT_COMMAND_ERROR;

	}

	lpEsParameter=pBat_EsInit();

    pBat_GetEndOfLine(lpLine+1, lpEsParameter);

    if (!ispunct(*lpLine) &&
        !strcmp(lpEsParameter->str, "/?")) {

        pBat_ShowInternalHelp(PBAT_HELP_PECHO);

    } else {

        pBat_OutputPromptString(lpEsParameter->str);
        fputs(PBAT_NL, fOutput);

    }

	pBat_EsFree(lpEsParameter);

	return 0;
}

int pBat_CmdPrompt(char* lpLine)
{
    ESTR* prompt = pBat_EsInit();

    pBat_GetEndOfLine(pBat_SkipBlanks(lpLine + 6), prompt);

    if (!strcmp(prompt->str, "/?"))
        pBat_ShowInternalHelp(PBAT_HELP_PROMPT);
    else if (strlen(prompt->str) == 0)
        pBat_SetEnv(lpeEnv, "PROMPT", "$P$G");
    else
        pBat_SetEnv(lpeEnv, "PROMPT", prompt->str);

    pBat_EsFree(prompt);

    return 0;
}

