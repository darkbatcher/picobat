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
#ifndef INTERNALS_H
#define INTERNALS_H

#include <iconv.h>

#ifdef  DLL_EXPORT
#define __LIBCU8_DLL
#endif /* DLL_EXPORT */

/* short circuit libcu8.h inclusion by file */
#include <libcu8.h>

/*
*******************************************************************************
    crt internal functions

    Define some msvcrt internal function to be able to alter msvcrt's private
    datas.
*******************************************************************************
*/


/* import the __pioinfo symbol */
struct ioinfo {
    void* osfhnd;
    unsigned char osfile;
    unsigned char pipech;
    int lockinitflag;
    CRITICAL_SECTION lock;
};

extern _CRTIMP struct ioinfo* __pioinfo[];

/* define some macros to deal with __pioinfo buffer (which stores
   data file information */
#define IOINFO_TABLE_SIZE (1 << 5)
#define IOINFO_MAX_TABLE 64
#define MAKE_FD(table,index)  ( (table) << 5 + (index))

#define pioinfo(i)  (__pioinfo[((i) >> 5)] + \
                                    ((i) & (IOINFO_TABLE_SIZE - 1)))
#define osfhnd(i)   (pioinfo(i)->osfhnd)
#define osfile(i)   (pioinfo(i)->osfile)
#define pipech(i)   (pioinfo(i)->pipech)

#define FHND        0x01 /* have a file handle */
#define ATEOF       0x02 /* descriptor at EOF */
#define PIPE        0x08 /* is a pipe */
#define TTY         0x40 /* is a tty */
#define TEXTMODE    0x80 /* use text mode */
#define NOINHERIT   0x10 /* not inheritable file */
#define APPEND      0x20 /* append at end of file */

#define IS_VALID(i)  (((i) >> 5 ) < IOINFO_MAX_TABLE && \
                     __pioinfo[(i) >> 5] != NULL && \
                     ( osfile(i) & FHND))


/* Some macros to make simple test */
#define IS_TEXTMODE(mode)   (mode & TEXTMODE)
#define IS_PIPE(mode)       (mode & PIPE)
#define HAS_FHND(mode)      (mode & FHND)
#define IS_TTY(mode)        (mode & TTY)
#define IS_ATEOF(mode)      (mode & ATEOF)

/*
*******************************************************************************
    inject.c functions

    Functions to inject new functions in place of dll functions or so.
*******************************************************************************
*/

/* A function to inject new enhanced functions */
int libcu8_replace_fn(void* oldfn, void* newfn, int n);
int libcu8_reserve_fn_table(int nb, void* base_fn);
void** libcu8_get_fn_pointer(void* fn);
void __cdecl libcu8_save_changes(void);

/*
*******************************************************************************
    encoding.c functions

    Functions to assist interaction with the console and other devices. Also
    contains functions to deal with fd buffering.
*******************************************************************************
*/



/* internal structure */
extern char libcu8_fencoding[FILENAME_MAX]; /* the ansi encoding */
extern CRITICAL_SECTION libcu8_fencoding_lock; /* a lock */

/* encoding funcs */
iconv_t libcu8_mode2context(int mode);
int libcu8_cat_chunk(char** buf, size_t* size, size_t* pos,
                                        char* chunk, size_t left);


/*
*******************************************************************************
    read.c functions

    Functions to assist interaction with the console and other devices. Also
    contains functions to deal with fd buffering.
*******************************************************************************
*/


/* A structure to buffer fd transmission. This is terribly harmful but
   we have to do this to ensure good text conversion. */
struct fd_buffering_t {
    char remain[4];
    size_t len;
    char remainder[4];
    size_t rcount;
};

/* extension to __ioinfo struct */
#define FD_BUFFERS_SIZE 0x800
extern struct fd_buffering_t* libcu8_fd_buffers;
extern void*  conout;

/* Buffering management functions */
void libcu8_write_buffered(int fd, char** buf, size_t* size,
                                            char* utf8, size_t len);
void libcu8_read_buffered(int fd, char** buf, size_t* size);
#define libcu8_cp_buffering(fd1, fd2) \
            memcpy(&libcu8_fd_buffers[(fd1)], &libcu8_fd_buffers[(fd2)],\
                   sizeof(struct fd_buffering_t))
#define libcu8_reset_buffered(fd) \
            memset(&libcu8_fd_buffers[(fd)], 0, sizeof(struct fd_buffering_t))

/* Test to check a handle is a console handle */
int libcu8_is_tty(void* hndl);

/* Custom readconsole-like function */
int libcu8_readconsole(int fd, char* buf, size_t size, size_t* written);
int libcu8_get_file_byte(void* handle, char* buf, size_t* sansi);
int libcu8_try_convert(iconv_t context, char* in, size_t* insize,
                                                char* utf8, size_t* outsize);

/* custom readfile-like function */
int libcu8_readfile(int fd, char* buf, size_t size, size_t* written);

/* character removal from u8 character buffer */
int libcu8_delete_character(char** buf, size_t* size, const size_t orig);

/* Console interaction function */
int libcu8_get_console_wchar(void* handle, wchar_t* wc);
int libcu8_get_console_input(void* handle, char* buf, size_t* size);
void libcu8_delete_console_character(void* handle);
void libcu8_delete_console_wchar(void* handle, char type);

/*
****************************************************************************
    write.c functions

    Functions to assist writing to files through the low level API (which
    is also used by standard file API)
****************************************************************************
 */

int libcu8_write_console(int fd, void* buf, size_t cnt, size_t* written);
int libcu8_write_file(int fd, void* buf, size_t cnt, size_t* written);
char* libcu8_lf_to_crlf(const char* buf, size_t len, size_t* cvt);



/*
****************************************************************************
    init.c functions

    Functions used to get utf8 argv.
****************************************************************************
 */
int libcu8_get_argv(const char*** pargv);

#endif // INTERNALS_H
