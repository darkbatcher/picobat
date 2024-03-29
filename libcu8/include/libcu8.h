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
#ifndef LIBCU8_H
#define LIBCU8_H

#ifndef __LIBCU8__IMP
#ifdef __LIBCU8__DLL
#define __LIBCU8__IMP __declspec(dllexport)
#else
#define __LIBCU8__IMP extern
#endif /* __LIBCU8__DLL */
#endif /* __LIBCU8__IMP */

/* enable C++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/stat.h>
#include <windows.h>
#include <io.h>
#include <dirent.h>

/* initialization function */
__LIBCU8__IMP __cdecl int libcu8_init(const char*** pargv);

/* encoding management functions */
__LIBCU8__IMP __cdecl void libcu8_get_fencoding(char* enc, size_t size);
__LIBCU8__IMP __cdecl int libcu8_set_fencoding(const char* enc);

/* conversion modes for libcu8_convert and libcu8_xconvert */
#define LIBCU8_TO_U16       0
#define LIBCU8_FROM_U16     1
#define LIBCU8_TO_ANSI      2
#define LIBCU8_FROM_ANSI    3

/* Convert function */
__LIBCU8__IMP __cdecl char* libcu8_convert(int mode, const char* src,
                                            size_t size, char* remainder,
                                            size_t* rcount, size_t rlen,
                                            size_t* converted);
__LIBCU8__IMP __cdecl char* libcu8_xconvert(int mode, const char* src,
                                            size_t size, size_t* converted);

/* CRT function replacement for low-level io */
#ifndef LIBCU8_NO_WRAPPERS
#undef fgetc
#undef getc
#define fgetc(a) libcu8_fgetc(a)
#define getc(a) libcu8_fgetc(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_fgetc(FILE *f);

#ifndef LIBCU8_NO_WRAPPERS
#undef fgets
#define fgets(a,b,c) libcu8_fgets(a,b,c)
#endif
__LIBCU8__IMP __cdecl char* libcu8_fgets(char* buf, int s, FILE* f);

#ifndef LIBCU8_NO_WRAPPERS
#undef fprintf
#define fprintf libcu8_fprintf
#endif
__LIBCU8__IMP __cdecl int libcu8_fprintf(FILE* f, const char* fmt, ...);

#ifndef LIBCU8_NO_WRAPPERS
#undef vfprintf
#define vfprintf libcu8_vfprintf
#endif
__LIBCU8__IMP __cdecl int libcu8_vfprintf(FILE* f, const char* fmt, va_list args);

#ifndef LIBCU8_NO_WRAPPERS
#undef fputc
#undef putc
#define fputc(a,b) libcu8_fputc(a,b)
#define putc(a,b) libcu8_fputc(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_fputc(int c, FILE* f);

#ifndef LIBCU8_NO_WRAPPERS
#undef fputs
#define fputs(a,b) libcu8_fputs(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_fputs(const char* s, FILE* f);

#ifndef LIBCU8_NO_WRAPPERS
#undef fopen
#define fopen(a,b) libcu8_fopen(a,b)
#endif
__LIBCU8__IMP __cdecl FILE* libcu8_fopen(const char* __restrict__ name, const char* __restrict__ mode);

#ifndef LIBCU8_NO_WRAPPERS
#undef open
#define open libcu8_open
#endif
__LIBCU8__IMP __cdecl int libcu8_open(char* name, int oflags, ...);

#ifndef LIBCU8_NO_WRAPPERS
#undef sopen
#define sopen(a,b,c,d) libcu8_sopen(a,b,c,d)
#endif
__LIBCU8__IMP __cdecl int libcu8_sopen(char* name, int oflags,
                                            int shflags, int pmode);

#ifndef LIBCU8_NO_WRAPPERS
#undef creat
#define creat(a,b) libcu8_creat(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_creat(char* name, int pmode);

#ifndef LIBCU8_NO_WRAPPERS
#undef lseek
#define lseek(a,b,c) libcu8_lseek(a,b,c)
#endif
__LIBCU8__IMP __cdecl int libcu8_lseek(int fd, long offset, int origin);

#ifndef LIBCU8_NO_WRAPPERS
#undef fseek
#define fseek(a,b,c) libcu8_fseek(a,b,c)
#endif
__LIBCU8__IMP __cdecl int libcu8_fseek(FILE* f, long offset, int origin);

#ifndef LIBCU8_NO_WRAPPERS
#undef commit
#define commit(a) libcu8_commit(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_commit(int fd);

#ifndef LIBCU8_NO_WRAPPERS
#undef fflush
#define fflush(a) libcu8_fflush(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_fflush(FILE* f);

#ifndef LIBCU8_NO_WRAPPERS
#undef dup
#define dup(a) libcu8_dup(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_dup(int fd);

#ifndef LIBCU8_NO_WRAPPERS
#undef dup2
#define dup2(a,b) libcu8_dup2(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_dup2(int fd1, int fd2);

/* CRT function replacement for file management functions */
#ifndef LIBCU8_NO_WRAPPERS
#undef remove
#define remove(a) libcu8_remove(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_remove(const char* file);

#ifndef LIBCU8_NO_WRAPPERS
#undef rename
#define rename(a,b) libcu8_rename(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_rename(const char* oldn, const char* newn);

#ifndef LIBCU8_NO_WRAPPERS
#undef unlink
#define unlink(a) libcu8_unlink(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_unlink(const char* file);

#ifndef LIBCU8_NO_WRAPPERS
#undef chmod
#define chmod(a,b) libcu8_chmod(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_chmod(const char* file, int mode);

#ifndef LIBCU8_NO_WRAPPERS
#undef getcwd
#define getcwd(a,b) libcu8_getcwd(a,b)
#endif
__LIBCU8__IMP __cdecl int libcu8_getcwd(char* dir, size_t size);

#ifndef LIBCU8_NO_WRAPPERS
#undef chdir
#define chdir(a) libcu8_chdir(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_chdir(const char* dir);

#ifndef LIBCU8_NO_WRAPPERS
#undef rmdir
#define rmdir(a) libcu8_rmdir(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_rmdir(const char* dir);

#ifndef LIBCU8_NO_WRAPPERS
#undef mkdir
#define mkdir(a) libcu8_mkdir(a)
#endif
__LIBCU8__IMP __cdecl int libcu8_mkdir(const char* dir);

/* CRT functions replacement for spawn*/
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnl(int mode, const char* file, ...);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnle(int mode, const char* file, ...);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnlp(int mode, const char* file, ...);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnlpe(int mode, const char* file, ...);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnv(int mode, const char* file,
                                                    const char* const *argv);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnve(int mode, const char* file,
                            const char* const *argv, const char *const *envp);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnvp(int mode, const char* file,
                                                const char* const *argv);
__LIBCU8__IMP __cdecl intptr_t libcu8_spawnvpe(int mode, const char* file,
                                                    const char* const *argv,
                                                    const char *const *envp);


__LIBCU8__IMP __cdecl int libcu8_fd_set_inheritance(int fd, int mode);

__LIBCU8__IMP __cdecl void (*libcu8_completion_handler)(const char*, char**);
__LIBCU8__IMP __cdecl void (*libcu8_completion_handler_free)(char*);


#ifdef dirent
#undef dirent
#endif

#ifdef DIR
#undef DIR
#endif

#ifdef opendir
#undef opendir
#endif

#ifdef closedir
#undef closedir
#endif

#ifdef readdir
#undef readdir
#endif



struct libcu8_dirent {
    char* d_name;
    int ret;
};

typedef struct libcu8_DIR {
    HANDLE h;
    struct libcu8_dirent ent;
} libcu8_DIR;

__LIBCU8__IMP __cdecl libcu8_DIR* libcu8_opendir(const char* dir);
__LIBCU8__IMP __cdecl int libcu8_closedir(libcu8_DIR* pdir);
__LIBCU8__IMP __cdecl struct libcu8_dirent* libcu8_readdir(libcu8_DIR* pdir);

#define DIR libcu8_DIR
#define dirent libcu8_dirent

#ifndef __LIBCU8__DLL
#define readdir(pdir) libcu8_readdir(pdir)
#define closedir(pdir) libcu8_closedir(pdir)
#define opendir(dir) libcu8_opendir(dir)
#endif

/* enable c++ compatibility */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* INTERNALS_H */
