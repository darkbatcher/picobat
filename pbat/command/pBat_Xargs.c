/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2017 Romain GARBI
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

#if defined(WIN32)
#include <io.h>
#endif

#include <libpBat.h>

#include "../core/pBat_Core.h"
#include "../../config.h"

#include "pBat_Xargs.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

/* XARGS [cmd ...]

    Run the command CMD taking arguments on the command line

 */

int pBat_CmdXargs(char* line)
{
    ESTR *param = pBat_EsInit_Cached(),
         *cmdline = pBat_EsInit_Cached();

    line = pBat_SkipBlanks(line + 5);

    int status = 0;

    if (*line == '\0') {

        pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "XARGS", 0);
        status = PBAT_EXPECTED_MORE;
        goto end;

    }

    if (!strcmp(line, "/?")) {

        pBat_ShowInternalHelp(PBAT_HELP_XARGS);
        goto end;

    }

    pBat_GetEndOfLine(line, cmdline); /* get the cmd */

    /* get input from stdin */
    while (!pBat_EsGet(param, fInput)) {

        pBat_RmTrailingNl(param->str);

        if (strpbrk(param->str, " ;,\t")
            && *(pBat_SkipAllBlanks(param->str)) != '"') {

            pBat_EsCat(cmdline, " \"");
            pBat_EsCatE(cmdline, param);
            pBat_EsCat(cmdline, "\"");

        } else {
            pBat_EsCat(cmdline, " ");
            pBat_EsCatE(cmdline, param);
        }

    }

    /* Reset files states if input refers to the terminal in which
       pbat runs */
    if (isatty(fileno(fInput)))
        clearerr(fInput);

    /* Run *a command* and not a line ! A line is somehow different
       since it may contain redirections or pipes. */
    status = pBat_RunCommand(cmdline, NULL);

end:
    pBat_EsFree_Cached(cmdline);
    pBat_EsFree_Cached(param);

    return status;
}
