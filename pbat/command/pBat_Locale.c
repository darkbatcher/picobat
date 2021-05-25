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

#include "pBat_Locale.h"

#include "../../config.h"
#include <gettext.h>

void pBat_LoadLocaleMessages(char* loc)
{
    if (loc == NULL)
        loc = lpCurrentDir;

    bindtextdomain("pbat", loc);

    textdomain("pbat");

    pBat_LoadErrors();
    pBat_LoadStrings();
    pBat_LoadInternalHelp();
}

/* LOCALE path

   Loads a .po file containing translation for pBat

 */
int pBat_CmdLocale(char* line)
{
    ESTR* param = pBat_EsInit();
    int done = 0, status = 0;

    while (line = pBat_GetNextParameterEs(line, param)) {

        if (done) {

            pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
                                    param->str, 0);
            status = PBAT_UNEXPECTED_ELEMENT;
            goto end;

        } else if (!strcmp(param->str, "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_LOCALE);
            done = 1;

        } else {

            pBat_LoadLocaleMessages(pBat_EsToFullPath(param));
            done = 1;

        }

    }

end:
    pBat_EsFree(param);
    return status;
}
