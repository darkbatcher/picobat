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

#if defined(WIN32) && defined(PBAT_USE_LIBCU8)
#include <libcu8.h>
#endif

#include "pBat_Chcp.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"


#if defined(WIN32)

int pBat_CmdChcp(char* line)
{
    ESTR* param = pBat_EsInit();
    char *enc;
    int cp,
        status = PBAT_NO_ERROR;

    line += 4;

    if ((line = pBat_GetNextParameterEs(line, param)) == NULL) {

        pBat_ShowErrorMessage(PBAT_BAD_COMMAND_LINE, "CHCP", 0);

        status = PBAT_BAD_COMMAND_LINE;
        goto error;

    }

    if (pBat_GetNextParameterEs(line, param) != NULL) {

        pBat_ShowErrorMessage(PBAT_BAD_COMMAND_LINE, "CHCP", 0);

        status = PBAT_BAD_COMMAND_LINE;
        goto error;

    }

    if (!strcmp(param->str, "/?")) {

        pBat_ShowInternalHelp(PBAT_HELP_CHCP);
        goto error;

    }

    /* try to get a codepage number */
    cp = strtol(param->str, &enc, 0);

    if (enc && *enc != '\0') {

        /* param->str is not a valid number */
#if defined(PBAT_USE_LIBCU8)
        if (libcu8_set_fencoding(param->str) == -1
            || !SetConsoleCP(cp))
#else
        if (1)
#endif
        {
            pBat_ShowErrorMessage(PBAT_INVALID_CODEPAGE, param->str, 0);

            status = PBAT_INVALID_CODEPAGE;
            goto error;

        }

    } else {

        /* param->str is a valid number */

#if defined(PBAT_USE_LIBCU8)
        if ((enc = pBat_ConsoleCP2Encoding(cp)) == NULL
            || libcu8_set_fencoding(enc) == -1
            || !SetConsoleCP(cp))
#else
        if (!SetConsoleCP(cp))
#endif
        {
            pBat_ShowErrorMessage(PBAT_INVALID_CODEPAGE, param->str, 0);
            status = PBAT_INVALID_CODEPAGE;
            goto error;

        }
    }

error:
    pBat_EsFree(param);
    return 0;

}

#else

int pBat_CmdChcp(char* line)
{
    return PBAT_NO_ERROR;
}

#endif
