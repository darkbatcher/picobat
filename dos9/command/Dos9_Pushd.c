/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *   Copyright (C) 2016      Astie Teddy
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
#include <string.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Pushd.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

/*
    PUSHD [path1] [path2] [pathN] ...

    For each path, change current directory and pushd to stack
    the previous current directory.

    POPD

    Pull a directory on the stack to change current directory.

*/

/* TODO: Makes this more Windows compatible
   On cmd.exe, pushd can mount UNC paths.
*/

LPSTACK lpStack;

int Dos9_CmdPushd (char *lpLine)
{
    int count = 1;
    ESTR *lpEstr=Dos9_EsInit();

    lpLine += 5;

    if (!(lpLine = Dos9_GetNextParameterEs(lpLine, lpEstr))) {

        ESTR *lpEsDir;

        if (!Dos9_GetStack(lpStack, &lpEsDir))
            puts(Dos9_EsToChar(lpEsDir));

        goto free;
    }

    if (!strncmp(Dos9_EsToChar(lpEstr), "/?", 2)) {
        Dos9_ShowInternalHelp(DOS9_HELP_PUSHD);
        goto free;
    }

    do {

        ESTR *lpEscd=Dos9_EsInit();

        Dos9_EsCat(lpEscd, Dos9_GetCurrentDir());

        if (!Dos9_SetCurrentDir(Dos9_EsToChar(lpEstr)))
            lpStack = Dos9_PushStack(lpStack, lpEscd);
        else {
            /* not a directory */
            Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, Dos9_EsToChar(lpEstr), FALSE);
            Dos9_EsFree(lpEscd);
            goto error;
        }

        count++;

    } while(lpLine = Dos9_GetNextParameterEs(lpLine, lpEstr));

    free:
        Dos9_EsFree(lpEstr);
        return 0;

    error:
        Dos9_EsFree(lpEstr);

        /* return the opposite of the count
           successful directories as error code */
        return count;
}

int Dos9_CmdPopd (char *lpLine)
{
    ESTR *lpEsDir;
    char *lpDir;

    lpLine += 4;

    lpLine = Dos9_SkipBlanks(lpLine);

    if (!strncmp(lpLine, "/?", 2)) {
        Dos9_ShowInternalHelp(DOS9_HELP_POPD);
        return 0;
    }

    if (Dos9_GetStack(lpStack, &lpEsDir))
        return -1;

    lpStack = Dos9_PopStack(lpStack, NULL);

    if (Dos9_SetCurrentDir(Dos9_EsToChar(lpEsDir))) {
        Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, Dos9_EsToChar(lpEsDir), FALSE);
        return -1;
    }

    Dos9_EsFree(lpEsDir);
    return 0;
}
