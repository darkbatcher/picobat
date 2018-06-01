/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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
#include "../core/Dos9_DirStack.h"

#include "Dos9_Pushd.h"
#include "Dos9_Cd.h"

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

int Dos9_CmdPushd (char *line)
{
  int count = 1;
  ESTR *estr = Dos9_EsInit();

  line += 5;

  if ((line = Dos9_GetNextParameterEs(line, estr)) == NULL) {
    /* No argument specified */
    size_t count = Dos9_DirStackCount();
    char **paths = Dos9_GetDirStack();

    while (count--)
      fprintf(fOutput, "%s" DOS9_NL, paths[count]);

    Dos9_EsFree(estr);
    return 0;
  }

  if (!strncmp(Dos9_EsToChar(estr), "/?", 2)) {
    /* Show help */
    Dos9_ShowInternalHelp(DOS9_HELP_PUSHD);
    Dos9_EsFree(estr);
    return 0;
  }

  do {
    ESTR *current_dir = Dos9_EsInit();

    Dos9_EsCat(current_dir, lpCurrentDir);

    if (Dos9_SetCurrentDir(Dos9_EsToChar(estr)) == 0) {
      Dos9_PushDir(Dos9_EsToChar(current_dir));
    } else {
      /* not a directory */
      Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, Dos9_EsToChar(estr), FALSE);
      Dos9_EsFree(current_dir);
      Dos9_EsFree(estr);
      return DOS9_DIRECTORY_ERROR;
    }

    count++;
  } while((line = Dos9_GetNextParameterEs(line, estr)));

  Dos9_EsFree(estr);
  return 0;
}

int Dos9_CmdPopd (char *line)
{
  line += 4;

  line = Dos9_SkipBlanks(line);

  if (strncmp(line, "/?", 2) == 0) {
    Dos9_ShowInternalHelp(DOS9_HELP_POPD);
    return 0;
  }

  char *path = Dos9_PopDir();

  if (path == NULL)
    return -1;

  if (Dos9_SetCurrentDir(path)) {
    Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, path, FALSE);
    free(path);
    return DOS9_DIRECTORY_ERROR;
  }

  free(path);
  return 0;
}
