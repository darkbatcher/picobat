/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Title.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "../../config.h"

/* TIMEOUT [/T] [/NOBREAK] n

   wait for seconds.

   - /T : unused (At least we guess)

   - /NOBREAK : do not end waiting uppon keystroke
*/




#define SLEEP_GRANULARITY 0.100

int Dos9_CmdTimeout(char* lpLine)
{
    int status = DOS9_NO_ERROR,
        nobreak = 0;
    double seconds = 0.0;
    char* p;

    ESTR* param = Dos9_EsInit();

    lpLine += 7;

    while (lpLine = Dos9_GetNextParameterEs(lpLine, param)) {

        if (!stricmp(param->str, "/?")) {

            Dos9_ShowInternalHelp(DOS9_HELP_TIMEOUT);
            goto end;

        } else if (!stricmp(param->str, "/T")) {

            /* Apparently, does nothing at all from the doc I red */

        } else if (!stricmp(param->str, "/NOBREAK")) {

            nobreak = 1;

        } else {

            if (seconds) {

                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, param->str, 0);
                status = DOS9_UNEXPECTED_ELEMENT;
                goto end;

            }

            seconds = strtod(param->str, &p);

            if (p != NULL && *p) {

                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, param->str, 0);
                status = DOS9_UNEXPECTED_ELEMENT;
                goto end;

            }

        }

    }

    if (seconds < 0) {

        fprintf(fError, nobreak ? lpMsgTimeoutBreak : lpMsgTimeoutKeyPress);

        while(nobreak || !Dos9_Kbhit(fInput))
            Dos9_Sleep((unsigned int)(SLEEP_GRANULARITY * 1000));

    } else {

        fprintf(fError, nobreak ? lpMsgTimeoutNoBreak : lpMsgTimeout,
                    seconds);

        while (seconds > 0) {

            if (seconds > SLEEP_GRANULARITY) {

                Dos9_Sleep((unsigned int)(SLEEP_GRANULARITY * 1000));
                seconds -= SLEEP_GRANULARITY;

#if !defined(LIBDOS9_NO_CONSOLE)
                Dos9_ClearConsoleLine(fOutput);
                fprintf(fError, nobreak ? lpMsgTimeoutNoBreak : lpMsgTimeout,
                        seconds);
#endif

            } else {

                Dos9_Sleep((unsigned int)(seconds * 1000));
                seconds = -1;
            }

            if (!nobreak && Dos9_Kbhit(fInput))
                break;

        }
    }

    fputs(DOS9_NL, fError);

end:
    Dos9_EsFree(param);
	return status;
}
