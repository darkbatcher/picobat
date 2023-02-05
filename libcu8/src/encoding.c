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

#include "iconv/iconv.h"

#include "internals.h"

#define CLOSE_IF_OPEN(cd) ( cd == (iconv_t) -1 ) ? () : ( iconv_close(cd))

char libcu8_fencoding[FILENAME_MAX]; /* the ansi encoding */
int libcu8_dummy = 0; /* true if fencoding is "UTF-8" */
CRITICAL_SECTION libcu8_fencoding_lock; /* a lock */

/* Get encoding for files */
__LIBCU8__IMP __cdecl void libcu8_get_fencoding(char* enc, size_t size)
{
    EnterCriticalSection(&libcu8_fencoding_lock);

    snprintf(enc, size, "%s", libcu8_fencoding);

    LeaveCriticalSection(&libcu8_fencoding_lock);
}

/* Set encoding for files */
__LIBCU8__IMP __cdecl int libcu8_set_fencoding(const char* enc)
{
    iconv_t test;

    EnterCriticalSection(&libcu8_fencoding_lock);

    /* Check that iconv actually support the encoding */
    if ((test = iconv_open(enc, "UTF-8")) == (iconv_t)-1) {

        LeaveCriticalSection(&libcu8_fencoding_lock);
        return -1;

    }

    iconv_close(test);

    snprintf(libcu8_fencoding, sizeof(libcu8_fencoding), "%s", enc);

    LeaveCriticalSection(&libcu8_fencoding_lock);

    return 0;
}

/* Open a iconv descriptor from mode */
iconv_t __cdecl libcu8_mode2context(int mode)
{
    iconv_t ret;

    switch (mode) {

        case LIBCU8_FROM_U16:
            ret = iconv_open("UTF-8",  "UTF-16LE");
            break;

        case LIBCU8_TO_U16:
            ret = iconv_open("UTF-16LE", "UTF-8");
            break;

        case LIBCU8_FROM_ANSI:
            EnterCriticalSection(&libcu8_fencoding_lock);
            ret = iconv_open("UTF-8", libcu8_fencoding);
            LeaveCriticalSection(&libcu8_fencoding_lock);
            break;

        case LIBCU8_TO_ANSI:
            EnterCriticalSection(&libcu8_fencoding_lock);
            ret = iconv_open(libcu8_fencoding, "UTF-8");
            LeaveCriticalSection(&libcu8_fencoding_lock);
            break;

        default:
            ret = (iconv_t) -1;
    }

    return ret;
}


#define CHUNK_SIZE 0x100
/* Cat a chunk to a buffer */
int libcu8_cat_chunk(char** buf, size_t* size, size_t* pos,
                                        char* chunk, size_t left)
{

    size_t in_chunk = CHUNK_SIZE - left, /* Used chars in chunks */
            newsize = *size + in_chunk; /* Size of the new buf to alloc */

    char* tmp;

    /* alloc the new buffer */
    if ((tmp = realloc(*buf, newsize)) == NULL) {

        return -1;
    }

    /* copy char used in chunk in the new buff */
    memcpy(tmp + *pos, chunk, in_chunk);

    /* update buffer information */
    *buf = tmp;
    *size = newsize;
    *pos += in_chunk;

    return 0;
}

/* Convert a complete byte string to another encoding. The returned
   string must be freed using *free* */
__LIBCU8__IMP __cdecl char* libcu8_xconvert(int mode, const char* src,
                                            size_t size, size_t* converted)
{
    iconv_t context;

    char chunk_buf[CHUNK_SIZE],
         *chunk = chunk_buf,
         *ret = NULL,
         *tmp;
    size_t chunk_size = CHUNK_SIZE,
            retsize = 0,
            retpos = 0,
            count = 0;
    int loop = 1;

    context = libcu8_mode2context(mode);

    if (context == (iconv_t) -1)
        return NULL;

    /* reset iconv context */
    iconv(context, NULL, NULL, &chunk, &chunk_size);

    while (loop) {

        count = iconv(context, &src, &size, &chunk, &chunk_size);

        if (count == (size_t) -1) {

            switch(errno) {

                case E2BIG:
                    /* chunck is full, copy it to ret */

                    if (libcu8_cat_chunk(&ret, &retsize, &retpos,
                               chunk_buf, chunk_size)) {

                        iconv_close(context);
                        if (ret)
                            free(ret);
                        return NULL;

                    }


                    chunk_size = CHUNK_SIZE;
                    chunk = chunk_buf;

                    break;

                case EILSEQ:
                    /* We encountered invalid byte sequence in
                       the input. Well, ignore the character */

                    /* We can do this as we are not at the end of the string */
                    size --;
                    src ++;
                    break;

                case EINVAL:
                    /* We encountered incomplete byte sequence at the end of
                       input. Just end the conversion */
                    loop = 0;

            }

        } else if (size == 0) {

            /* we converted all the buffer */
            break;

        }
    }

    /* add the last chunk */
    if (libcu8_cat_chunk(&ret, &retsize, &retpos, chunk_buf, chunk_size)) {

        iconv_close(context);
        if (ret)
            free(ret);
        return NULL;

    }

    *converted = retsize;

    iconv_close(context);

    return ret;
}

/* convert a possibly incomplete string to another encoding and return
   remaining bytes */
__LIBCU8__IMP __cdecl char* libcu8_convert(int mode, const char* src,
                                            size_t size, char* remainder,
                                            size_t* rcount, size_t rlen,
                                            size_t* converted)
{
    char chunk_buf[CHUNK_SIZE],
         *chunk = chunk_buf,
         *ret = NULL,
         *tmp = NULL;
    size_t chunk_size = CHUNK_SIZE,
            retsize = 0,
            retpos = 0,
            count;

    iconv_t context;

    if (*rcount != 0) {

        /* if we have trailing unconverted characters from a previous
           conversion, allocate a new buffer that can hold the full buffer */

        size += *rcount;

        if ((tmp = malloc (size)) == NULL)
            return NULL;

        memcpy(tmp, remainder, *rcount);
        memcpy(tmp + *rcount, src, size);
        *rcount = 0;

        src = tmp;
    }

    context = libcu8_mode2context(mode);

    /* reset iconv context */
    iconv(context, NULL, NULL, &chunk, &chunk_size);

    while (1) {

        count = iconv(context, &src, &size, &chunk, &chunk_size);

        if (count == (size_t) -1) {

            switch(errno) {

                case E2BIG:
                    /* chunck is full, copy it to ret */
                    if (libcu8_cat_chunk(&ret, &retsize, &retpos,
                                           chunk_buf, chunk_size)) {

                        iconv_close(context);
                        if (ret)
                            free(ret);
                        if (tmp)
                            free(tmp);
                        return NULL;

                    }
                    chunk_size = CHUNK_SIZE;
                    chunk = chunk_buf;
                    break;

                case EINVAL:
                    /* get the remainder and end the loop */
                    if (size > rlen) {

                        /* Ain't enough room in remainder to store trailing
                           bytes, just abort */
                        iconv_close(context);
                        if (ret)
                            free(ret);
                        if (tmp)
                            free(tmp);
                        return NULL;

                    }


                    /* saves trailing bytes */
                    memcpy(remainder, src, size);
                    *rcount = size;

                    goto next;

                case EILSEQ:
                    /* We encountered invalid byte sequence in
                       the input. Well, ignore the character */

                    /* We can do this as we are not at the end of the string */
                    size --;
                    src ++;
                    break;


            }

        } else if (size == 0) {

            /* we converted all the buffer */
            break;

        }
    }
next:

    /* add the last chunk */
    if (libcu8_cat_chunk(&ret, &retsize, &retpos, chunk_buf, chunk_size)) {

        iconv_close(context);
        if (ret)
            free(ret);
        return NULL;

    }

    *converted = retsize;

    iconv_close(context);

    return ret;

}
