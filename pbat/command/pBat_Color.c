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
#include "../../config.h"

#include "../core/pBat_Core.h"

#include "pBat_Color.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"
#include "../errors/pBat_Errors.h"

#define TO_CHAR(a) (0x00FF & (a))
#define HWORD(a) ((0xF0 & (a)) >> 4)
#define LWORD(a) (0xF & (a))

#if defined(WIN32) && !defined(LIBPBAT_W10_ANSI)
#define BACKGROUND_COLOR(x) ((x) << 4)
#define FOREGROUND_COLOR(x) (x)
#else
/* On windows, colors are shuffled versus unix :

    win32           UNIX

    1 = blue        1 = red
    2 = green       2 = green
    4 = red         4 = blue

 */
#define BACKGROUND_COLOR(x) ((((x) >= 8) ? PBAT_BACKGROUND_INT : 0) \
                           | (((x) % 8) & 1 ? PBAT_BACKGROUND_BLUE : 0) \
                           | (((x) % 8) & 2 ? PBAT_BACKGROUND_GREEN : 0) \
                           | (((x) % 8) & 4 ? PBAT_BACKGROUND_RED : 0))
#define FOREGROUND_COLOR(x) ((((x) >= 8) ? PBAT_FOREGROUND_INT : 0) \
                           | (((x) % 8) & 1 ? PBAT_FOREGROUND_BLUE : 0) \
                           | (((x) % 8) & 2 ? PBAT_FOREGROUND_GREEN : 0) \
                           | (((x) % 8) & 4 ? PBAT_FOREGROUND_RED : 0))
#endif

int pBat_GetColorCode(char* lpArg)
{
    int back=PBAT_GET_BACKGROUND(colColor) << 4,
        fore; /* back and foreground */
    int i=strlen(lpArg);

    if (i > 2
        || i <= 0)
        return -1;

    if (i > 1) {

        switch (tolower(*lpArg)) {

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            back = BACKGROUND_COLOR(*lpArg - '0');
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            back = BACKGROUND_COLOR(10 + tolower(*lpArg) - 'a');
            break;

        case '.':
            back = PBAT_GET_BACKGROUND(colColor);
            break;

        default:
            return -1;

        }

        lpArg ++;
    }

    switch (tolower(*lpArg)) {

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        fore = FOREGROUND_COLOR(*lpArg - '0');
        break;

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        fore = FOREGROUND_COLOR(10 + tolower(*lpArg) - 'a');
        break;

    case '.':
        fore = PBAT_GET_FOREGROUND(colColor);
        break;

    default:
        return -1;

    }

    return fore | back;

}

int pBat_CmdColor(char* lpLine)
{
	char lpArg[4];
	int code;

	if (pBat_GetNextParameter(lpLine+5, lpArg, 4)) {

		if (!strcmp(lpArg, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_COLOR);

		} else {

			if ((code = pBat_GetColorCode(lpArg)) == -1
                || (HWORD(code) == LWORD(code))) {

                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, lpArg, 0);
                return PBAT_UNEXPECTED_ELEMENT;

            }

            colColor = code;
			pBat_SetConsoleColor(fOutput, colColor);

		}

		return PBAT_NO_ERROR;

	}

	colColor=PBAT_COLOR_DEFAULT;
	pBat_SetConsoleColor(fOutput, PBAT_COLOR_DEFAULT);

	return PBAT_NO_ERROR;
}
