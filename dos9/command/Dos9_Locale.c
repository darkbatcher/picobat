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

#include "Dos9_Locale.h"

#include "../../config.h"
#include <gettext.h>

void Dos9_LoadLocaleMessages(char* loc)
{
    if (loc == NULL)
        loc = lpCurrentDir;

    bindtextdomain("dos9", loc);

    textdomain("dos9");

    Dos9_LoadErrors();
    Dos9_LoadStrings();
    Dos9_LoadInternalHelp();
}

/* LOCALE path

   Loads a .po file containing translation for Dos9

 */
int Dos9_CmdLocale(char* line)
{
    ESTR* param = Dos9_EsInit();
    int done = 0, status = 0;

    while (line = Dos9_GetNextParameterEs(line, param)) {

        if (done) {

            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
                                    param->str, 0);
            status = DOS9_UNEXPECTED_ELEMENT;
            goto end;

        } else if (!strcmp(param->str, "/?")) {

            /* Do some thing with help message */

        } else {

            Dos9_LoadLocaleMessages(Dos9_EsToFullPath(param));
            done = 1;

        }

    }

end:
    Dos9_EsFree(param);
    return status;
}
