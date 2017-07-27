/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"
#include "../../config.h"

#include "Dos9_Xargs.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* XARGS [cmd ...]

    Run the command CMD taking arguments on the command line

 */

int Dos9_CmdXargs(char* line)
{
    ESTR *param = Dos9_EsInit(),
         *cmdline = Dos9_EsInit();

    line = Dos9_SkipBlanks(line + 5);

    int status = 0;

    if (*line == '\0') {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "XARGS", 0);
        status = -1;
        goto end;

    }

    if (!strcmp(line, "/?")) {

        Dos9_ShowInternalHelp(DOS9_HELP_XARGS);
        goto end;

    }

    Dos9_GetEndOfLine(line, cmdline); /* get the cmd */

    /* get input from stdin */
    while (!Dos9_EsGet(param, stdin)) {

        Dos9_RmTrailingNl(param->str);
        Dos9_EsCat(cmdline, " ");
        Dos9_EsCatE(cmdline, param);

    }

    /* Reset files states if stdin refers to the terminal in which
       dos9 runs */
    if (isatty(DOS9_STDIN))
        clearerr(stdin);

    /* Run *a command* and not a line ! A line is somehow different
       since it may contain redirections or pipes. */
    status = Dos9_RunCommand(cmdline);

end:
    Dos9_EsFree(cmdline);
    Dos9_EsFree(param);
    return status;

}
