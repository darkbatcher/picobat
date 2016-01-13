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
__LIBCU8__IMP __cdecl char* libcu8_convert(int mode, char* src, size_t size,
                                            char* remainder, size_t* rcount,
                                              size_t rlen, size_t* converted);
__LIBCU8__IMP __cdecl char* libcu8_xconvert(int mode, char* src,
                                            size_t size, size_t* converted);

/* CRT function replacement for low-level io */
__LIBCU8__IMP __cdecl int libcu8_read_nolock(int fd, void* buf,
                                             unsigned int cnt);
__LIBCU8__IMP __cdecl int libcu8_read(int fd, void* buf, unsigned int cnt);
__LIBCU8__IMP __cdecl int libcu8_write_nolock(int fd, void* buf,
                                                unsigned int cnt);
__LIBCU8__IMP __cdecl int libcu8_write(int fd, void* buf, unsigned int cnt);
__LIBCU8__IMP __cdecl int libcu8_open(char* name, int oflags, int pmode);
__LIBCU8__IMP __cdecl int libcu8_creat(char* name, int pmode);
__LIBCU8__IMP __cdecl int libcu8_lseek(int fd, long offset, int origin);
__LIBCU8__IMP __cdecl int libcu8_commit(int fd);
__LIBCU8__IMP __cdecl int libcu8_dup(int fd);
__LIBCU8__IMP __cdecl int libcu8_dup_nolock(int fd);
__LIBCU8__IMP __cdecl int libcu8_dup2(int fd1, int fd2);
__LIBCU8__IMP __cdecl int libcu8_dup2_nolock(int fd1, int fd2);

/* CRT function replacement for file management functions */
__LIBCU8__IMP __cdecl int libcu8_remove(const char* file);
__LIBCU8__IMP __cdecl int libcu8_rename(const char* oldn, const char* newn);
__LIBCU8__IMP __cdecl int libcu8_unlink(const char* file);
__LIBCU8__IMP __cdecl int libcu8_stat(const char* file, struct _stat* buf);
__LIBCU8__IMP __cdecl int libcu8_stat32(const char* file,
                                            struct _stat32* buf);
__LIBCU8__IMP __cdecl int libcu8_stat64(const char* file,
                                            struct _stat64* buf);
__LIBCU8__IMP __cdecl int libcu8_stat32i64(const char* file,
                                            struct _stat32i64* buf);
__LIBCU8__IMP __cdecl int libcu8_stat64i32(const char* file,
                                            struct _stat64i32* buf);
__LIBCU8__IMP __cdecl int libcu8_chmod(const char* file, int mode);


/* enable c++ compatibility */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* INTERNALS_H */
