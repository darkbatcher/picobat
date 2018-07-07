/*
 *
 *   Dos9 DUMP, a free file binary dumper, The Dos9 Project
 *   Copyright (C) 2013-2018 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


/* #include <libcu8.h> */

#ifndef WIN32
#define stricmp(a, b) strcasecmp(a, b)
#define strnicmp(a , b, c) strncasecmp(a, b, c)
#define O_BINARY 0
#define set_bin_mode(f)
#else
#define set_bin_mode(f) _setmode(f, O_BINARY)
#endif

#define DUMP_CHAR 0
#define DUMP_UCHAR 1
#define DUMP_SHORT 2
#define DUMP_USHORT 3
#define DUMP_INT 4
#define DUMP_UINT 5
#define DUMP_LONGLONG 5
#define DUMP_ULONGLONG 6
#define DUMP_FLOAT 7
#define DUMP_DOUBLE 8

#define DUMP_ADDRESS 0x1
#define DUMP_TITLE 0x2
#define DUMP_CHARS 0x4
#define DUMP_QUIET 0x8
#define DUMP_HEX 0x10
#define DUMP_DEFAULT (DUMP_ADDRESS | DUMP_CHARS | DUMP_TITLE)

#define FORMAT_SIZE 10

#define DEFAULT_WIDTH 80

#define GET_CAST_9(type, p, fn, ...) (type == DUMP_DOUBLE) ? (fn ( __VA_ARGS__ , *((double*)p ))) : \
                                            (0)
#define GET_CAST_8(type, p, fn, ...) (type == DUMP_FLOAT) ? (fn ( __VA_ARGS__ , *((float*)p ))) : \
                                            (GET_CAST_9(type, p, fn, __VA_ARGS__))
#define GET_CAST_7(type, p, fn, ...) (type == DUMP_ULONGLONG) ? (fn ( __VA_ARGS__ , *((unsigned long long*)p ))) : \
                                            (GET_CAST_8(type, p, fn, __VA_ARGS__))
#define GET_CAST_6(type, p, fn, ...) (type == DUMP_LONGLONG) ? (fn ( __VA_ARGS__ , *((long long*)p ))) : \
                                            (GET_CAST_7(type, p, fn, __VA_ARGS__))
#define GET_CAST_5(type, p, fn, ...) (type == DUMP_UINT) ? (fn ( __VA_ARGS__ , *((unsigned int*)p ))) : \
                                            (GET_CAST_6(type, p, fn, __VA_ARGS__))
#define GET_CAST_4(type, p, fn, ...) (type == DUMP_INT) ? (fn ( __VA_ARGS__ , *((int*)p ))) : \
                                            (GET_CAST_5(type, p, fn, __VA_ARGS__))
#define GET_CAST_3(type, p, fn, ...) (type == DUMP_USHORT) ? (fn ( __VA_ARGS__ , *((unsigned short*)p ))) : \
                                            (GET_CAST_4(type, p, fn, __VA_ARGS__))
#define GET_CAST_2(type, p, fn, ...) (type == DUMP_SHORT) ? (fn ( __VA_ARGS__ , *((short*)p ))) : \
                                            (GET_CAST_3(type, p, fn, __VA_ARGS__))
#define GET_CAST_1(type, p, fn, ...) (type == DUMP_UCHAR) ? (fn ( __VA_ARGS__ , *((unsigned char*)p))) : \
                                            (GET_CAST_2(type, p, fn, __VA_ARGS__))
#define GET_CAST_0(type, p, fn, ...) (type == DUMP_CHAR) ? (fn ( __VA_ARGS__ , *((char*)p))) : \
                                            (GET_CAST_1(type, p, fn, __VA_ARGS__))
#define GET_CAST(type, p, fn, ...) GET_CAST_0(type, p, fn, __VA_ARGS__)

struct dump_t {
    char flags;
    size_t size;
    int type;
    unsigned int n;
    unsigned int width;
    char format[FORMAT_SIZE];
};

struct dump_types_t {
    const char* name;
    int size;
    const char* max;
    const char* min;
    const char* hexfmt;
    const char* decfmt;
};


#include "dump.h"
#include "../config.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

#ifndef DBL_DECIMAL_DIG
#define DBL_DECIMAL_DIG 17
#endif // DBL_DECIMAL_DIG

#ifndef FLT_DECIMAL_DIG
#define FLT_DECIMAL_DIG 9
#endif // DBL_DECIMAL_DIG

struct dump_types_t types[] = {
    {"c", sizeof(char), DUMP_CHAR_MAX, DUMP_CHAR_MIN, "x", "d"},
    {"uc", sizeof(char), DUMP_UCHAR_MAX, DUMP_UCHAR_MIN, "x",  "u"},
    {"s", sizeof(short), DUMP_SHRT_MAX, DUMP_SHRT_MIN, "x", "d"},
    {"us", sizeof(short), DUMP_USHRT_MAX, DUMP_USHRT_MIN, "x", "u"},
    {"i", sizeof(int), DUMP_INT_MAX, DUMP_INT_MIN, "x", "d"},
    {"ui", sizeof(int), DUMP_UINT_MAX, DUMP_UINT_MIN, "x", "u"},
    {"ll", sizeof(long long), DUMP_LLONG_MAX, DUMP_LLONG_MIN, "llx", "lld"},
    {"ull", sizeof(long long), DUMP_LLONG_MAX, DUMP_LLONG_MIN, "llx", "llu"},
    {"f", sizeof(float), DUMP_FLT_MAX, DUMP_FLT_MIN, "x","." STR(FLT_DECIMAL_DIG) "g"},
    {"d", sizeof(double), DUMP_DBL_MAX, DUMP_DBL_MIN, "llx", "." STR(DBL_DECIMAL_DIG) "g"},
    {NULL, 0, NULL, NULL, NULL, NULL},
};

void help(void)
{
    puts("Dos9 dump [2.0] (" HOST ") - " __DATE__ "\n"
            "Copyright (c) 2013-2018 Romain Garbi\n");

    puts("Usage: dump [/H] [/B[:]format] [/T[:]type] [files ...]\n"
         "Dump content of files.\n\n"
         "\t- files ... : A list of files to be dumped.\n"
         "\t- /h : Hexadecimal dump.\n"
         "\t- /b:format : Set display format. Combination of:\n"
         "\t\t- A : display addresses.\n"
         "\t\t- T : display title.\n"
         "\t\t- C : display chars.\n"
         "\t\t- Q : display no spaces\n"
         "\t- /T[:]type : Set dumped elements types. One of:\n"
         "\t\t- c : char\n"
         "\t\t- uc : unsigned char\n"
         "\t\t- s : short\n"
         "\t\t- us : unsigned short\n"
         "\t\t- i : int\n"
         "\t\t- ui : unsigned int\n"
         "\t\t- ll : long long\n"
         "\t\t- ull : unsigned long long\n"
         "\t\t- f : float\n"
         "\t\t- d : double"
         );
}

int get_max(int i, int j)
{
    return (j > i) ? j : i;
}

void fill_context(int type, int flags, struct dump_t* context)
{
    unsigned int size, s, a, c;
    char* fmt;

    if (flags & DUMP_HEX) {
        size = types[type].size * 2;
    } else {
        size = get_max(strlen(types[type].min),
                               strlen(types[type].max));
    }

    snprintf(context->format, FORMAT_SIZE,
                "%%0%d%s", size, (flags & DUMP_HEX) ?
                                  (types[type].hexfmt) : (types[type].decfmt));

    context->size = types[type].size;

    s = (flags & DUMP_QUIET) ? 0 : 1 ;
    a = (flags & DUMP_ADDRESS) ? 2*sizeof(void*) + s : 0;
    c = (flags & DUMP_CHARS) ? types[type].size : 0;
    context->n = (DEFAULT_WIDTH - a)
                                / (size + s + c);
    context->width = size;
    context->flags = flags;
    context->type = type;
}

int get_type(const char* type)
{
    int i=0;

    while (types[i].name) {

        if (!stricmp(type, types[i].name))
            return i;

        i++;

    }

    return -1;
}

int dump_fd(int fd, struct dump_t* context)
{
    char *chars, *data, *p, *addr = NULL, *spaces;
    int count, i;
    size_t size = context->n * context->size;

    if (!(chars = malloc(size + 1))
        || !(data = malloc(size))) {

        fprintf(stderr, "dump: not enough memory.\n");
        return -1;

    }

    spaces = (context->flags & DUMP_QUIET) ? "" : " ";

    while (count = read(fd, data, size)) {

        if (count < size) {
             /* we reached eof and count is not a multiple of the
                size we set just pad with 0 until the next
                multiple ... */

            memset(data + count, 0, size - count);

            if (count % context->size)
                count += context->size - (count % context->size);

        }

        p = data;

        if (context->flags & DUMP_ADDRESS)
            printf("%p%s", addr, spaces);

        /* loop over elements of the buffer */
        while (p < data + count) {

            GET_CAST(context->type, p, printf, context->format);
            printf("%s", spaces);
            p += context->size;

        }

        /* Add some spaces to the output in order to addresses vertically
           aligned */
        if (context->flags & DUMP_CHARS) {
            while (p < data + size) {

                for (i=0; i < context->width; i ++)
                    printf(" ");

                printf("%s", spaces);

                p += context->size;
            }
        }

        if (context->flags & DUMP_CHARS) {

            memcpy(chars, data, count);
            *(chars + count) = '\0';

            p = chars;

            while (p < chars + size) {

                if (*p < 0x20)
                    *p = '.';

                p ++;

            }

            printf("%s", chars);

        }

        printf("\n");

        addr += size;
    }

    free(chars);
    free(data);

    return 0;

}

int dump_file(const char* file, struct dump_t* context)
{
    int fd, ret;

    if ((fd = open(file, O_RDONLY | O_BINARY)) == -1) {

        fprintf(stderr, "dump: unable to open %s.\n", file);
        return -1;

    }

    if (context->flags & DUMP_TITLE)
        printf("%s -----------------\n", file);

    ret = dump_fd(fd, context);
    close(fd);

    return ret;
}

int main(int argc, char** argv)
{
    char* p;
    int type = 0, flags = DUMP_DEFAULT,
        n = 0;

    struct dump_t context;

    if (*argv)
        argv ++;

    while(*argv) {

        if (!strnicmp(*argv, "/T", 2)) {
            *argv += 2;

            if (*(*argv) == ':')
                (*argv) ++;

            if ((type = get_type(*argv)) == -1) {

                fprintf(stderr, "dump: %s is not a valid type.\n", *argv);
                exit(1);

            }


        } else if (!stricmp(*argv, "/H")) {

            flags |= DUMP_HEX;

        } else if (!strnicmp(*argv, "/B", 2)) {

            *argv += 2;

            if (*(*argv) == ':')
                (*argv) ++;

            flags &= ~ DUMP_DEFAULT;

            while (*argv) {
                switch (**argv) {

                case 'a':
                case 'A':
                    flags |= DUMP_ADDRESS;
                    break;

                case 'c':
                case 'C':
                    flags |= DUMP_CHARS;
                    break;

                case 'T':
                case 't':
                    flags |= DUMP_TITLE;
                    break;

                case 'Q':
                case 'q':
                    flags |= DUMP_QUIET;

                }

                (*argv) ++;
            }

        } else if (!stricmp(*argv, "/?")) {

            help();
            exit(0);

        } else {

            fill_context(type, flags, &context);

            if (dump_file(*argv, &context) == -1)
                exit(2);

            n ++;

        }

        argv ++;

    }

    if (!n) {
        fill_context(type, flags, &context);
        set_bin_mode(STDIN_FILENO);
        if ((dump_fd(STDIN_FILENO, &context) == -1))
            exit(2);
    }

    return 0;
}
