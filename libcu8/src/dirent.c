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

/* this is an posix-compatible dirent.h layer added to libcu8 to prevent
   programs from using non-utf8 aware version of dirent.h for windows

   This is strictly posix-compatible, we don't include XSI shit, at least
   for the moment. We do not provide readdir_r but readdir is thread safe
   anyway */

__LIBCU8__IMP __cdecl DIR* libcu8_opendir(const char* dir)
{
    struct DIR* pdir;
    wchar_t* wdir;
    HANDLE handle;
    char* exp;
    size_t size, conv;
    WIN32_FIND_DATAW data;

    //printf("Libcu8_opendir used !\n");

    if (dir == NULL || ((size = strlen(dir))  <= 0)) {

        errno = ENOENT;
        return NULL;

    }

    // printf("libcu8_opendir: dir=\"%s\" size=%d\n", dir, size);

    size += sizeof("/*");

    if ((exp = malloc(size)) == NULL) {

        errno = ENOMEM;
        return NULL;

    }


    strcpy(exp, dir);
    strcat(exp, "/*");

    if ((wdir = (wchar_t*) libcu8_xconvert(LIBCU8_TO_U16, exp,
                                    size, &conv)) == NULL) {

        /* No more memory to convert utf-8 to utf-16 */
        errno = ENOMEM;
        free(exp);
        return NULL;

    }

    /* we don't need the expression with joker anymore */
    free(exp);

    if ((handle = FindFirstFileW(wdir, &data)) == INVALID_HANDLE_VALUE) {

        /* Failed to get a file from windows */
        errno = ENOENT;
        free(wdir);
        return NULL;

    }

    /* we don't need the utf-16 expression anymore */
    free(wdir);

    if ((exp = libcu8_xconvert(LIBCU8_FROM_U16, data.cFileName,
            (wcslen(data.cFileName)+1)*sizeof(wchar_t), &conv)) == NULL ) {

        errno = ENOMEM;
        CloseHandle(handle);
        return NULL;

    }


    if ((pdir = malloc(sizeof(struct DIR))) == NULL) {

        /* failed to alloc a DIR structure */
        errno = ENOMEM;
        CloseHandle(handle);
        return NULL;

    }

    pdir->h = handle;
    pdir->ent.d_name = exp;
    pdir->ent.ret = 0;

    return pdir;
}

__LIBCU8__IMP __cdecl int libcu8_closedir(DIR* pdir)
{
    if (pdir->ent.d_name != NULL)
        free(pdir->ent.d_name);

    FindClose(pdir->h);

    free(pdir);

    return 0;
}

__LIBCU8__IMP __cdecl struct dirent* libcu8_readdir(DIR* pdir)
{
    WIN32_FIND_DATAW data;
    size_t cnt;

    if (pdir->ent.ret == 0) {

        pdir->ent.ret = 1;
        return &(pdir->ent);

    }

    free(pdir->ent.d_name);
    pdir->ent.d_name = NULL;

    if (FindNextFileW(pdir->h, &data) == 0) {

        if (GetLastError() != ERROR_NO_MORE_FILES)
            errno = ENOENT;
        return NULL;

    }

    if ((pdir->ent.d_name = libcu8_xconvert(LIBCU8_FROM_U16, data.cFileName,
            (wcslen(data.cFileName)+1) * sizeof(wchar_t), &cnt)) == NULL) {

        errno = ENOMEM;
        return NULL;

    }

    return &(pdir->ent);
}
