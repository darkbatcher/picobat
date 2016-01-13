/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015 Romain GARBI

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

#include <iconv.h>

#include "internals.h"
#include "libcu8.h"

#define nitems(array) (sizeof(array)/sizeof(array[0]))

struct libcu8_init_context_t {
    void* oldfn;
    void* newfn;
};

#define CONTEXT(old, new) {(void*) old, (void*) new}

/* initialize the new functions */
__LIBCU8__IMP __cdecl int libcu8_init(const char*** pargv)
{
    int i;
    struct libcu8_init_context_t array[] = {
        /* Hook low level io functions so that standard file functions do
           not have to be hooked */
        CONTEXT(_read, libcu8_read),
        CONTEXT(_write, libcu8_write),
        CONTEXT(_open, libcu8_open),
        CONTEXT(_creat, libcu8_creat),
        CONTEXT(_commit, libcu8_commit),
        CONTEXT(_lseek, libcu8_lseek),
        CONTEXT(NULL, NULL)
    };
    struct libcu8_init_context_t* func = array;

    /* Initialize the lock for fencoding */
    InitializeCriticalSection(&libcu8_fencoding_lock);

    sprintf(libcu8_fencoding, "UTF-8");

    LeaveCriticalSection(&libcu8_fencoding_lock);

    /* Allocate memory for libcu8 internal buffers */
    if (!(libcu8_fd_buffers = malloc(FD_BUFFERS_SIZE
                                        * sizeof(struct fd_buffering_t))))
        return -1;

    /* Reset buffers */
    for (i=0;i < FD_BUFFERS_SIZE; i++)
        libcu8_reset_buffered(i); /* reset all the buffers */

    /* Get utf-8 encoded argv */
    if (pargv != NULL && libcu8_get_argv(pargv) == -1 )
        return -1;

    while (func->newfn && func->oldfn) {

        if (libcu8_replace_fn(func->oldfn, func->newfn) != 0) {

            free(libcu8_fd_buffers);
            return -1;

        }

        func ++;
    }

    return 0;

}

typedef struct {
int newmode;
} _startupinfo;

extern int __wgetmainargs(int*, wchar_t***, wchar_t***, int, _startupinfo*);

int libcu8_get_argv(const char*** pargv)
{
    wchar_t **wargv, **wenv;
    char **argv;
    int argc, i;
    size_t converted;
    _startupinfo stinfo;

    __wgetmainargs(&argc, &wargv, &wenv, 0, &stinfo);

    if (!(argv = malloc(argc * sizeof(char*)))) {

        errno = ENOMEM;
        return -1;

    }

    argv[argc] = NULL;

    for (i=0; i < argc; i ++) {

        if (!(argv[i] = libcu8_xconvert(LIBCU8_FROM_U16, (char*)wargv[i],
                                wcslen(wargv[i])+sizeof(wchar_t), &converted))) {

            return -1;

        }

    }

    *pargv = (const char**)argv;

    return 0;
}
