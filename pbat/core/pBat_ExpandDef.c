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
#include <libpBat.h>

#include "pBat_Core.h"

void pBat_ExpandDef(ESTR* lpRet, char* lpCh, char* lpExp)
{
    int i;
    char var[]="$1";
    ESTR *restrict ret=pBat_EsInit(),
        *restrict param=pBat_EsInit();

    pBat_EsCpy(ret, lpExp);

    lpCh = pBat_SkipBlanks(lpCh);
    pBat_EsReplace(ret, "$*", lpCh);

    for (i = 1; i <= 10; i++) {

        var[1] = '0' + i;

        if (lpCh
            && (lpCh = pBat_GetNextParameterEs(lpCh, param)))
            pBat_EsReplace(ret, var, param->str);
        else
            pBat_EsReplace(ret, var, "");
    }

    pBat_EsCpyE(lpRet, ret);

    pBat_EsFree(ret);
    pBat_EsFree(param);

}
