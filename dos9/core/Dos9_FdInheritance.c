/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifdef WIN32
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

struct ioinfo {
    void* osfhnd;
    unsigned char osfile;
    unsigned char pipech;
    int lockinitflag;
    CRITICAL_SECTION lock;
};

extern _CRTIMP struct ioinfo* __pioinfo[];

/* define some macros to deal with __pioinfo buffer (which stores
   data file information */
#define IOINFO_TABLE_SIZE (1 << 5)
#define IOINFO_MAX_TABLE 64
#define MAKE_FD(table,index)  ( (table) << 5 + (index))

#define pioinfo(i)  (__pioinfo[((i) >> 5)] + \
                                    ((i) & (IOINFO_TABLE_SIZE - 1)))
#define osfile(i)   (pioinfo(i)->osfile)
#define osfhnd(i)   (pioinfo(i)->osfhnd)
#define NOINHERIT   0x10 /* not inheritable file */

void Dos9_SetFdInheritance(int fd, int mode)
{
    HANDLE handle = osfhnd(fd);

    SetHandleInformation(handle, HANDLE_FLAG_INHERIT, mode);

    osfile(fd) &= ~ NOINHERIT;
}

#else
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
void Dos9_SetFdInheritance(int fd, int mode)
{
    fcntl(fd, F_SETFD, mode ?
          (fcntl(fd, F_GETFD) & ~FD_CLOEXEC) :
                (fcntl(fd, F_GETFD) | FD_CLOEXEC));
}

#endif /* WIN32 */
