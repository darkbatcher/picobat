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

#include "internals.h"
#include "libcu8.h"

__LIBCU8__IMP __cdecl FILE* libcu8_fopen(const char* __restrict__ name, const char* __restrict__ mode)
{
    wchar_t *wfile, *wmode;
    size_t conv;
    FILE* ret;

    if (!(wfile = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, name,
                                                strlen(name)+1, &conv))) {

        errno = ENOMEM;
        return NULL;

    }

    if (!(wmode = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, mode,
                                                strlen(mode)+1, &conv))) {

        errno = ENOMEM;
        free(wfile);
        return NULL;

    }

    ret = _wfopen(wfile, wmode);

    free(wfile);
    free(wmode);

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_open(char* name, int oflags, int pmode)
{
    wchar_t *wcs;
    int fd;
    size_t len;

    if (!(wcs = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, name, strlen(name)+1,
                                          &len)))
        return -1;

    fd = _wopen(wcs, oflags, pmode);

    free(wcs);

    if (fd != -1) {

        /* empty buffering structure */
        libcu8_fd_buffers[fd].rcount = 0;
        libcu8_fd_buffers[fd].len = 0;

    }

    return fd;
}


__LIBCU8__IMP __cdecl int libcu8_sopen(char* name, int oflags, int shflags, int pmode)
{
    wchar_t *wcs;
    int fd;
    size_t len;

    if (!(wcs = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, name, strlen(name)+1,
                                          &len)))
        return -1;

    fd = _wsopen(wcs, oflags, shflags, pmode);

    free(wcs);

    if (fd != -1) {

        /* empty buffering structure */
        libcu8_fd_buffers[fd].rcount = 0;
        libcu8_fd_buffers[fd].len = 0;

    }

    return fd;
}

__LIBCU8__IMP __cdecl int libcu8_creat(char* name, int pmode)
{
    wchar_t *wcs;
    int fd;
    size_t len;

    if (!(wcs = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, name, strlen(name)+1,
                                          &len)))
        return -1;

    fd = _wcreat(wcs, pmode);

    free(wcs);

    if (fd != -1) {

        /* empty buffering structure */
        libcu8_fd_buffers[fd].rcount = 0;
        libcu8_fd_buffers[fd].len = 0;

    }

    return fd;

}

__LIBCU8__IMP __cdecl int libcu8_commit(int fd)
{
    HANDLE handle = osfhnd(fd);

    if (FlushFileBuffers(handle)) {
        errno = GetLastError();
        return errno;
    }

    libcu8_fd_buffers[fd].rcount = 0;
    libcu8_fd_buffers[fd].len = 0;

    return 0;
}

__LIBCU8__IMP __cdecl int libcu8_lseek(int fd, long offset, int origin)
{
    HANDLE handle = osfhnd(fd);
    int ret;

    if ((ret = SetFilePointer(handle, offset, NULL, origin)) == -1) {
        errno = GetLastError();
    }

    libcu8_fd_buffers[fd].rcount = 0;
    libcu8_fd_buffers[fd].len = 0;

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_fd_set_inheritance(int fd, int mode)
{
    HANDLE handle = osfhnd(fd);

    SetHandleInformation(handle, HANDLE_FLAG_INHERIT, mode);

    osfile(fd) &= ~ NOINHERIT;

    return 0;
}

/* CRT function replacement for file management functions */
__LIBCU8__IMP __cdecl int libcu8_remove(const char* file)
{
    wchar_t *wcs;
    int ret;
    size_t len;

    if (!(wcs = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, file, strlen(file)+1,
                                          &len)))
        return -1;

    ret = _wremove(wcs);
    free(wcs);

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_unlink(const char* file)
{
    wchar_t *wcs;
    int ret;
    size_t len;

    if (!(wcs = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, file, strlen(file)+1,
                                          &len)))
        return -1;

    ret = _wunlink(wcs);
    free(wcs);

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_rename(const char* oldn, const char* newn)
{
    wchar_t *wold=NULL, *wnew=NULL;
    int ret;
    size_t len;

    if ((wold = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, oldn, strlen(oldn)+1,
                                          &len))
        && (wnew = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, newn, strlen(newn)+1,
                                          &len))) {

        ret = _wrename(wold, wnew);

    } else
        ret = -1;


    if (wold)
        free(wold);
    if (wnew)
        free(wnew);

    return ret;
}
