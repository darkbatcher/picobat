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

#include "pBat_Cls.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

int pBat_CmdCls(char* lpLine)
{
	char lpArg[4];

	if (pBat_GetNextParameter(lpLine+3, lpArg, 4)) {

		if (!strcmp(lpArg, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_CLS);
			return PBAT_NO_ERROR;

		} else {

			pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, lpArg, FALSE);
			return PBAT_UNEXPECTED_ELEMENT;

		}

	}

	pBat_ClearConsoleScreen(fOutput);

	return PBAT_NO_ERROR;
}
