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

#include "iconv/iconv.h"

#ifdef  DLL_EXPORT
#define __LIBCU8__DLL
#endif /* DLL_EXPORT */

/* short circuit libcu8.h inclusion by file */
#include <libcu8.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <windows.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "config.h"

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
extern int libcu8_dummy; /* true if ansi encoding set is utf-8 */
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
    char remain[8]; /* this is for reading operations */
    size_t len;
    char remainder[4]; /* this is for writing operations */
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

struct libcu8_line_t {
    COORD orig;
    COORD current;
    COORD end;
};

#define LIBCU8_HISTORY_SIZE 50
struct libcu8_history_entry_t {
    char* entry;
    size_t size;
};

extern struct libcu8_history_entry_t
                            libcu8_history[LIBCU8_HISTORY_SIZE];
extern CRITICAL_SECTION libcu8_history_lock;
extern int libcu8_history_count;

/* Console interaction function */
int libcu8_get_console_wchar(void* handle, wchar_t* wc, int* vk);
int libcu8_get_console_input(void* handle, char* buf, size_t* size, int* vk);

int libcu8_refresh_console_line(void* handle, char* buf, size_t size,
                                    size_t cursor,
                                    struct libcu8_line_t* line,
                                    CONSOLE_SCREEN_BUFFER_INFO* csbi);
void libcu8_clear_character(void* handle, struct libcu8_line_t* line);
void libcu8_coord_decrement(COORD* coords, CONSOLE_SCREEN_BUFFER_INFO* csbi);
void libcu8_coord_increment(COORD* coords, CONSOLE_SCREEN_BUFFER_INFO* csbi);
int libcu8_count_cells(COORD orig, COORD dest,
                                    CONSOLE_SCREEN_BUFFER_INFO* csbi);
char* libcu8_previous_character(char* restrict pos, char* restrict orig_buf);
char* libcu8_next_character(char* restrict  pos, char* restrict buf);
void libcu8_history_add_entry(char* orig_buf, size_t size);
int libcu8_count_characters(char* buf, size_t size);

char* libcu8_completion_get_item(const char* restrict pos,
                                 const char* restrict orig,
                                 const char* restrict buf);
void libcu8_completion_insert(const char* completion,
                                char** pos, char** buf, size_t* size);


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

struct fn_replace_t {
    char* name;
    void* fn;
};

int libcu8_get_argv(const char*** pargv);

/*
****************************************************************************
    find.c functions

    Functions used for find replacement
****************************************************************************
 */
/* CRT functions replacement for find* familly */
#ifdef HAVE__FINDFIRST
__LIBCU8__IMP __cdecl intptr_t libcu8_findfirst(const char* file,
                                                    void* findinf);
__LIBCU8__IMP __cdecl int libcu8_findnext(intptr_t handle,
                                                void* findinf);
#endif
#ifdef HAVE__FINDFIRST64

__LIBCU8__IMP __cdecl intptr_t libcu8_findfirst64(const char* file,
                                                    void* findinf);
__LIBCU8__IMP __cdecl int libcu8_findnext64(intptr_t handle,
                                                void* findinf);
#endif
#ifdef HAVE__FINDFIRST64I32
__LIBCU8__IMP __cdecl intptr_t libcu8_findfirst64i32(const char* file,
                                                void* findinf);

__LIBCU8__IMP __cdecl int libcu8_findnext64i32(intptr_t handle,
                                                void* findinf);
#endif
#ifdef HAVE__FINDFIRST32
__LIBCU8__IMP __cdecl intptr_t libcu8_findfirst32(const char* file,
                                                    void* findinf);
__LIBCU8__IMP __cdecl int libcu8_findnext32(intptr_t handle,
                                                void* findinf);
#endif
#ifdef HAVE__FINDFIRST32I64
__LIBCU8__IMP __cdecl intptr_t libcu8_findfirst32i64(const char* file,
                                                void* findinf);

__LIBCU8__IMP __cdecl int libcu8_findnext32i64(intptr_t handle,
                                                void* findinf);
#endif

/*
****************************************************************************
    stat.c functions

    Functions used for stat.c replacement
****************************************************************************
 */

/* CRT functions for stat.h */
#ifdef HAVE__STAT
__LIBCU8__IMP __cdecl int libcu8_stat(const char* file,
                                        void* inbuf);
#endif
#ifdef HAVE__STAT32
__LIBCU8__IMP __cdecl int libcu8_stat32(const char* file,
                                            void* inbuf);
#endif
#ifdef HAVE__STAT32I64
__LIBCU8__IMP __cdecl int libcu8_stat32i64(const char* file,
                                            void* inbuf);
#endif
#ifdef HAVE__STAT64
__LIBCU8__IMP __cdecl int libcu8_stat64(const char* file,
                                            void* inbuf);
#endif
#ifdef HAVE__STAT64I32
__LIBCU8__IMP __cdecl int libcu8_stat64i32(const char* file,
                                            void* inbuf);
#endif

/*
****************************************************************************
    dup.c functions

****************************************************************************
 */
void libcu8_manage_std_files(int file, void* handle);

#endif // INTERNALS_H
