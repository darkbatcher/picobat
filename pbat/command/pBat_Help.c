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

#include "pBat_Help.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

extern COMMANDINFO lpCmdInfo[];
extern const int iCmdInfoNb;

/* Ok, this is a wrapper for the help command. In fact, the usual 'Help'
   command is handled through an alias to hlp.bat, thus, this function is never
   effectively called by the executable. Some may argue that it is not safe to
   do so, but, it's actually what is done by cmd.exe */

int pBat_CmdHelp(char* lpLine)
{
    ESTR *param = pBat_EsInit_Cached();
    int n = 0, status = 0;

    lpLine += 4;

    while (lpLine = pBat_GetNextParameterEs(lpLine, param))
        n++;

    if (n == 0) {

        /* loop through commands */
        while (n < iCmdInfoNb) {


            /* skip top level block */
            if (*(lpCmdInfo[n].ptrCommandName) != '(')
                fprintf(fError, "%s" PBAT_NL, lpCmdInfo[n].ptrCommandName);

            n++;
        }

        goto error;

    } else if (n > 1) {

        pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
        status = PBAT_UNEXPECTED_ELEMENT;
        goto error;

    }

    pBat_EsCat(param, " /?");

    pBat_RunCommand(param, NULL);

error:
    pBat_EsFree_Cached(param);
	return status;
}
