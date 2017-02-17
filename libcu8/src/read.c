/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015, 2016, 2017 Romain GARBI

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

struct fd_buffering_t* libcu8_fd_buffers;

__LIBCU8__IMP __cdecl int libcu8_read(int fd, void* buf, unsigned int cnt)
{
    struct ioinfo* info = pioinfo(fd);
    int ret;

    if (!IS_VALID(fd)) {

        errno = EBADF;
        return -1;

    }

    /* printf("libcu8 read() !\n"); */

    //*((char*)NULL) = NULL;

    /* lock fd and call nolock version */
    EnterCriticalSection(&(info->lock));

    ret = libcu8_read_nolock(fd, buf,cnt);

    LeaveCriticalSection(&(info->lock));

    return ret;
}

int libcu8_is_tty(void* hndl)
{
    DWORD mode;
    int ret;

    if (GetConsoleMode(hndl, &mode) == 0 &&
            GetLastError() == ERROR_INVALID_HANDLE)
        return 0;

    return 1;
}

__LIBCU8__IMP __cdecl int libcu8_read_nolock(int fd, void* b, unsigned int cnt_arg)
{
    HANDLE file;
    int mode,
        ret;
    char *buf = (char*)b,
         *orig = b, *p;
    size_t cnt = cnt_arg,
           len = cnt,
           written;

    DWORD nbread=0, wrt;

    mode = osfile(fd);
    file = osfhnd(fd);

    if (buf == NULL || cnt == 0 || IS_ATEOF(mode) ) {

        /* nothing much to read */
        return 0;

    }

    libcu8_read_buffered (fd, &buf, &cnt);
    nbread = len - cnt;

    if (cnt == 0)
        return nbread;

    if (!IS_TEXTMODE(mode)) {
        /* Whatever, if the file is using binary mode, just get input from the
           ReadFile API, without any kind of translation or encoding
           conversion */

        /* fprintf(stderr, "libcu8: [%d] Reading binary file ! \n", file); */

        ret = ReadFile(file, buf, cnt, &wrt, NULL);
        written = wrt;

    } else if (libcu8_is_tty(file)) {

        /* Since ReadConsoleA is broken for many encodings, and ReadConsoleW is
           just pain in the ass (can't support byte-to-byte transmission, that
           is, can only return full wchar_t characters), We prefer using our
           own read console function */

        /* fprintf(stderr, "libcu8: [%d] reading tty ! \n", file); */
        ret = libcu8_readconsole(fd, buf, cnt, &written);

    } else {

        /* For other files (such as regular files, pipes or device) opened in text
           mode, just convert from ascii to utf8 characters. */
        /* fprintf(stderr, "Calling readfile(%d, %p, %p , &written)\n", fd, buf, cnt);
        fprintf(stderr, "libcu8: [%d] reading text file ! \n", file); */
        ret = libcu8_readfile(fd, buf, cnt, &written);
        /* fprintf(stderr, "%d = readfile(%d, %p, %p , %d)\n", ret, fd, buf, cnt, written); */

    }

    if (!ret) {
        if (GetLastError() == ERROR_BROKEN_PIPE)
            return 0; /* msvcrt does so */

        errno = EBADF;
        return -1;
    }

    nbread += written;

    return nbread;
}

int libcu8_readfile(int fd, char* buf, size_t size, size_t* written)
{
    const size_t orig = size;
    void* handle = osfhnd(fd);
    char utf8[4],
         ansi[FILENAME_MAX],
         *pin,
         *pout,
         last = pipech(fd);
    int  ret = 0;
    size_t su8=sizeof(utf8), sansi = 0 ;

    /* fprintf(stderr, "Starting (Size = %d)\n", size); */

    pipech(fd) = 0; /* Clear remaining \r if set */

    /* fprintf(stderr, "Getting context\n"); */

    iconv_t context = libcu8_mode2context(LIBCU8_FROM_ANSI);

    /* fprintf(stderr, "Return\n"); */

    if (context == (iconv_t) -1)
        return -1;

    /* fprintf(stderr, "[libcu8_readfile] {\n"); */

    while (size) {

        /* fprintf(stderr, "Getting byte (Size = %d)\n", size); */

        ret = libcu8_get_file_byte(handle, ansi, &sansi);

        switch (ret) {

            case -1:
                /* error */
                goto err;

            case 0:
                /* EOF */
                goto next;

        }

        /* fprintf(stderr, "Trying to convert\n"); */

        ret = libcu8_try_convert(context, ansi, &sansi, utf8, &su8);

        switch (ret) {

            case 0:
                if (last == '\r' && *utf8 != '\n') {
                    /* fprintf(stderr, "Got \\r without following  \\n\n");
                    fprintf(stderr, "Writing %d {0x%x, 0x%x, 0x%x, 0x%x}\n", su8, utf8[0], utf8[1], utf8[2], utf8[3]); */
                    libcu8_write_buffered(fd, &buf, &size, &last, 1);

                    if (*utf8 == '\r' && size == 0) {
                        pipech(fd) = '\r';
                        continue;
                    }
                }

                if (*utf8 == '\r') {
                    last = '\r';
                    /* fprintf(stderr, "Got \\r\n"); */
                    continue;
                } else {
                    last = 0;
                }

                /* We were able to get an utf8 character, write it*/
                /* fprintf(stderr, "Writing %d {0x%x, 0x%x, 0x%x, 0x%x}\n", su8, utf8[0], utf8[1], utf8[2], utf8[3]); */
                libcu8_write_buffered(fd, &buf, &size, utf8, su8);
                su8 = sizeof(utf8);
                break;

            case -1:
                /* came accross an invalid character sequence, skip it*/
                sansi = 0;
                su8 = sizeof(utf8);

        }

        /* fprintf(stderr, "%d, ", size); */

    }

next:

    /* fprintf(stderr,"End\n"); */

    iconv_close(context);

    *written = orig - size;

    return 1;

err:
    iconv_close(context);
    return 0;

}

int libcu8_get_file_byte(void* handle, char* buf, size_t* sansi)
{
    DWORD wrt = 0;
    int ret;

    if (*sansi >= FILENAME_MAX)
        return -1; /* we ran out of space in the buffer, report error */

    //fprintf(stderr, "Calling ReadFile(%d, %p, %d, %p, NULL)\n", handle, buf + *sansi, 1, &wrt);

    ret = ReadFile(handle, buf + *sansi, 1, &wrt, NULL);

    //fprintf(stderr, "Read %d bytes ret = %d\n", wrt, ret);

    if ((!ret) && GetLastError() != ERROR_BROKEN_PIPE)
        return -1; /* report error */

    //if (GetLastError() == ERROR_BROKEN_PIPE)
    //    fprintf(stderr, "ERROR_BROKEN_PIPE\n");

    if (wrt == 0)
        return 0; /* we do not read anything */

    *sansi += 1;
    return 1;
}

int libcu8_try_convert(iconv_t context, char* in, size_t* insize,
                                                char* utf8, size_t* outsize)
{
    char *inorig = in;
    size_t inlen = *insize,
           outlen = *outsize;
    int ret;

    ret = iconv(context, &in, &inlen, &utf8, &outlen);

    if (ret == (size_t)-1) {

        switch (errno) {
            case EINVAL:
            case E2BIG:
                /* We either came across an unterminated character
                   sequence, or a too short buffer. Anyway, it should
                   easily be recoverable */
                break;

            case EILSEQ:
                /* came across an invalid input sequence, give up with
                   input functions */
                return -1;
        }

    }

    if (*insize != inlen) {

        /* Apparently, iconv swallowed some bytes from the input buffer
           with(out) writing a byte to the output. Well, try to handle this
           by adjusting input buffer */

        memcpy(inorig, in, inlen);
        *insize = inlen;
    }

    if (*outsize != outlen) {

        /* We actually converted some bytes, this is quite
           cool */

        *outsize -= outlen;
        return 0; /* return 0 to indicate that something
        was actually translated */
    }

    return 1;

}

#define DEL 0x08
#define TAB 0x09
#define TAB_LEN 8
int libcu8_readconsole(int fd, char* buf, size_t size, size_t* written)
{
    void *handle = osfhnd(fd), *conout = GetStdHandle(STD_OUTPUT_HANDLE);
    char utf8[4]="", wcs[2*sizeof(wchar_t)],
         *pin,
         *pout,
         last = 0;
    wchar_t* wstr;
    const size_t orig = size;
    size_t wlen, wlen_tmp,
            len;
    DWORD wrt;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int ret;

    iconv_t context = libcu8_mode2context(LIBCU8_FROM_U16);

    *written  = 0;

    if (context == (iconv_t)-1)
        return -1;

    GetConsoleScreenBufferInfo(conout, &csbi);

    while (size >= 1 && *utf8 != '\n') {

        ret = libcu8_get_console_input(handle, wcs, &wlen);

        switch (ret) {
            case -1: /* error */
                goto err;

            case 0: /* EOF */
                osfile(fd) |= ATEOF;
                goto next;

        }

        len = sizeof(utf8);
        wlen_tmp = wlen;
        pin = wcs;
        pout = utf8;
        if (iconv(context, &pin, &wlen_tmp, &pout, &len) == (size_t)-1)
            goto err;

        //printf("wlen = %d\n", wlen);

        switch (*utf8) {

            case DEL:
                /* try to remove preceding character */
                if (!libcu8_delete_character(&buf, &size, orig))
                    continue; /* There wasn't any character in the buffer */

                if (*buf == '\t') {
                    /* Tabs are complicated to figure out, just redraw the
                       text */
                    SetConsoleCursorPosition(conout, csbi.dwCursorPosition);

                    if (!(wstr = libcu8_xconvert(LIBCU8_TO_U16,
                                                    buf - orig + size,
                                                    orig-size, &wlen_tmp)))
                        return -1;

                    WriteConsoleW(conout, wstr,
                                    wlen_tmp/sizeof(wchar_t), &wrt, NULL);

                    free(wstr);
                    continue;
                }

                libcu8_delete_console_character(conout);

                continue;

            case '\r':
                WriteConsoleW(conout, wcs, 1, &wrt, NULL);
                *((wchar_t*)wcs) = L'\n';
                *utf8 = '\n';

        }

        /* write character at the console */
        WriteConsoleW(conout, wcs, wlen / sizeof(wchar_t), &wrt, NULL);

        /* write to the buffer in with intermediate buffering function */
        libcu8_write_buffered(fd, &buf, &size, utf8, sizeof(utf8) - len);

    }
next:

    if (*utf8 != '\n') {
        while (!libcu8_get_console_wchar(handle, (wchar_t*)wcs)
               && *(wchar_t*)wcs != L'\r'
               );

        size --;
        *buf = '\n';
    }

    iconv_close (context);

    *written = orig - size;
    return 1;

err:
    iconv_close (context);
    return 0;
}


/* Check if wc is surrogate */
#define IS_SURROGATE(wc) ((wc) & 0xD800)
#define IS_SURROGATE_HIGH(wc) (IS_SURROGATE(wc) && !((wc) & 0x0400))
#define IS_SURROGATE_LOW(wc) (IS_SURROGATE(wc) && ((wc) & 0x0400))
int libcu8_get_console_input(void* handle, char* buf, size_t* size)
{
    wchar_t* wcs= (wchar_t*)buf,
             wc;

    /* loop while we do not get a valid starting wchar (ie. not the
       low part of a surrogate pair) */
    do {

        if (libcu8_get_console_wchar(handle, wcs))
            return -1;

    } while (IS_SURROGATE_LOW(*wcs));

retry:
    *size = sizeof(wchar_t);

    if (!IS_SURROGATE(*wcs)) {
        switch (*wcs) {
            case 0x1A:
                return 0;
            default:
                return 1;
        }
    }

    /* this is surrogate, deal with it */
    if (libcu8_get_console_wchar(handle, &wc))
        return -1;

    if (IS_SURROGATE_LOW(wc)) {

        *(wcs + 1) = wc;
        *size += sizeof(wchar_t);
        return 1;

    } else {

        /* Well, ReadConsoleInput is apparently throwing junk
           at us, but this is easily solvable */
        *wcs = wc;
        goto retry;

    }
}

int libcu8_get_console_wchar(void* handle, wchar_t* wc)
{
    INPUT_RECORD rec;
    DWORD written;

    static wchar_t swc;
    static unsigned int nb = 0;

    if (nb != 0) {
        /* We got a multiple keystroke last round */
        nb --;
        *wc = swc;
        return 0;
    }

    while (1) {

        if (!ReadConsoleInputW(handle, &rec, 1, &written))
            return -1;

        if (written != 0 && rec.EventType == KEY_EVENT
            && rec.Event.KeyEvent.bKeyDown == TRUE /* Key is pressed down */
            && rec.Event.KeyEvent.uChar.UnicodeChar != 0 /* Key not NUL */
            ) {

            if (rec.Event.KeyEvent.wRepeatCount > 1) {
                /* cope with MS bad function design. Indeed I never asked for
                   a multiple event. However, this doesn't seem to happen
                   anyway */
                swc = rec.Event.KeyEvent.uChar.UnicodeChar;
                nb = rec.Event.KeyEvent.wRepeatCount - 1;
            }

            *wc = rec.Event.KeyEvent.uChar.UnicodeChar;
            return 0;

        }

    }
}

/* Move buf pointer one character backwards */
#define IS_ASCII_CHAR(c) !((c) & 0x80)  /* starts with 0 */
#define IS_FOLLOWING_BYTE(c) (((c) & 0xC0) == 0x80) /* starts with 10 */
#define IS_LEADING_2_BYTE(c) (((c) & 0xE0) == 0xC0) /* starts with 110 */
#define IS_LEADING_3_BYTE(c) (((c) & 0xF0) == 0xE0) /* starts with 1110 */
#define IS_LEADING_4_BYTE(c) (((c) & 0xF8) == 0xF0) /* starts with 11110 */
#define IS_LEADING_BYTE(c) (IS_LEADING_2_BYTE(c) || IS_LEADING_3_BYTE(c) || \
                            IS_LEADING_4_BYTE(c))
int libcu8_delete_character(char** buf, size_t* size, const size_t orig)
{
    char tmp;

    if (*size >= orig)
        return 0;

    while (*size < orig) {

        /* Get one byte back */
        (*buf) --;
        (*size) ++;

        /* If not a following byte (ie. either ascii or leading byte), break
           we are at beginning of preceding character  */
        if (!IS_FOLLOWING_BYTE(**buf))
            break;
    }

    return 1;
}

void libcu8_delete_console_wchar(void* handle, char type)
{
    int i, max=1;
    for (i=0;i < max; i ++)
        libcu8_delete_console_character(handle);
}

void libcu8_delete_console_character(void* handle)
{
    wchar_t del_seq[] = {DEL, L' ', DEL};
    DWORD wrt;
    COORD pos, oldpos;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    /* Get cursor position prior to erasing character */
    GetConsoleScreenBufferInfo(handle, &csbi);
    oldpos = csbi.dwCursorPosition;

    if (csbi.dwCursorPosition.X == 0) {
        pos.Y = csbi.dwCursorPosition.Y - 1;
        pos.X = csbi.dwSize.X - 1;
        SetConsoleCursorPosition(handle, pos);
        WriteConsoleW(handle, del_seq + 1, 1, &wrt, NULL);
        SetConsoleCursorPosition(handle, pos);
    } else {

        WriteConsoleW(handle, del_seq, 3, &wrt, NULL);

    }
    /* Get new cursor position after erasing character */
}

void libcu8_write_buffered(int fd, char** buf, size_t* size,
                                            char* utf8, size_t len)
{
    struct fd_buffering_t* buffering = &(libcu8_fd_buffers[fd]);

    if (*size >= len) {

        memcpy(*buf, utf8, len);
        *size -= len;
        *buf += len;

    } else {

        memcpy(*buf, utf8, *size);
        *buf += *size;
        memcpy(buffering->remain, utf8 + *size, len - *size);
        buffering->len = len - *size;
        *size = 0;
    }
}

void libcu8_read_buffered(int fd, char** buf, size_t* size)
{
    struct fd_buffering_t* buffering = &(libcu8_fd_buffers[fd]);

    if (!buffering->len)
        return;

    if (buffering->len <= *size) {

        memcpy(*buf, buffering->remain, buffering->len);
        *size -= buffering->len;
        *buf += buffering->len;
        buffering->len = 0;

    } else {

        memcpy(*buf, buffering->remain, *size);
        memcpy(buffering->remain, buffering->remain + *size,
                                        buffering->len - *size);

        *buf += *size;
        buffering->len -= *size;
        *size = 0;

    }
}
