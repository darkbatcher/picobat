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
#include "libcu8.h"


__LIBCU8__IMP __cdecl int libcu8_write(int fd, void* buf, unsigned int cnt)
{
    struct ioinfo* info;
    int ret;


    if (!IS_VALID(fd)) {

        errno = EBADF;
        return -1;

    }

    if (osfile(fd) & APPEND) {

        lseek(fd, 0, FILE_END);

    }

    info = pioinfo(fd);

    EnterCriticalSection(&(info->lock));

    ret = libcu8_write_nolock(fd, buf, cnt);

    LeaveCriticalSection(&(info->lock));

    return ret;
}

void libcu8_dbg_msg(const char* fmt, ...)
{
    va_list args;
    HANDLE out;
    char str[256];
    int cnt;

    va_start(args, fmt);
    out = GetStdHandle(STD_ERROR_HANDLE);

    _vsnprintf(str, sizeof(str), fmt, args);
    WriteConsole(out, str, strlen(str), &cnt, NULL);

    va_end(args);
}

__LIBCU8__IMP __cdecl int libcu8_write_nolock(int fd, void* buf,
                                                unsigned int cnt)
{
    HANDLE file;
    int mode,
        ret;
    DWORD wrt;
    size_t cvt,
            size = cnt,
            written;
    char* text;

    file = osfhnd(fd);
    mode = osfile(fd);

    if (buf == NULL || cnt == 0 || IS_ATEOF(mode)) {

        /* are we really supposed to do anything here */
        //libcu8_dbg_msg("libcu8_write: [%d] writing nothing !\r\n", fd);
        return 0;

    }

    if (!IS_TEXTMODE(mode)) {

        /* The file uses binary, that is, no conversion needed */
        //libcu8_dbg_msg("libcu8_write: [%d] writing to binary file !\r\n", fd);
        ret = WriteFile(osfhnd(fd), buf, cnt, &wrt, NULL);
        written = wrt;

    } else if (libcu8_is_tty(file)) {

        /* this is a real tty, convert to unicode before writing anything */
        //libcu8_dbg_msg("libcu8_write: [%d] writing to console !\r\n", fd);
        ret = libcu8_write_console(fd, buf, cnt, &written);

    } else if (IS_PIPE(mode) || libcu8_dummy) {

        if (!(text = libcu8_lf_to_crlf(buf, cnt, &cvt)))
            return -1;

        //libcu8_dbg_msg("libcu8_write: [%d] writing to text file !\r\n", fd);

        /* this is a regular file (aka. ansi, convert it back to ansi) */
        ret = WriteFile(osfhnd(fd), text, cvt, &written, NULL);

        /* In case of success, return count without counting added \r */
        if (written)
            written = cnt;

        free(text);

    } else {

        if (!(text = libcu8_lf_to_crlf(buf, cnt, &cvt)))
            return -1;

        //libcu8_dbg_msg("libcu8_write: [%d] writing to text file !\r\n", fd);

        /* this is a regular file (aka. ansi, convert it back to ansi) */
        ret = libcu8_write_file(fd, text, cvt, &written);

        /* In case of success, return count without counting added \r */
        if (written)
            written = cnt;

        free(text);

    }

    if (!ret) {

        if (GetLastError() == ERROR_BROKEN_PIPE)
            return 0; /* msvcrt does so */

        errno = EBADF;
        return -1;

    }

    return written;
}

int libcu8_write_console(int fd, void* buf, size_t cnt, size_t* written)
{
    wchar_t* wcs;
    size_t size;
    DWORD wrt;
    HANDLE file = osfhnd(fd);
    struct fd_buffering_t* buffering = &(libcu8_fd_buffers[fd]);

    if ((wcs = libcu8_convert(LIBCU8_TO_U16, buf, cnt, buffering->remainder,
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
           easier, pretend we wrote nothing */
        *written = 0;
    }

    free(wcs);

    return 1;
}

int libcu8_write_file(int fd, void* buf, size_t cnt, size_t* written)
{
    char* str;
    size_t size;
    DWORD wrt;
    HANDLE file = osfhnd(fd);

    struct fd_buffering_t* buffering = &(libcu8_fd_buffers[fd]);

    if ((str = libcu8_convert(LIBCU8_TO_ANSI, buf, cnt, buffering->remainder,
                    &(buffering->rcount), sizeof (buffering->remainder),
                                                            &size)) == NULL)
        return 0;

    if (!WriteFile(file, str, size, &wrt, NULL)) {

        free(str);
        return 0;

    }

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

char* libcu8_lf_to_crlf(const char* buf, size_t len, size_t* cvt)
{
    char  *tmp=buf,
          *end = buf + len;
    char *ret;

    *cvt = len;

    /* compute the number of \n to be converted back to '\n' */
    while (tmp < end) {

        if (*tmp == '\n')
            (*cvt) ++;

        tmp ++;

    }

    if (!(ret = malloc(*cvt)))
        return NULL;

    tmp = ret;

    while (len) {

        if (*buf == '\n')
            *(tmp ++) = '\r';

        *(tmp ++) = *(buf ++);
        len --;

    }

    return ret;
}
