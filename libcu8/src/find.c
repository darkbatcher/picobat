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
#include <sys/stat.h>

#include "config.h"
#include "internals.h"
#include "libcu8.h"

#ifdef HAVE__FINDFIRST64
/* Gen libcu8_findfirst64 and libcu8_findnext64 */

struct _xfinddata64_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    __int64 size;
    char name[260];
};

struct _xwfinddata64_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    __int64 size;
    wchar_t name[260];
};
#define __libcu8_finddata_t  _xfinddata64_t
#define __libcu8_wfinddata_t _xwfinddata64_t
#define __libcu8_wfindfirst  _wfindfirst64
#define __libcu8_wfindnext   _wfindnext64
#define __libcu8_findfirst   libcu8_findfirst64
#define __libcu8_findnext    libcu8_findnext64
#include "find_gen.c"
#endif

#ifdef HAVE__FINDFIRST64I32
/* Gen libcu8_findfirst64i32 and libcu8_findnext64i32 */
#undef __libcu8_finddata_t
#undef __libcu8_wfinddata_t
#undef __libcu8_wfindfirst
#undef __libcu8_wfindnext
#undef __libcu8_findfirst
#undef __libcu8_findnext

struct _xfinddata64i32_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    _fsize_t size;
    char name[260];
};

struct _xwfinddata64i32_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    _fsize_t size;
    wchar_t name[260];
};

#define __libcu8_finddata_t  _xfinddata64i32_t
#define __libcu8_wfinddata_t _xwfinddata64i32_t
#define __libcu8_wfindfirst  _wfindfirst64i32
#define __libcu8_wfindnext   _wfindnext64i32
#define __libcu8_findfirst   libcu8_findfirst64i32
#define __libcu8_findnext    libcu8_findnext64i32
#include "find_gen.c"
#undef __libcu8_find_suffix
#endif

#ifdef HAVE__FINDFIRST32
/* Gen libcu8_findfirst64 and libcu8_findnext64 */
#undef __libcu8_finddata_t
#undef __libcu8_wfinddata_t
#undef __libcu8_wfindfirst
#undef __libcu8_wfindnext
#undef __libcu8_findfirst
#undef __libcu8_findnext

struct _xwfinddata32_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    _fsize_t size;
    wchar_t name[260];
};

struct _xfinddata32_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    _fsize_t size;
    char name[260];
};

#define __libcu8_finddata_t  _xfinddata32_t
#define __libcu8_wfinddata_t _xwfinddata32_t
#define __libcu8_wfindfirst  _wfindfirst32
#define __libcu8_wfindnext   _wfindnext32
#define __libcu8_findfirst   libcu8_findfirst32
#define __libcu8_findnext    libcu8_findnext32
#include "find_gen.c"
#endif

#ifdef HAVE__FINDFIRST32I64
/* Gen libcu8_findfirst32i64 and libcu8_findnext32i64 */
#undef __libcu8_finddata_t
#undef __libcu8_wfinddata_t
#undef __libcu8_wfindfirst
#undef __libcu8_wfindnext
#undef __libcu8_findfirst
#undef __libcu8_findnext

struct _xfinddata32i64_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    __int64 size;
    char name[260];
};

struct _xwfinddata32i64_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    __int64 size;
    wchar_t name[260];
};

#define __libcu8_finddata_t  _xfinddata32i64_t
#define __libcu8_wfinddata_t _xwfinddata32i64_t
#define __libcu8_wfindfirst  _wfindfirst32i64
#define __libcu8_wfindnext   _wfindnext32i64
#define __libcu8_findfirst   libcu8_findfirst32i64
#define __libcu8_findnext    libcu8_findnext32i64
#include "find_gen.c"
#endif

#ifdef HAVE__FINDFIRST
#undef __libcu8_finddata_t
#undef __libcu8_wfinddata_t
#undef __libcu8_wfindfirst
#undef __libcu8_wfindnext
#undef __libcu8_findfirst
#undef __libcu8_findnext
#define __libcu8_finddata_t  _finddata_t
#define __libcu8_wfinddata_t _wfinddata_t
#define __libcu8_wfindfirst  _wfindfirst
#define __libcu8_wfindnext   _wfindnext
#define __libcu8_findfirst   libcu8_findfirst
#define __libcu8_findnext    libcu8_findnext
#include "find_gen.c"
#endif
