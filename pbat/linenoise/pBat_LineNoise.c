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
 */

#include <stdio.h>
#include <stdlib.h>

#include <libpBat.h>
#include "../core/pBat_Core.h"

#ifndef WIN32

#include "linenoise.h"
#include "utf8.h"

/* something similar to pBat_EsGet() (ie. return 1 at eof) */
int pBat_LineNoise(ESTR* estr, FILE* pFile)
{
    char* line;
    if (pBat_LockMutex(&mLineNoise))
        pBat_ShowErrorMessage(PBAT_LOCK_MUTEX_ERROR,
                              __FILE__ "/pBat_LineNoise()" , -1);


    linenoiseSetEncodingFunctions(
        linenoiseUtf8PrevCharLen,
        linenoiseUtf8NextCharLen,
        linenoiseUtf8ReadCode);
    linenoiseSetMultiLine(1);

    line = linenoise(NULL);
    if (line)
        linenoiseHistoryAdd(line);


    if (pBat_ReleaseMutex(&mLineNoise))
        pBat_ShowErrorMessage(PBAT_RELEASE_MUTEX_ERROR,
                              __FILE__ "/pBat_LineNoise()" , -1);

    if (line == NULL)
        return 1;


    pBat_EsCpy(estr, line);

    return 0;
}

#endif /* !WIN32 */
