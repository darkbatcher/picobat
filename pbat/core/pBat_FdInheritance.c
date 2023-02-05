/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2017 Romain GARBI
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

/* This source contains a bit of code derived from libcu8. The objective
   of the function is to set the fd inheritance reliably */

#include <stdio.h>

#ifdef WIN32
#include <stdlib.h>
#include <windows.h>
#include <io.h>

void pBat_SetFdInheritance(int fd, int mode)
{
    HANDLE handle = (HANDLE)_get_osfhandle(fd);

    SetHandleInformation(handle, HANDLE_FLAG_INHERIT, mode);
}

void pBat_SetAllFdInheritance(int mode)
{
    int i;

    for (i =0;i < 1024; i ++)
        pBat_SetFdInheritance(i, mode);

}

#else /* WIN32 */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
void pBat_SetFdInheritance(int fd, int mode)
{
    fcntl(fd, F_SETFD, mode ?
          (fcntl(fd, F_GETFD) & ~FD_CLOEXEC) :
                (fcntl(fd, F_GETFD) | FD_CLOEXEC));
}

#endif /* WIN32 */

void pBat_SetStdInheritance(FILE *file, int mode)
{
  pBat_SetFdInheritance(fileno(file), mode);
}
