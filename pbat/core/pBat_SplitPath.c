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
#include <string.h>
#include "pBat_Core.h"

//#define PBAT_DBG_MODE
#include "pBat_Debug.h"

void pBat_SplitPath(char* lpPath,
                    char* lpDisk, char* lpDir, char* lpName, char* lpExt)
{

	char* lpNextToken, *lpToken=lpPath;
	char cSaveChar;

	if (*lpToken
        && (!strncmp(lpToken + 1, ":\\", 2)
            || !strncmp(lpToken + 1, ":/", 2))) {

        PBAT_DBG("Found disk_name=\"%c\"\n", *lpToken);

        if (lpDisk)
            snprintf(lpDisk, FILENAME_MAX, "%c:", *lpToken);

        lpToken+=3;

	} else {

		if (lpDisk)
			*lpDisk='\0';

	}

	if ((lpNextToken = pBat_SearchLastToken(lpToken, "\\/"))) {

		lpNextToken++;
		cSaveChar=*lpNextToken;
		*lpNextToken='\0';

		PBAT_DBG("found path=\"%s\"\n", lpToken);

		if (lpDir)
			strncpy(lpDir, lpToken, _MAX_DIR);

		*lpNextToken=cSaveChar;
		lpToken=lpNextToken;

	} else {

		if (lpDir)
			*lpDir='\0';

	}

	if ((lpNextToken = pBat_SearchLastChar(lpToken, '.'))) {

		cSaveChar=*lpNextToken;
		*lpNextToken='\0';

		PBAT_DBG("found name=\"%s\"\n", lpToken);


		if (lpName)
			strncpy(lpName, lpToken, _MAX_FNAME);

		*lpNextToken=cSaveChar;
		lpToken=lpNextToken+1;

		PBAT_DBG("found ext=\"%s\"\n", lpToken);

		if (lpExt)
			snprintf(lpExt, _MAX_EXT, ".%s", lpToken);

	} else {

		if (lpName)
			strncpy(lpName, lpToken, _MAX_FNAME);

		if (lpExt)
			*lpExt='\0';

	}


}
