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

__LIBCU8__IMP __cdecl int libcu8_getcwd(char* dir, size_t size)
{
    wchar_t* wdir;
    char* ret;
    size_t conv;
    int code;

    /* alloc at most n wchar since it is the most character that
       dir can possibly hold */
    if (!(wdir = malloc(size*sizeof(wchar_t)))) {

        errno = ENOMEM;
        return -1;

    }

    if (code = _wgetcwd(wdir, size) == -1) {

        free(wdir);
        return -1;

    }

    if (!(ret = libcu8_xconvert(LIBCU8_FROM_U16, wdir,
                                    (wcslen(wdir)+1)*sizeof(wchar_t),
                                    &conv))) {
        errno = ENOMEM;
        free(wdir);
        return -1;

    }

    snprintf(dir, size, "%s", ret);

    free(ret);
    free(wdir);

    return code;
}

__LIBCU8__IMP __cdecl int libcu8_chdir(const char* dir)
{
    wchar_t* wdir;
    size_t conv;
    int ret;

    if (!(wdir = libcu8_xconvert(LIBCU8_TO_U16, dir,
                                    strlen(dir)+1, &conv))) {

        errno = ENOMEM;
        free(wdir);
        return -1;

    }

    ret = _wchdir(wdir);
    free(wdir);

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_rmdir(const char* dir)
{
    wchar_t* wdir;
    size_t conv;
    int ret;

    if (!(wdir = libcu8_xconvert(LIBCU8_TO_U16, dir,
                                    strlen(dir)+1, &conv))) {

        errno = ENOMEM;
        free(wdir);
        return -1;

    }

    ret = _wrmdir(wdir);
    free(wdir);

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_mkdir(const char* dir)
{
    wchar_t* wdir;
    size_t conv;
    int ret;

    if (!(wdir = libcu8_xconvert(LIBCU8_TO_U16, dir,
                                    strlen(dir)+1, &conv))) {

        errno = ENOMEM;
        free(wdir);
        return -1;

    }

    ret = _wmkdir(wdir);
    free(wdir);

    return ret;
}
