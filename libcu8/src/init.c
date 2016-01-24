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
#include <process.h>
#include <sys/stat.h>


#include <iconv.h>

#include "internals.h"
#include <libcu8.h>


#define REPLACE_FN( )


/* initialize the new functions */
__LIBCU8__IMP __cdecl int libcu8_init(const char*** pargv)
{
    int i;

    /* Initialize the lock for fencoding */
    InitializeCriticalSection(&libcu8_fencoding_lock);

    sprintf(libcu8_fencoding, "UTF-8");

    LeaveCriticalSection(&libcu8_fencoding_lock);


    //fprintf(stderr, "Allocating buffers\n");

    /* Allocate memory for libcu8 internal buffers */
    if (!(libcu8_fd_buffers = malloc(FD_BUFFERS_SIZE
                                        * sizeof(struct fd_buffering_t))))
        return -1;

    /* Reset buffers */
    for (i=0;i < FD_BUFFERS_SIZE; i++)
        libcu8_reset_buffered(i); /* reset all the buffers */

    //fprintf(stderr, "Getting argv\n");
    /* Get utf-8 encoded argv */
    if (pargv != NULL && libcu8_get_argv(pargv) == -1 )
        return -1;

    //fprintf(stderr, "Replace with functions\n");
    //printf("_read = %p, libcu8_read = %p, oldfn = %p\n", _read, libcu8_read);

    /* replace functions from msvcrt by functions from libcu8 */
    if (libcu8_replace_fn(_read, libcu8_read , 52) != 0
        || libcu8_replace_fn(_write, libcu8_write , 52) != 0
        || libcu8_replace_fn(_open, libcu8_open , 52) != 0
        || libcu8_replace_fn(_creat, libcu8_creat , 52) != 0
        || libcu8_replace_fn(_lseek, libcu8_lseek , 52) != 0
        || libcu8_replace_fn(_commit, libcu8_commit , 52) != 0
        || libcu8_replace_fn(_dup, libcu8_dup , 52) != 0
        || libcu8_replace_fn(_dup2, libcu8_dup2 , 52) != 0
        || libcu8_replace_fn(_spawnl, libcu8_spawnl , 52) != 0
        || libcu8_replace_fn(_spawnlp, libcu8_spawnlp , 52) != 0
        || libcu8_replace_fn(_spawnlpe, libcu8_spawnlpe , 52) != 0
        || libcu8_replace_fn(_spawnle, libcu8_spawnle , 52) != 0
        || libcu8_replace_fn(_spawnv, libcu8_spawnv , 52) != 0
        || libcu8_replace_fn(_spawnve, libcu8_spawnve , 52) != 0
        || libcu8_replace_fn(_spawnvp, libcu8_spawnvp , 52) != 0
        || libcu8_replace_fn(_spawnvpe, libcu8_spawnvpe , 52) != 0
#ifndef __x86_64__
        || libcu8_replace_fn(_stat32, libcu8_stat32, 52) != 0
        || libcu8_replace_fn(_stat32i64, libcu8_stat32i64, 52) != 0
#endif
        || libcu8_replace_fn(_stat64i32, libcu8_stat64i32, 52) != 0
        || libcu8_replace_fn(_stat64, libcu8_stat64, 52) != 0
#ifndef __x86_64__
        || libcu8_replace_fn(_findfirst32, libcu8_findfirst32, 52) != 0
        || libcu8_replace_fn(_findnext32, libcu8_findnext32, 52) != 0
        || libcu8_replace_fn(_findfirst32i64, libcu8_findfirst32i64, 52) != 0
        || libcu8_replace_fn(_findnext32i64, libcu8_findnext32i64, 52) != 0
#endif
        || libcu8_replace_fn(_findfirst64, libcu8_findfirst64, 52) != 0
        || libcu8_replace_fn(_findnext64, libcu8_findnext64, 52) != 0
        || libcu8_replace_fn(_findfirst64i32, libcu8_findfirst64i32, 52) != 0
        || libcu8_replace_fn(_findnext64i32, libcu8_findnext64i32, 52) != 0
        ) {

        free(libcu8_fd_buffers);
        return -1;

    }

    libcu8_save_changes();

    //fprintf(stderr, "Returning \n");

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

    //fprintf(stderr, "Getting memory for argv\n");

    if (!(argv = malloc((argc + 1) * sizeof(char*)))) {


        //fprintf(stderr, "Failed to get memory for argv\n");
        errno = ENOMEM;
        return -1;

    }

    argv[argc] = NULL;

    for (i=0; i < argc; i ++) {

        //fwprintf(stderr, L"Converting %d th argumenent \"%s\"\n", i, wargv[i]);

        if (!(argv[i] = libcu8_xconvert(LIBCU8_FROM_U16, (char*)wargv[i],
                                (wcslen(wargv[i])+1)*sizeof(wchar_t), &converted))) {

            //fprintf(stderr, "Failed to convert %d th argumenent\n", i);
            return -1;

        }

    }

    //fprintf(stderr, "End lol");

    *pargv = (const char**)argv;

    return 0;
}
