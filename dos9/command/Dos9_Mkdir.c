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

#include "Dos9_Commands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "Dos9_Mkdir.h"

#ifdef WIN32
#define IS_DIR_DELIM(c) (c=='/' || c=='\\')
#define MKDIR_MACRO(dir, mode) mkdir(dir)
#else
#define IS_DIR_DELIM(c) (c=='/')
#define MKDIR_MACRO(dir, mode) mkdir(dir, mode)
#endif // WIN32

int Dos9_CmdMkdir(char* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();
	int status = DOS9_NO_ERROR;

	if (!(lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "MD/MKDIR", FALSE);
		status = DOS9_EXPECTED_MORE;
		goto error;

	}

	while ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {
		if (!strcmp(Dos9_EsToChar(lpEstr), "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_MD);
			break;

		} else {

            if (status = Dos9_CmdMakeDirs(Dos9_EsToFullPath(lpEstr)))
                goto error;

		}
	}

error:
	Dos9_EsFree(lpEstr);
	return status;

}

int Dos9_CmdMakeDirs(char* str)
{
    char c,
         *dir = str;

    int status = DOS9_NO_ERROR;

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

            if (!Dos9_DirExists(dir))
                MKDIR_MACRO(dir, 0777);

            *str = c;

        }

        str ++;

    }

    if (!Dos9_DirExists(dir)
        && MKDIR_MACRO(dir, 0777)) {

        Dos9_ShowErrorMessage(DOS9_MKDIR_ERROR | DOS9_PRINT_C_ERROR,
                              dir,
                              FALSE);
        status = DOS9_MKDIR_ERROR;

    }

    return status;
}
