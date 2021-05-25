/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Pushd.h"
#include "pBat_Cd.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

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

int pBat_CmdPushd (char *line)
{
  int count = 1;
  ESTR *estr = pBat_EsInit();

  line += 5;

  if ((line = pBat_GetNextParameterEs(line, estr)) == NULL) {
    /* No argument specified, display add paths of the stack. */
    size_t i = dsDirStack.count;

    while (i--)
      fprintf(fOutput, "%s" PBAT_NL, dsDirStack.paths[i]);

    pBat_EsFree(estr);
    return 0;
  }

  if (!strncmp(pBat_EsToChar(estr), "/?", 2)) {
    /* Show help */
    pBat_ShowInternalHelp(PBAT_HELP_PUSHD);
    pBat_EsFree(estr);
    return 0;
  }

  ESTR *current_dir = pBat_EsInit();

  do {
    pBat_EsCpy(current_dir, lpCurrentDir);

    if (pBat_SetCurrentDir(pBat_EsToChar(estr)) == 0)
      pBat_PushDir(pBat_EsToChar(current_dir));
    else {
      /* not a directory */
      pBat_ShowErrorMessage(PBAT_DIRECTORY_ERROR, pBat_EsToChar(estr), FALSE);
      pBat_EsFree(current_dir);
      pBat_EsFree(estr);
      return PBAT_DIRECTORY_ERROR;
    }

    count++;
  } while((line = pBat_GetNextParameterEs(line, estr)));

  pBat_EsFree(current_dir);
  pBat_EsFree(estr);
  return 0;
}

int pBat_CmdPopd (char *line)
{
  line += 4;

  line = pBat_SkipBlanks(line);

  if (strncmp(line, "/?", 2) == 0) {
    pBat_ShowInternalHelp(PBAT_HELP_POPD);
    return 0;
  }

  char *path = pBat_PopDir();

  if (path == NULL)
    return -1;

  if (pBat_SetCurrentDir(path)) {
    pBat_ShowErrorMessage(PBAT_DIRECTORY_ERROR, path, FALSE);
    free(path);
    return PBAT_DIRECTORY_ERROR;
  }

  free(path);
  return 0;
}
