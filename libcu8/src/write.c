/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015, 2016 Romain GARBI (Darkbatcher)

 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of the name of Romain Garbi (Darkbatcher) nor the
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
#include <stdarg.h>

#include "internals.h"

__LIBCU8__IMP __cdecl int libcu8_fprintf(FILE* f, const char* fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = libcu8_vfprintf(f, fmt, args);
    va_end(args);

    return ret;
}

__LIBCU8__IMP __cdecl int libcu8_vfprintf(FILE* f, const char* fmt, va_list args)
{
    char int_buf[512], *out = int_buf;
    int sz=sizeof(int_buf), nd, ret;
    va_list cpy;

    va_copy(cpy, args); /* create a duplicate just in case */

    nd = _vsnprintf(out, sz, fmt, args);

    if (nd < 0)
        goto err;

    if (nd + 1 > sz) {

        sz = nd + 1;
        if ((out = malloc(sz)) == NULL)
            goto err;

        nd = _vsnprintf(out, sz, fmt, cpy);
    }

    ret = libcu8_fputs(out, f);

err:
    va_end(cpy);

    if (out && out != int_buf)
        free(out);

    return (ret == EOF) ? -1 : nd;
}


__LIBCU8__IMP __cdecl int libcu8_fputc(int c, FILE* f)
{
    char buf[] = {c, 0};

    return libcu8_fputs(buf, f);
}

__LIBCU8__IMP __cdecl int libcu8_fputs(const char* s, FILE* f)
{
    int fd = fileno(f),
        ret;
    size_t sz = strlen(s), wrt;


    if (isatty(fd)) {
        /* this is a real tty, convert to unicode before writing anything */
        ret = libcu8_write_console(fd, s, sz, &wrt);

    } else {
        /* this is a regular file (aka. ansi, convert it back to ansi) */
        ret = libcu8_write_file(f, s, sz, &wrt);

    }

    if (ret == 0)
        return EOF;
    else
        return 1;
}


void libcu8_dbg_msg(const char* fmt, ...)
{
    va_list args;
    HANDLE out;
    char str[256];
    DWORD cnt;

    va_start(args, fmt);
    out = GetStdHandle(STD_ERROR_HANDLE);

    _vsnprintf(str, sizeof(str), fmt, args);
    WriteConsole(out, str, strlen(str), &cnt, NULL);

    va_end(args);
}

int libcu8_write_console(int fd, void* buf, size_t cnt, size_t* written)
{
    wchar_t* wcs;
    size_t size;
    DWORD wrt;
    HANDLE file = (HANDLE)_get_osfhandle(fd);
    struct fd_buffering_t* buffering = &(libcu8_fd_buffers[fd]);

    if ((wcs = (wchar_t*)libcu8_convert(LIBCU8_TO_U16, buf, cnt, buffering->remainder,
                              &(buffering->rcount),
                              sizeof(buffering->remainder), &size)) == NULL)
        return 0;

    if (!WriteConsoleW(file, wcs, size / sizeof(wchar_t), &wrt, NULL)) {
        free(wcs);
        return 0;
    }

    if (wrt == size /sizeof(wchar_t)) {

        *written = cnt;

    } else {

        /* hard to figure out how many bytes from buf have been written
           since conversion is not fixed size to fixed size. To make it
           easier, pretend we wrote wrote nothing */
        *written = 0;
    }

    free(wcs);

    return 1;
}

int libcu8_write_file(FILE* f, void* buf, size_t cnt, size_t* written)
{
    char* str = NULL;
    size_t size, wrt;

    struct fd_buffering_t* buffering = &(libcu8_fd_buffers[fileno(f)]);

    if ((str = libcu8_convert(LIBCU8_TO_ANSI, buf, cnt, buffering->remainder,
                    &(buffering->rcount), sizeof (buffering->remainder),
                                                            &size)) == NULL)
        return 0;

    wrt = fwrite(str, size, 1, f);

    if (wrt == size) {

        *written = cnt;

    } else {

        /* hard to figure out how many bytes from buf have been written
           since conversion is not fixed size to fixed size. To make it
           simple, pretend we wrote nothing */
        *written = 0;

    }

    free(str);

    return 1;
}
