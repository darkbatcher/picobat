/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2018-2020 Astie Teddy
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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Dos9_Core.h"
#include "Dos9_DirStack.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#define paths_resize(new_size) realloc(dsDirStack.paths, sizeof(char *) * (new_size))

bool Dos9_DirStackInit(void)
{
  dsDirStack.paths = NULL;
  dsDirStack.count = 0;
  return false;
}

void Dos9_DirStackFree(void)
{
  for (size_t i = 0; i < dsDirStack.count; i++)
    free(dsDirStack.paths[i]);

  free(dsDirStack.paths);
}

bool Dos9_PushDir(const char *dir)
{
  char **new_paths = paths_resize(dsDirStack.count + 1);
  if (new_paths == NULL) {
    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION, "Dos9_PushDir()", 0);
    return true;
  }

  dsDirStack.paths = new_paths;

  size_t dir_len = strlen(dir) + 1;
  char *dir_copy = malloc(dir_len);
  if (dir_copy == NULL) {
    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION, "Dos9_PushDir", 0);
    return true;
  }

  strncpy(dir_copy, dir, dir_len);

  dsDirStack.paths[dsDirStack.count] = dir_copy;
  dsDirStack.count++;
  return false;
}

char *Dos9_PopDir(void)
{
  if (dsDirStack.count == 0)
    /* No more path. */
    return NULL;

  dsDirStack.count--;

  char *p = dsDirStack.paths[dsDirStack.count];
  char **new_paths = paths_resize(dsDirStack.count);

  if (dsDirStack.count == 0 || new_paths != NULL) {
    /* If new_paths is NULL, there is no "big" issue
       because old paths is bigger than new_paths.

       In case path_count is 0, new_paths may be NULL.
    */
    dsDirStack.paths = new_paths;
  }

  return p;
}

bool Dos9_DirStackCopy(struct dirstack_t *dest)
{
  /* NOTE: In case of failure, *dest must still be defined with a acceptable value. */

  DOS9_DBG("Dos9_DirStackCopy(): Copying DirStack to %p", dest);

  if (dest == NULL) {
    memset(dest, 0, sizeof(struct dirstack_t)); /* NOTE */
    return true;
  }

  dest->paths = malloc(dsDirStack.count * sizeof(char *));
  dest->count = dsDirStack.count;

  if (dest->paths == NULL) {
    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION, "Dos9_DirStackCopy", 0);
    memset(dest, 0, sizeof(struct dirstack_t)); /* NOTE */
    return true;
  }

  for (size_t i = 0; i < dsDirStack.count; i++) {
    size_t len = strlen(dsDirStack.paths[i]) + 1;

    dest->paths[i] = malloc(len);

    if (dest->paths[i] == NULL) {
      Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION, "Dos9_DirStackCopy", 0);

      /* We need to free all allocated paths, from 0 to i-1. */
      for (size_t j = 0; j < i; j++)
        free(dest->paths[i]);

      free(dest->paths);

      memset(dest, 0, sizeof(struct dirstack_t)); /* NOTE */
      return true;
    }

    memcpy(dest->paths[i], dsDirStack.paths[i], len);
  }

  return false;
}
