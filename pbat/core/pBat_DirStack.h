/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#ifndef PBAT_DIRSTACK_H
#define PBAT_DIRSTACK_H

#include <stdbool.h>
#include <stddef.h>

struct dirstack_t {
  char **paths;
  size_t count;
};

bool pBat_DirStackInit(void);
void pBat_DirStackFree(void);

bool pBat_PushDir(const char *dir);
char *pBat_PopDir(void); /* You need to free() the output directory path. */

bool pBat_DirStackCopy(struct dirstack_t *dest);

#endif /* PBAT_DIRSTACK_H */
