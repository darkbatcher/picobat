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

#include "pBat_Goto.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

int pBat_CmdGoto(char* lpLine)
{
    char lpLabelName[FILENAME_MAX] = "";
    char lpFileName[FILENAME_MAX] = "";
    char *next, *pch;
    ESTR* arg = pBat_EsInit();
    int quiet = 0;

    lpLine+=4;

    while ((next = pBat_GetNextParameterEs(lpLine, arg))) {

        if (!strcmp(arg->str, "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_GOTO);
            goto err;

        } else if (!bCmdlyCorrect && !stricmp(arg->str, "/Q")) {

            quiet = 1;

        } else if (!bCmdlyCorrect && !strnicmp(arg->str, "/F", 2)) {

            pch = arg->str + 2;

            if (*pch == ':')
                pch ++;

            if (pBat_GetFileFullPath(lpFileName, pch, FILENAME_MAX) == -1) {

                pBat_ShowErrorMessage(PBAT_FILE_ERROR, pch, 0);
                iErrorLevel = PBAT_FILE_ERROR;
                goto err;

            }

        } else {

            break;

        }

        lpLine = next;

    }

    lpLine = pBat_SkipBlanks(lpLine);
    pBat_StripEndDelims(lpLine);

    pBat_GetEndOfLine(lpLine, arg);

    switch (*lpLine) {

    case ':':
        snprintf(lpLabelName, sizeof(lpLabelName), "%s", arg->str);
        break;

    default:
        snprintf(lpLabelName, sizeof(lpLabelName), ":%s", arg->str);

    }

    if (!stricmp(lpLabelName, ":EOF")) {

        bAbortCommand = PBAT_ABORT_EXECUTION_LEVEL;
        goto next;

    }

    if ((pBat_JumpToLabel(lpLabelName, *lpFileName ? lpFileName : NULL) == -1)) {

        if (! quiet)
            pBat_ShowErrorMessage(PBAT_LABEL_ERROR, lpLabelName, 0);

        iErrorLevel = PBAT_LABEL_ERROR;
        goto err;

    }

    bAbortCommand = PBAT_ABORT_COMMAND_BLOCK;

next:
err:
    pBat_EsFree(arg);
    return iErrorLevel; /* leave errorlevel unaffected */
}
