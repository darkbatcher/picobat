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

#include "Dos9_Help.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

extern COMMANDINFO lpCmdInfo[];
extern const int iCmdInfoNb;

/* Ok, this is a wrapper for the help command. In fact, the usual 'Help'
   command is handled through an alias to hlp.bat, thus, this function is never
   effectively called by the executable. Some may argue that it is not safe to
   do so, but, it's actually what is done by cmd.exe */

int Dos9_CmdHelp(char* lpLine)
{
    ESTR *param = Dos9_EsInit();
    int n = 0, status = 0;

    lpLine += 4;

    while (lpLine = Dos9_GetNextParameterEs(lpLine, param))
        n++;

    if (n == 0) {

        /* loop through commands */
        while (n < iCmdInfoNb) {


            /* skip top level block */
            if (*(lpCmdInfo[n].ptrCommandName) != '(')
                fprintf(fError, "%s" DOS9_NL, lpCmdInfo[n].ptrCommandName);

            n++;
        }

        goto error;

    } else if (n > 1) {

        Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, param->str, 0);
        status = DOS9_UNEXPECTED_ELEMENT;
        goto error;

    }

    Dos9_EsCat(param, " /?");

    Dos9_RunCommand(param, NULL);

error:
    Dos9_EsFree(param);
	return status;
}
