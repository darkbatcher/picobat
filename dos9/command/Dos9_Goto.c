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

#include "Dos9_Goto.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

int Dos9_CmdGoto(char* lpLine)
{
    char lpLabelName[FILENAME_MAX] = "";
    char lpFileName[FILENAME_MAX] = "";
    char *next, *pch;
    ESTR* arg = Dos9_EsInit();
    int quiet = 0;

    lpLine+=4;

    while ((next = Dos9_GetNextParameterEs(lpLine, arg))) {

        if (!strcmp(arg->str, "/?")) {

            Dos9_ShowInternalHelp(DOS9_HELP_GOTO);
            goto err;

        } else if (!bCmdlyCorrect && !stricmp(arg->str, "/Q")) {

            quiet = 1;

        } else if (!bCmdlyCorrect && !strnicmp(arg->str, "/F", 2)) {

            pch = arg->str + 2;

            if (*pch == ':')
                pch ++;

            if (Dos9_GetFileFullPath(lpFileName, pch, FILENAME_MAX) == -1) {

                Dos9_ShowErrorMessage(DOS9_FILE_ERROR, pch, 0);
                goto err;

            }

        } else {

            break;

        }

        lpLine = next;

    }

    lpLine = Dos9_SkipBlanks(lpLine);
    Dos9_StripEndDelims(lpLine);

    Dos9_GetEndOfLine(lpLine, arg);

    switch (*lpLine) {

    case ':':
        snprintf(lpLabelName, sizeof(lpLabelName), "%s", arg->str);
        break;

    default:
        snprintf(lpLabelName, sizeof(lpLabelName), ":%s", arg->str);

    }

    if (!stricmp(lpLabelName, ":EOF")) {

        bAbortCommand = -1;
        goto next;

    }

    if ((Dos9_JumpToLabel(lpLabelName, *lpFileName ? lpFileName : NULL) == -1)
        && !quiet) {

        Dos9_ShowErrorMessage(DOS9_LABEL_ERROR, lpLabelName, 0);
        goto err;

    }

    bAbortCommand = 1;

next:
    Dos9_EsFree(arg);
    return 0;

err:
    Dos9_EsFree(arg);
    return -1;
}
