/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *   Copyright (C) 2016      Astie Teddy
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

#include "Dos9_Cd.h"
#include "Dos9_Popd.h"
#include "Dos9_Pushd.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

char* Dos9_Pushd_GetDirectory();

 /* Change directory back to the path/folder most recently stored by the PUSHD command.

     POPD
 */

/* TODO : For Windows only :
     - make it able to dismount previusly mounted UNC path using pushd
*/
int Dos9_CmdPopd (char *lpLine)
{
    lpLine += 4;
    lpLine = Dos9_SkipBlanks(lpLine);

    if (!strcmp(lpLine, "/?")) {
        Dos9_ShowInternalHelp(DOS9_HELP_POPD);
        return 0;
    }

    /* No directory in the stack. */
    if (Dos9_CmdPushd_GetDirectory() == "")
        return -1;

    char *dir = Dos9_CmdPushd_GetDirectory();

    if (Dos9_SetCurrentDir(dir)) {
        Dos9_CmdPushd_PopDirectory();
        return -1;
    } else {
        Dos9_CmdPushd_PopDirectory();
        return 0;
    }
}
