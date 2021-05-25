/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Title.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

#include "../../config.h"

/* TIMEOUT [/T] [/NOBREAK] n

   wait for seconds.

   - /T : unused (At least we guess)

   - /NOBREAK : do not end waiting uppon keystroke
*/




#define SLEEP_GRANULARITY 0.100

int pBat_CmdTimeout(char* lpLine)
{
    int status = PBAT_NO_ERROR,
        nobreak = 0;
    double seconds = 0.0;
    char* p;

    ESTR* param = pBat_EsInit();

    lpLine += 7;

    while (lpLine = pBat_GetNextParameterEs(lpLine, param)) {

        if (!stricmp(param->str, "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_TIMEOUT);
            goto end;

        } else if (!stricmp(param->str, "/T")) {

            /* Apparently, does nothing at all from the doc I red */

        } else if (!stricmp(param->str, "/NOBREAK")) {

            nobreak = 1;

        } else {

            if (seconds) {

                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
                status = PBAT_UNEXPECTED_ELEMENT;
                goto end;

            }

            seconds = strtod(param->str, &p);

            if (p != NULL && *p) {

                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
                status = PBAT_UNEXPECTED_ELEMENT;
                goto end;

            }

        }

    }

    if (seconds < 0) {

        fprintf(fError, nobreak ? lpMsgTimeoutBreak : lpMsgTimeoutKeyPress);

        while(nobreak || !pBat_Kbhit(fInput))
            pBat_Sleep((unsigned int)(SLEEP_GRANULARITY * 1000));

    } else {

        fprintf(fError, nobreak ? lpMsgTimeoutNoBreak : lpMsgTimeout,
                    seconds);

        while (seconds > 0) {

            if (seconds > SLEEP_GRANULARITY) {

                pBat_Sleep((unsigned int)(SLEEP_GRANULARITY * 1000));
                seconds -= SLEEP_GRANULARITY;

#if !defined(LIBPBAT_NO_CONSOLE)
                pBat_ClearConsoleLine(fOutput);
                fprintf(fError, nobreak ? lpMsgTimeoutNoBreak : lpMsgTimeout,
                        seconds);
#endif

            } else {

                pBat_Sleep((unsigned int)(seconds * 1000));
                seconds = -1;
            }

            if (!nobreak && pBat_Kbhit(fInput))
                break;

        }
    }

    fputs(PBAT_NL, fError);

end:
    pBat_EsFree(param);
	return status;
}
