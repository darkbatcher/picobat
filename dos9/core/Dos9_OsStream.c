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
#include <strings.h>

#include "Dos9_Core.h"

#ifndef WIN32

#define STREAM_CASE(str, k) \
            case k: \
                return str #k;

#define CHOOSE_STREAM(str, i) \
            switch(i) { \
                STREAM_CASE(str, 0)\
                STREAM_CASE(str, 1)\
                STREAM_CASE(str, 2)\
                STREAM_CASE(str, 3)\
                STREAM_CASE(str, 4)\
                STREAM_CASE(str, 5)\
                STREAM_CASE(str, 6)\
                STREAM_CASE(str, 7)\
                STREAM_CASE(str, 8)\
                STREAM_CASE(str, 9)\
                STREAM_CASE(str, 10)\
            }

const char* _Dos9_GetOsStreamName(const char* str)
{

    int i;
    char *pCh;

    if (!stricmp(str, "NUL"))
        return "/dev/null";

    if (!stricmp(str, "CON"))
        return "/dev/tty";

    if (!stricmp(str, "PRN"))
        return "/dev/lpt0";

    if (!strnicmp(str, "LPT", 3)) {

        i = strtol(str+3, &pCh, 10) - 1;

        if (!pCh || !*pCh)
            CHOOSE_STREAM("/dev/lpt", i);

    }

    if (!strnicmp(str, "COM", 3)) {

        i = strtol(str+3, &pCh, 10) - 1;

        if (!pCh || !*pCh)
            CHOOSE_STREAM("/dev/ttyS", i);

     }

    return str;
}

#endif
