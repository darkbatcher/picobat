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


	if (!(lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "MD/MKDIR", FALSE);
		goto error;

	}

	while ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {
		if (!strcmp(Dos9_EsToChar(lpEstr), "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_MD);
			break;

		} else {

            if (Dos9_CmdMakeDirs(Dos9_EsToChar(lpEstr)))
                goto error;

		}
	}

	Dos9_EsFree(lpEstr);
	return 0;

error:
	Dos9_EsFree(lpEstr);
	return -1;

}

int Dos9_CmdMakeDirs(char* str)
{
    char c,
         *dir = str;

    while (*str) {

        if (IS_DIR_DELIM(*str) && str != dir) {

            /* Descent throught the tree to create *every* non-existing
                dir in the path */

            c = *str;
            *str = '\0';

            if (!Dos9_DirExists(dir)
                && MKDIR_MACRO(dir, 0777)) {

                Dos9_ShowErrorMessage(DOS9_UNABLE_MKDIR | DOS9_PRINT_C_ERROR,
                                      dir,
                                      FALSE);
                return -1;

            }

            *str = c;

        }

        str ++;

    }

    if (!Dos9_DirExists(dir)
        && MKDIR_MACRO(dir, 0777)) {

        Dos9_ShowErrorMessage(DOS9_MKDIR_ERROR | DOS9_PRINT_C_ERROR,
                              dir,
                              FALSE);
        return -1;

    }
}

