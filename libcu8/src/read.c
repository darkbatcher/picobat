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
#include <string.h>

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
        ret,
        i,
        j;
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

    if (!IS_TEXTMODE(mode) /*|| IS_PIPE(mode)*/) {
        /* Whatever, if the file is using binary mode, just get input from the
           ReadFile API, without any kind of translation or encoding
           conversion */

        ret = ReadFile(file, buf, cnt, &wrt, NULL);
        written = wrt;

    } else if (libcu8_is_tty(file)) {

        /* Since ReadConsoleA is broken for many encodings, and ReadConsoleW is
           just pain in the ass (can't support byte-to-byte transmission, that
           is, can only return full wchar_t characters), We prefer using our
           own read console function */

        ret = libcu8_readconsole(fd, buf, cnt, &written);

    } else if (IS_PIPE(mode)) {

        /* Pipes are somewhat hard to deal with under windows, since
           it messes everything if you do not read at least two bytes at a time
           (who knows why ?). At first I thought it has something to deal with file
           interlocking, but apparently, it is just some kind of weird bug.  */
        ret = ReadFile(file, buf, cnt, &wrt, NULL);

        i = (j = 0);

        while (i + j < wrt) {

            if (j != 0)
                buf[i] = buf[i + j];

            if (buf[i] ==  '\r' ) {

                /* strip the \r from the input. In theory, we
                   should be able to handle this the right way, with
                   a somewhat really complex algorithm, but anyway,
                   who really cares ? */

                j ++;

                continue;

            }

            i++;

        }

        written = i;

    } else {

        /* For other files (such as regular files, pipes or device) opened in text
           mode, just convert from ascii to utf8 characters. */

        ret = libcu8_readfile(fd, buf, cnt, &written);

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
         last = pipech(fd);
    int  ret = 0;
    size_t su8=sizeof(utf8), sansi = 0 ;

    pipech(fd) = 0; /* Clear remaining \r if set */

    iconv_t context = libcu8_mode2context(LIBCU8_FROM_ANSI);

    if (context == (iconv_t) -1)
        return -1;

    while (size) {

        ret = libcu8_get_file_byte(handle, ansi, &sansi);

        switch (ret) {

            case -1:
                /* error */
                goto err;

            case 0:
                /* EOF */
                goto next;

        }

        ret = libcu8_try_convert(context, ansi, &sansi, utf8, &su8);

        switch (ret) {

            case 0:
                if (last == '\r' && *utf8 != '\n') {

                    libcu8_write_buffered(fd, &buf, &size, &last, 1);

                    if (*utf8 == '\r' && size == 0) {
                        pipech(fd) = '\r';
                        continue;
                    }
                }

                if (*utf8 == '\r') {
                    last = '\r';
                    continue;
                } else {
                    last = 0;
                }

                /* We were able to get an utf8 character, write it*/
                libcu8_write_buffered(fd, &buf, &size, utf8, su8);
                su8 = sizeof(utf8);
                break;

            case -1:
                /* came across an invalid character sequence, skip it*/
                sansi = 0;
                su8 = sizeof(utf8);

        }


    }

next:

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

    ret = ReadFile(handle, buf + *sansi, 1, &wrt, NULL);


    if ((!ret) && GetLastError() != ERROR_BROKEN_PIPE)
        return -1; /* report error */

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

struct libcu8_history_entry_t
                            libcu8_history[LIBCU8_HISTORY_SIZE];
CRITICAL_SECTION libcu8_history_lock;
int libcu8_history_count = 0;

__cdecl void (*libcu8_completion_handler)(const char*, char**) = NULL;
__cdecl void (*libcu8_completion_handler_free)(char*);

#define DEL 0x08
#define TAB 0x09
#define TAB_LEN 8
int libcu8_readconsole(int fd, char* buf, size_t size, size_t* written)
{
    void *handle = osfhnd(fd), *conout = GetStdHandle(STD_OUTPUT_HANDLE);
    char utf8[4]="", tutf8[4]="", wcs[2*sizeof(wchar_t)],
         *pin,
         *pout,
         *pos = buf, /* the position of the cursor in buf */
         *newpos,
         *completion,
         last = 0;

    wchar_t* wstr;
    const size_t orig = size;
    const char* orig_buf = buf;

    size_t wlen, wlen_tmp,
            len;
    DWORD wrt;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    struct libcu8_line_t line;

    int ret,
        vk = 0, /* virtual key for handling stuff like arrows and SUPPR */
        hid = 0; /* position on the history array */

    iconv_t context = libcu8_mode2context(LIBCU8_FROM_U16);

    *written  = 0;

    if (context == (iconv_t)-1)
        return -1;

    EnterCriticalSection(&libcu8_history_lock);

    if (libcu8_history_count)
        hid = libcu8_history_count;

    LeaveCriticalSection(&libcu8_history_lock);

    GetConsoleScreenBufferInfo(conout, &csbi);

    line.orig = csbi.dwCursorPosition;
    line.current = csbi.dwCursorPosition;
    line.end = csbi.dwCursorPosition;

    while (size >= 1 && *utf8 != '\n') {

        ret = libcu8_get_console_input(handle, wcs, &wlen, &vk);

        switch (ret) {
            case -1: /* error */
                goto err;

            case 0: /* EOF */
                osfile(fd) |= ATEOF;
                goto next;

        }

        switch (vk) {
        case 0:
            break;

        case VK_LEFT:
            if (pos > orig_buf) {

                pos --;
                libcu8_coord_decrement(&(line.current), &csbi);
                SetConsoleCursorPosition(conout, line.current);

            }
            continue;

        case VK_RIGHT:
            if (pos < buf) {

                pos ++;
                libcu8_coord_increment(&(line.current), &csbi);
                SetConsoleCursorPosition(conout, line.current);

            }
            continue;

        case VK_DELETE:

            if (pos < buf) {

                /* clear character on the right */
                newpos = libcu8_next_character(pos, buf);
                memmove(pos, newpos, buf - newpos);

                buf -= newpos - pos;
                size += newpos - pos;

                libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                                    pos - orig_buf, &line, &csbi);

                libcu8_clear_character(conout, &line);

            }
            continue;

        case VK_UP:
            EnterCriticalSection(&libcu8_history_lock);

            if (hid > 0)
                hid --;

            if ((libcu8_history_count > hid)
                && (libcu8_history[hid].size <= orig)) {

                memcpy(orig_buf, libcu8_history[hid].entry,
                                        libcu8_history[hid].size);

                ret = libcu8_count_characters(orig_buf, orig - size)
                        - libcu8_count_characters(libcu8_history[hid].entry,
                                              libcu8_history[hid].size);

                pos = (buf = orig_buf + libcu8_history[hid].size);
                size = orig - libcu8_history[hid].size;

                libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                                        pos - orig_buf, &line, &csbi);

                if (ret > 0)
                    FillConsoleOutputCharacterW(conout, L' ', ret, line.end, &wrt);

            }

            LeaveCriticalSection(&libcu8_history_lock);
            continue;

        case VK_DOWN:
            EnterCriticalSection(&libcu8_history_lock);

            if (hid < (libcu8_history_count - 1))
                hid ++;
            else
                hid = libcu8_history_count - 1;

            if (libcu8_history_count > hid
                && (libcu8_history[hid].size <= orig)) {

                memcpy(orig_buf, libcu8_history[hid].entry,
                                        libcu8_history[hid].size);

                ret = libcu8_count_characters(orig_buf, orig - size)
                        - libcu8_count_characters(libcu8_history[hid].entry,
                                              libcu8_history[hid].size);


                pos = (buf = orig_buf + libcu8_history[hid].size);
                size = orig - libcu8_history[hid].size;

                libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                                        pos - orig_buf, &line, &csbi);

                if (ret > 0)
                    FillConsoleOutputCharacterW(conout, L' ', ret, line.end, &wrt);

            }

            LeaveCriticalSection(&libcu8_history_lock);
            continue;

        case VK_END:
            pos = buf;
            line.current = line.end;
            SetConsoleCursorPosition(conout, line.current);
            continue;

        case VK_HOME: /* yeah, apparently, VK_HOME is what we usually
                         call begin ... */
            pos = orig_buf;
            line.current = line.orig;
            SetConsoleCursorPosition(conout, line.current);
            continue;

        default:
            continue;

        }

        len = sizeof(utf8);
        wlen_tmp = wlen;
        pin = wcs;
        pout = utf8;
        if (iconv(context, &pin, &wlen_tmp, &pout, &len) == (size_t)-1)
            goto err;

        switch (*utf8) {

            case DEL:
                /* Do some tricks to remove a character without damaging
                   the whole system. Shift characters to the left */

                if (pos > orig_buf) {
                    /* if we have anything to delete on the left */

                    /* clear character on the right */
                    newpos = libcu8_previous_character(pos, orig_buf);
                    memmove(newpos, pos, buf - pos);

                    buf -= pos - newpos;
                    size += pos - newpos;
                    pos = newpos;

                    libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                                        pos - orig_buf, &line, &csbi);

                    libcu8_clear_character(conout, &line);

                }

                continue;

            case TAB:
                if (libcu8_completion_handler) {
                    /*something to do with autocompletion */
                    newpos = libcu8_completion_get_item(pos, orig_buf, buf);

                    if (newpos) {

                        libcu8_completion_handler(newpos, &completion);

                        if (completion == (char*)-1) {
                            /* Well, apparently a list is to be printed. Make a simple
                               assertion : The prompt is the same length as the
                               previous one ... */

                            SetConsoleCursorPosition(conout, line.end);
                            libcu8_completion_handler(newpos, NULL);

                            GetConsoleScreenBufferInfo(conout, &csbi);
                            line.orig = csbi.dwCursorPosition;

                        } else if (completion != NULL) {

                            libcu8_completion_insert(completion, &pos, &buf, &size);
                            libcu8_completion_handler_free(completion);

                        }

                        libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                    pos - orig_buf, &line, &csbi);

                        free(newpos);

                    }
                }
                continue;

            case '\r':
                WriteConsoleW(conout, wcs, 1, &wrt, NULL);
                *((wchar_t*)wcs) = L'\n';
                *utf8 = '\n';

                /* write character at the console */
                WriteConsoleW(conout, wcs, wlen / sizeof(wchar_t), &wrt, NULL);

                libcu8_write_buffered(fd, &buf, &size, utf8, sizeof(utf8) - len);

                break;

            default:
                /* Let's do some clever tricks... move the
                   content of utf-8 into buf and put the extra bytes inside
                   utf-8 in order not break the buffered output system */

                if (pos == buf) {

                    /* Update pos. We don't risk anything as pos is never to be
                       reused again if the size of the buffer is exceeded */
                    pos += sizeof(utf8) - len;

                    libcu8_write_buffered(fd, &buf, &size, utf8, sizeof(utf8) - len);
                    libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                                        pos - orig_buf, &line, &csbi);
                    break; /* Nothing to do */

                }

                if ((buf - pos) < (sizeof(utf8) - len)) {

                    /* Apparently, buf is too close to pos to make this simple
                       just move the firsts bytes into the return buffer and
                       let the remaining ones inside utf8 buffer.

                       Use an intermediate tutf8 buffer to perform the modification

                       */

                    memcpy(tutf8, pos, buf - pos);
                    memcpy(pos, utf8, buf - pos);
                    memmove(utf8, utf8 + (buf - pos),
                                (sizeof(utf8) - len) - (buf - pos));
                    memcpy(utf8 + (sizeof(utf8) - len) - (buf - pos),
                            tutf8, buf - pos);

                } else {

                    memcpy(tutf8, buf - (sizeof(utf8) - len), sizeof(utf8) - len);
                    memmove(pos + sizeof(utf8) - len, pos,
                                (buf - pos) - (sizeof(utf8) - len));
                    memcpy(pos, utf8, sizeof(utf8) - len);
                    memcpy(utf8, tutf8, sizeof(utf8) - len);

                }

                pos += sizeof(utf8) - len;

                /* write to the buffer in with intermediate buffering function */
                libcu8_write_buffered(fd, &buf, &size, utf8, sizeof(utf8) - len);
                libcu8_refresh_console_line(conout, orig_buf, orig-size,
                                                        pos - orig_buf, &line, &csbi);

        }

    }
next:

    iconv_close (context);

    *written = orig - size;

    libcu8_history_add_entry(orig_buf, *written);

    return 1;

err:
    iconv_close (context);
    return 0;
}

int libcu8_refresh_console_line(void* handle, char* buf, size_t size,
                                    size_t cursor,
                                    struct libcu8_line_t* line,
                                    CONSOLE_SCREEN_BUFFER_INFO* csbi)
{
    wchar_t *wstr;
    size_t wlen;
    int wrt;
    unsigned int i;
    CONSOLE_CURSOR_INFO info;

    info.dwSize = 100;
    info.bVisible = FALSE;

    assert(cursor <= size);

    SetConsoleCursorInfo(handle, &info);
    SetConsoleCursorPosition(handle, line->orig);

    /* Update orig, just in case the windows has been resized */
    GetConsoleScreenBufferInfo(handle, csbi);
    line->orig.X = csbi->dwCursorPosition.X;
    line->orig.Y = csbi->dwCursorPosition.Y;
    line->current.X = line->orig.X;
    line->current.Y = line->orig.Y;

    if (cursor) {

        if (!(wstr = libcu8_xconvert(LIBCU8_TO_U16,
                                    buf,
                                    cursor, &wlen)))
            return -1;

        i = 0;
        while (i < wlen/sizeof(wchar_t)) {

            WriteConsoleW(handle, wstr + i, 1, &wrt, NULL);

            GetConsoleScreenBufferInfo(handle, csbi);

            if (line->current.Y == csbi->dwCursorPosition.Y
                && line->current.X > csbi->dwCursorPosition.X) {
                /* the console scrolled */
                if (line->orig.Y > 0)
                    line->orig.Y --;
            }

            line->current.X = csbi->dwCursorPosition.X;
            line->current.Y = csbi->dwCursorPosition.Y;

            i ++;

        }

        free(wstr);

    }

    line->end.X = line->current.X;
    line->end.Y = line->current.Y;

    if (size - cursor) {

        if (!(wstr = libcu8_xconvert(LIBCU8_TO_U16,
                                        buf + cursor,
                                        size - cursor, &wlen)))
            return -1;

        i = 0;
        while (i < wlen/sizeof(wchar_t)) {

            WriteConsoleW(handle, wstr + i, 1, &wrt, NULL);

            GetConsoleScreenBufferInfo(handle, csbi);

            if (line->end.Y == csbi->dwCursorPosition.Y
                && line->end.X > csbi->dwCursorPosition.X) {
                /* the console scrolled */
                if (line->orig.Y > 0)
                    line->orig.Y --;

                if (line->current.Y > 0)
                    line->current.Y --;
            }

            line->end.X = csbi->dwCursorPosition.X;
            line->end.Y = csbi->dwCursorPosition.Y;

            i ++;

        }

        free(wstr);

    }

    SetConsoleCursorPosition(handle, line->current);

    info.bVisible = TRUE;
    SetConsoleCursorInfo(handle, &info);

}

void libcu8_clear_character(void* handle, struct libcu8_line_t* line)
{
    int wrt;
    CONSOLE_CURSOR_INFO info;

    info.dwSize = 100;
    info.bVisible = FALSE;

    SetConsoleCursorInfo(handle, &info);
    SetConsoleCursorPosition(handle, line->end);

    WriteConsoleW(handle, L" ", 1, &wrt, NULL);

    SetConsoleCursorPosition(handle, line->current);
    info.bVisible = TRUE;
    SetConsoleCursorInfo(handle, &info);

}

void libcu8_coord_decrement(COORD* coords, CONSOLE_SCREEN_BUFFER_INFO* csbi)
{

    if (coords->X == 0) {
        coords->Y --;
        coords->X = csbi->dwSize.X - 1;

    } else
       coords->X --;

}

void libcu8_coord_increment(COORD* coords, CONSOLE_SCREEN_BUFFER_INFO* csbi)
{

    if (coords->X == csbi->dwSize.X - 1) {
        coords->Y ++;
        coords->X = 0;

    } else
       coords->X ++;

}

#define COORDS_EQUAL(c1,c2) ((c1.X == c2.X) && (c1.Y == c2.Y))
int libcu8_count_cells(COORD orig, COORD dest, CONSOLE_SCREEN_BUFFER_INFO* csbi)
{
    COORD destdec = dest;
    int cells = 0;

    while (!COORDS_EQUAL(orig, dest)
           && !COORDS_EQUAL(orig, destdec)) {

        libcu8_coord_increment(&dest, csbi);
        libcu8_coord_decrement(&destdec, csbi);

        cells ++;
    }

    return COORDS_EQUAL(orig, destdec) ? (cells) : (- cells);
}

/* Check if wc is surrogate */
#define IS_SURROGATE(wc) ((wc) & 0xD800)
#define IS_SURROGATE_HIGH(wc) (IS_SURROGATE(wc) && !((wc) & 0x0400))
#define IS_SURROGATE_LOW(wc) (IS_SURROGATE(wc) && ((wc) & 0x0400))
int libcu8_get_console_input(void* handle, char* buf, size_t* size, int* vk)
{
    wchar_t* wcs= (wchar_t*)buf,
             wc;

    /* loop while we do not get a valid starting wchar (ie. not the
       low part of a surrogate pair) */
    do {

        if (libcu8_get_console_wchar(handle, wcs, vk))
            return -1;

    } while (IS_SURROGATE_LOW(*wcs));

retry:
    *size = sizeof(wchar_t);

    if (!IS_SURROGATE(*wcs)) {
        switch (*wcs) {
            case 0x1A: /* CTRL-Z */
                return 0;
            default:
                return 1;
        }
    }

    /* this is surrogate, deal with it */
    if (libcu8_get_console_wchar(handle, &wc, NULL))
        return -1;

    if (IS_SURROGATE_LOW(wc)) {

        *(wcs + 1) = wc;
        *size += sizeof(wchar_t);
        return 1;

    } else {

        /* Well, ReadConsoleInput is apparently throwing junk
           at us, but this is easily solvable (try again) */
        *wcs = wc;
        goto retry;

    }
}

int libcu8_get_console_wchar(void* handle, wchar_t* wc, int* vk)
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

    if (vk)
        *vk = 0; /* reset vk */

    while (1) {

        if (!ReadConsoleInputW(handle, &rec, 1, &written))
            return -1;

        if (written != 0 && rec.EventType == KEY_EVENT
            && rec.Event.KeyEvent.bKeyDown == TRUE /* Key is pressed down */
            ) {

            if  (rec.Event.KeyEvent.uChar.UnicodeChar != 0 /* Key not NUL */
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

            } else if (rec.Event.KeyEvent.wVirtualKeyCode != 0 /* virtual key pressed */
                        ) {

                if (vk) {
                    *vk = rec.Event.KeyEvent.wVirtualKeyCode;
                    return 0;
                }

            }

        }
    }
}

void libcu8_history_add_entry(char* orig_buf, size_t size)
{
    char* buf;
    int id;

    if (size && (orig_buf[size - 1] == '\n'))
        size --;

    if (!(buf = malloc(size * sizeof(char*))))
        return;

    memcpy(buf, orig_buf, size);

    EnterCriticalSection(&libcu8_history_lock);

    if (libcu8_history_count < LIBCU8_HISTORY_SIZE) {

        /* this is the easy part, just increment the history count
           to get a free entry */
        id = libcu8_history_count ++;

    } else {

        /* No room left in the array, just shift the content up */
        free(libcu8_history[0].entry);
        memmove(libcu8_history, libcu8_history + 1,
                                    LIBCU8_HISTORY_SIZE - 1);

        id = LIBCU8_HISTORY_SIZE - 1;

    }

    libcu8_history[id].entry = buf;
    libcu8_history[id].size = size;

    LeaveCriticalSection(&libcu8_history_lock);
}

/* Move buf pointer one character backwards */
#define IS_ASCII_CHAR(c) !((c) & 0x80)  /* starts with 0 */
#define IS_FOLLOWING_BYTE(c) (((c) & 0xC0) == 0x80) /* starts with 10 */
#define IS_LEADING_2_BYTE(c) (((c) & 0xE0) == 0xC0) /* starts with 110 */
#define IS_LEADING_3_BYTE(c) (((c) & 0xF0) == 0xE0) /* starts with 1110 */
#define IS_LEADING_4_BYTE(c) (((c) & 0xF8) == 0xF0) /* starts with 11110 */
#define IS_LEADING_BYTE(c) (IS_LEADING_2_BYTE(c) || IS_LEADING_3_BYTE(c) || \
                            IS_LEADING_4_BYTE(c))
char* libcu8_previous_character(char* restrict pos, char* restrict orig_buf)
{
    if (pos == orig_buf)
        return pos;

    pos --;

    while ((pos > orig_buf)
           && IS_FOLLOWING_BYTE(*pos))
            pos --;

    return pos;
}

char* libcu8_next_character(char* restrict  pos, char* restrict buf)
{
    if (pos == buf)
        return pos;

    pos ++;

    while ((pos < buf)
           && IS_FOLLOWING_BYTE(*pos))
            pos ++;

    return pos;
}

int libcu8_count_characters(char* buf, size_t size)
{
    int ret = 0;

    while (size) {

        if (!IS_FOLLOWING_BYTE(*buf))
            ret ++;

        size --;
        buf ++;
    }

    return ret;
}

char* libcu8_completion_get_item(const char* restrict pos,
                                 const char* restrict orig,
                                 const char* restrict buf)
{
    size_t size = 0;
    char quotes = 0;
    char* ret;

    if (buf > pos
        && *pos == '"')
        quotes = 1;

    while (pos > orig) {

        if (*(pos - 1) == '"') {

            quotes = !quotes;

        } else if (!quotes &&
                   (*(pos - 1) == '\t' || *(pos - 1) == ' '
                    || *(pos - 1) == '&' || *(pos - 1) == '|')) {

            break;

        }


        pos --;
        size ++;

    }

    if (size) {

        if (*pos == '"') {
            pos ++;
            size --;
        }

        if (ret = malloc(size + 1))
            snprintf(ret, size + 1, "%s", pos);

    } else if (ret = malloc(1))
        *ret = '\0';

    return ret;
}

void libcu8_completion_insert(const char* completion,
                                char** pos, char** buf, size_t* size)
{
    size_t len = strlen(completion);

    if (*size < len)
        return; /* no room left, just ignore */

    *size -= len;

    memcpy(*pos + len, *pos, *buf - *pos);
    memcpy(*pos, completion, len);

    *pos += len;
    *buf += len;
}

/* Write buffered functions */

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
