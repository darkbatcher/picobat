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

#include "internals.h"
#include "libcu8.h"

#ifdef _findfirst
#undef _findfirst
#endif // _findfirst

#ifdef _findnext
#undef _findnext
#endif // _findnext

#ifdef _wfindfirst
#undef _wfindfirst
#endif // _findfirst

#ifdef _wfindnext
#undef _wfindnext
#endif // _findnext


/* Gen libcu8_findfirst64 and libcu8_findnext64 */
#define __libcu8_find_suffix 64
#include "find_gen.c"
#undef __libcu8_find_suffix

/* Gen libcu8_findfirst64i32 and libcu8_findnext64i32 */
#define __libcu8_find_suffix 64i32
#include "find_gen.c"
#undef __libcu8_find_suffix

#ifndef __x86_64__

/* Gen libcu8_findfirst64 and libcu8_findnext64 */
#define __libcu8_find_suffix 32
#include "find_gen.c"
#undef __libcu8_find_suffix

/* Gen libcu8_findfirst32i64 and libcu8_findnext32i64 */
#define __libcu8_find_suffix 32i64
#include "find_gen.c"
#undef __libcu8_find_suffix

#endif // __x86_64__
