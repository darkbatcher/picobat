/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015, 2017 Romain GARBI

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
#include <direct.h>

#include <iconv.h>

#include "internals.h"
#include "config.h"
#include <libcu8.h>


#define REPLACE_FN( )

struct fn_replace_t functions[] = {
    {"_read", libcu8_read},
    {"_write", libcu8_write},
    {"_sopen", libcu8_sopen},
    {"_sopen", libcu8_sopen},
    {"_creat", libcu8_creat},
    {"_lseek", libcu8_lseek},
    {"_commit", libcu8_commit},
    {"_dup", libcu8_dup},
    {"_dup2", libcu8_dup2},
    {"_spawn", libcu8_spawnl},
    {"_spawnlp", libcu8_spawnlp}, /* 10th */
    {"_spawnlpe", libcu8_spawnlpe},
    {"_spawnle", libcu8_spawnle},
    {"_spawnv", libcu8_spawnv},
    {"_spawnve", libcu8_spawnve},
    {"_spawnvp", libcu8_spawnvp},
    {"_spawnvpe", libcu8_spawnvpe},
#ifdef HAVE__STAT
    {"_stat", libcu8_stat},
#endif
#ifdef HAVE__STAT32
    {"_stat32", libcu8_stat32},
#endif
#ifdef HAVE__STAT32I64
    {"_stat32i64", libcu8_stat32i64},
#endif
#ifdef HAVE__STAT64I32
    {"_stat64i32", libcu8_stat64i32}, /* 20th */
#endif
#ifdef HAVE__STAT64
    {"_stat64", libcu8_stat64},
#endif
#ifdef HAVE__FINDFIRST
    {"_findfirst", libcu8_findfirst},
    {"_findnext", libcu8_findnext},
#endif
#ifdef HAVE__FINDFIRST32
    {"_findfirst32", libcu8_findfirst32},
    {"_findnext32", libcu8_findnext32},
#endif
#ifdef HAVE__FINDFIRST32I64
    {"_findfirst32i64", libcu8_findfirst32i64},
    {"_findnext32i64", libcu8_findnext32i64},
#endif
#ifdef HAVE__FINDFIRST64
    {"_findfirst64", libcu8_findfirst64},
    {"_findnext64", libcu8_findnext64},
#endif
#ifdef HAVE__FINDFIRST64I32
    {"_findfirst64i32", libcu8_findfirst64i32}, /* 30th */
    {"_findnext64i32", libcu8_findnext64i32},
#endif
    {"_chdir", libcu8_chdir},
    {"_rmdir", libcu8_rmdir},
    {"_mkdir", libcu8_mkdir},
    {"_getcwd", libcu8_getcwd},
    {"_open", libcu8_open},
    {"fopen", libcu8_fopen}
};


/* initialize the new functions */
__LIBCU8__IMP __cdecl int libcu8_init(const char*** pargv)
{
    int i, n;
    HANDLE msvcrt;
    void* oldfn;

    /* Initialize the lock for fencoding */
    InitializeCriticalSection(&libcu8_fencoding_lock);
    EnterCriticalSection(&libcu8_fencoding_lock);

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


    if ((msvcrt = LoadLibraryW(L"msvcrt.dll")) == NULL) {

        free(libcu8_fd_buffers);
        return -1;

    }

    n = sizeof(functions)/sizeof(functions[0]);

    /* replace functions from msvcrt by functions from libcu8 */
    for (i = 0;i < n;i ++) {

        /* get the source function address */
        if ((oldfn = GetProcAddress(msvcrt, functions[i].name)) != NULL) {

            /* replace the function by those shipped with libcu8 */
            if (libcu8_replace_fn(oldfn, functions[i].fn, n))
            {

                free(libcu8_fd_buffers);
                return -1;

            }

        }
    }

    FreeLibrary(msvcrt);

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

    // fprintf(stderr, "Getting memory for argv\n");

    if (!(argv = malloc((argc + 1) * sizeof(char*)))) {

        // fprintf(stderr, "Failed to get memory for argv\n");
        errno = ENOMEM;
        return -1;

    }

    argv[argc] = NULL;

    for (i=0; i < argc; i ++) {

        //fwprintf(stderr, "Converting %d th argumenent \"%s\"\n", i, wargv[i]);

        if (!(argv[i] = libcu8_xconvert(LIBCU8_FROM_U16, (char*)wargv[i],
                                (wcslen(wargv[i])+1)*sizeof(wchar_t), &converted))) {

        //    fprintf(stderr, "Failed to convert %d th argumenent\n", i);
            return -1;

        }

    }

    // fprintf(stderr, "End lo");

    *pargv = (const char**)argv;

    return 0;
}
