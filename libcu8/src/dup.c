/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015, 2016 Romain GARBI

 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of the name of Romain Garbi nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY ROMAIN GARBI AND CONTRIBUTORS ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL ROMAIN GARBI AND CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <windows.h>
#include <assert.h>

#include <libcu8.h>

#include "internals.h"


/* lock file before duping */
__LIBCU8__IMP __cdecl int libcu8_dup(int fd)
{
    struct ioinfo* info;
    int ret;

    if (!IS_VALID(fd)) {

        errno = EBADF;
        return -1;

    }

    info = pioinfo(fd);

    EnterCriticalSection(&(info->lock));

    ret = libcu8_dup_nolock(fd);

    LeaveCriticalSection(&(info->lock));

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_dup_nolock(int fd)
{
    int newfd;
    void *handle, *duplicate;

    handle = osfhnd(fd);

    if (!(DuplicateHandle(GetCurrentProcess(), (HANDLE)handle,
                            GetCurrentProcess(), (HANDLE*)&duplicate, 0,
                            TRUE, DUPLICATE_SAME_ACCESS))) {

        /* FIXME : Do something better with errno */
        errno = EBADF;
        return -1;

    }

    /* allocate new fd to store duplicated handle. Do not tamper to
       much with msvcrt internals */
    if ((newfd = _open_osfhandle(duplicate, _O_APPEND)) == -1) {

        CloseHandle(duplicate);
        return -1;

    }

    libcu8_manage_std_files(newfd, duplicate);

    /* Do not duplicate NOINHERIT attribute */
    osfile(newfd) = osfile(fd) & ~NOINHERIT;

    /* Duplicate libcu8 internal state */
    libcu8_cp_buffering(newfd, fd);

    return  newfd;
}

__LIBCU8__IMP __cdecl int libcu8_dup2(int fd1, int fd2)
{
    struct ioinfo *info2,
                *info1;
    int ret;


    /* According to POSIX standards, both fd1 and fd2 must be valid file
       descriptor. This contradicts what msvcrt does (ie. not requiring
       fd2 to be valid), but I believe it is not that important.

       Anyway, it is way safer because it avoids tampering to much within
       msvcrt internals.
       */
    if (!IS_VALID(fd1) || !IS_VALID(fd2)) {
        errno = EBADF;
        return -1;

    }


    if (fd1 == fd2) {

        /* Both file descriptors refer to the same file, e.g. duplication
           does not make sense. Return success according to POSIX standard */
        return fd2;

    }

    info1 = pioinfo(fd1);
    info2 = pioinfo(fd2);

    /* Lock both files. Note that locking/releasing order is important to
       prevent deadlocks... Indeed, we must use a lifo system. */
    EnterCriticalSection(&(info1->lock));
    EnterCriticalSection(&(info2->lock));

    if (info2->osfile & FHND)
        CloseHandle(info2->osfhnd);

    ret = libcu8_dup2_nolock(fd1, fd2);

    LeaveCriticalSection(&(info2->lock));
    LeaveCriticalSection(&(info1->lock));

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_dup2_nolock(int fd1, int fd2)
{
    void *handle, *duplicate;
    handle = osfhnd(fd1);

    if (!(DuplicateHandle(GetCurrentProcess(), (HANDLE)handle,
                            GetCurrentProcess(), (HANDLE*)&duplicate, 0,
                            TRUE, DUPLICATE_SAME_ACCESS))) {

        /* FIXME : Do something better with errno */
        errno = EBADF;
        return -1;

    }

    libcu8_manage_std_files(fd2, duplicate);

    /* Set the file handle */
    osfhnd(fd2) = duplicate;

    /* Do not duplicate NOINHERIT attribute */
    osfile(fd2) = osfile(fd1) & ~NOINHERIT;

    /* Duplicate libcu8 internal state */
    libcu8_cp_buffering(fd2, fd1);

    return  fd2;
}

void __inline__ libcu8_manage_std_files(int file, void* handle)
{
    /* Try to keep crt infos up to date with actual console
       configuration. It does not really matter, of course, if
       no console is actually opened, just let windows return
       errors. */
    switch(file) {
        case 0:
            SetStdHandle(STD_INPUT_HANDLE, handle);
            break;
        case 1:
            SetStdHandle(STD_OUTPUT_HANDLE, handle);
            break;
        case 2:
            SetStdHandle(STD_ERROR_HANDLE, handle);
            break;
    }
}

