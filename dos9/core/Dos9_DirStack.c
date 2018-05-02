/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2018 Astie Teddy
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

#include "Dos9_DirStack.h"

static __thread char **paths;
static __thread size_t path_count;

#define paths_resize(new_size) realloc(paths, sizeof(char *) * (new_size))

bool Dos9_DirStackInit(void)
{
  paths = malloc(0);
  if (paths == NULL)
    return true;

  path_count = 0;
  return false;
}

void Dos9_DirStackFree(void)
{
  for (size_t i = 0; i < path_count; i++)
    free(paths[i]);

  free(paths);
}

bool Dos9_PushDir(const char *s)
{
  char **new_paths = paths_resize(path_count + 1);
  if (new_paths == NULL)
    return true;

  paths = new_paths;

  size_t s_len = strlen(s) + 1;
  char *s_copy = malloc(s_len);
  if (s_copy == NULL)
    return true;

  strncpy(s_copy, s, s_len);

  paths[path_count] = s_copy;
  path_count++;
  return false;
}

char *Dos9_PopDir(void)
{
  if (path_count == 0)
    /* No more path. */
    return NULL;

  char *p = paths[path_count - 1];
  char **new_paths = paths_resize(path_count - 1);
  if (new_paths != NULL) {
    /* If new_paths is NULL, there is no "big" issue
       because old paths is bigger than new_paths.
    */
    paths = new_paths;
  }

  path_count--;
  return p;
}

size_t Dos9_DirStackCount(void)
{
  return path_count;
}
