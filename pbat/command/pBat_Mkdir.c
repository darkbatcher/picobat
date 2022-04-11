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

#include "pBat_Commands.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

#include "pBat_Mkdir.h"

#ifdef WIN32
#define IS_DIR_DELIM(c) (c=='/' || c=='\\')
#define MKDIR_MACRO(dir, mode) mkdir(dir)
#else
#define IS_DIR_DELIM(c) (c=='/')
#define MKDIR_MACRO(dir, mode) mkdir(dir, mode)
#endif // WIN32

int pBat_CmdMkdir(char* lpLine)
{
	ESTR* lpEstr=pBat_EsInit_Cached();
	int status = PBAT_NO_ERROR;

	if (!(lpLine=pBat_GetNextParameterEs(lpLine, lpEstr))) {

		pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "MD/MKDIR", FALSE);
		status = PBAT_EXPECTED_MORE;
		goto error;

	}

	while ((lpLine=pBat_GetNextParameterEs(lpLine, lpEstr))) {
		if (!strcmp(pBat_EsToChar(lpEstr), "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_MD);
			break;

		} else {

            if (status = pBat_CmdMakeDirs(pBat_EsToFullPath(lpEstr)))
                goto error;

		}
	}

error:
	pBat_EsFree_Cached(lpEstr);
	return status;

}

int pBat_CmdMakeDirs(char* str)
{
    char c,
         *dir = str;

    int status = PBAT_NO_ERROR;

    while (*str) {

        if (IS_DIR_DELIM(*str) && str != dir) {

            /* Descent through the tree to create *every* non-existing
                dir in the path */

            c = *str;
            *str = '\0';


            /* You know... it's not really that important to report
               errors yet, since if any of the folder fail to create,
               any subsequent will fail too and so eventually creating
               the last directory will fail.

               Anyway, its saves us the worry of checking whether it is
               legitimate to create the said dir, which might happen
               processing unc paths (\\?\) ...  */

            if (!pBat_DirExists(dir))
                MKDIR_MACRO(dir, 0777);

            *str = c;

        }

        str ++;

    }

    if (!pBat_DirExists(dir)
        && MKDIR_MACRO(dir, 0777)) {

        pBat_ShowErrorMessage(PBAT_MKDIR_ERROR | PBAT_PRINT_C_ERROR,
                              dir,
                              FALSE);
        status = PBAT_MKDIR_ERROR;

    }

    return status;
}
