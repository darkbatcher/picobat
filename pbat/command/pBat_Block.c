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

#include "pBat_Block.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

int pBat_CmdBlock(char* lpLine)
{
	BLOCKINFO bkCode;
	char* lpToken;
	ESTR* lpNextBlock=pBat_EsInit();
	int status = PBAT_NO_ERROR;

	pBat_GetNextBlockEs(lpLine, lpNextBlock);

	lpToken=pBat_GetNextBlock(lpLine, &bkCode);

	if (!lpToken) {

		pBat_ShowErrorMessage(PBAT_INVALID_TOP_BLOCK, lpLine, FALSE);
        status = PBAT_INVALID_TOP_BLOCK;

		goto error;

	}

	if (*lpToken!=')') {

		pBat_ShowErrorMessage(PBAT_INVALID_TOP_BLOCK, lpLine, FALSE);
        status = PBAT_INVALID_TOP_BLOCK;

		goto error;

	}

	lpToken++;

	lpToken=pBat_SkipBlanks(lpToken);


	if (*lpToken) {

		pBat_ShowErrorMessage(PBAT_INVALID_TOP_BLOCK, lpLine, FALSE);
        status = PBAT_INVALID_TOP_BLOCK;

		goto error;

	}

	pBat_RunBlock(&bkCode);

error:
	pBat_EsFree(lpNextBlock);
	return status ? status : iErrorLevel;
}
