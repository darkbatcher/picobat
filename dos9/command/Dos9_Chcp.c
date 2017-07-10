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

#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
#include <libcu8.h>
#endif

#include "Dos9_Chcp.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"


#if defined(WIN32)

int Dos9_CmdChcp(char* line)
{
    ESTR* param = Dos9_EsInit();
    char *enc;
    int cp;

    line += 4;

    if ((line = Dos9_GetNextParameterEs(line, param)) == NULL) {

        Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, "CHCP", 0);
        return -1;

    }

    if (Dos9_GetNextParameterEs(line, param) != NULL) {

        Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, "CHCP", 0);
        return -1;

    }

    if (!strcmp(param->str, "/?")) {

        Dos9_ShowInternalHelp(DOS9_HELP_CHCP);
        return 0;

    }

    /* try to get a codepage number */
    cp = strtol(param->str, &enc, 0);

    if (enc && *enc != '\0') {

        /* param->str is not a valid number */
#if defined(DOS9_USE_LIBCU8)
        if (libcu8_set_fencoding(param->str) == -1)
#else
        if (1)
#endif
        {
            Dos9_ShowErrorMessage(DOS9_INVALID_CODEPAGE, param->str, 0);
            return -1;

        }

    } else {

        /* param->str is a valid number */

#if defined(DOS9_USE_LIBCU8)
        if ((enc = Dos9_ConsoleCP2Encoding(cp)) == NULL
            || libcu8_set_fencoding(enc) == -1)
#else
        if (!SetConsoleCP(cp))
#endif
        {
            Dos9_ShowErrorMessage(DOS9_INVALID_CODEPAGE, param->str, 0);
            return -1;

        }
    }

    return 0;

}

#else

int Dos9_CmdChcp(char* line)
{
    fprintf(fError, "The CHCP command is not supported on platforms other than Windows\n");
    return -1;
}

#endif
