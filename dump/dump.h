/*
 *
 *   Dos9 DUMP, a free file binary dumper, The Dos9 Project
 *   Copyright (C) 2010-2016 Romain GARBI
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
#ifndef DUMP_H
#define DUMP_H

#include <stdlib.h>

#define PAGE_MODE_ON 0x01
#define ADDRESSES_ON 0x02
#define CHARS_ON 0x04
#define TITLE_ON 0x08
#define HEADERS_ON 0x10
#define HEX_ON 0x20

#define DUMP_LITTLE_ENDIAN 0
#define DUMP_BIG_ENDIAN 1

union dump_data_u {
    unsigned char c;
    unsigned short s;
    int i;
    long l;
    long long ll;
    float f;
    double d;
    void* v;
};

struct dump_data_t {
    int type;
    int rev; /* reverse endianess */
    union dump_data_u data;
};

#define DATA_CHAR       0
#define DATA_SHORT      1
#define DATA_INT        2
#define DATA_LONG       3
#define DATA_LONGLONG   4
#define DATA_FLOAT      5
#define DATA_DOUBLE     6
#define DATA_VOID       7

void dump_license(void);
void dump_help(void);

int dump_file(int fd, struct dump_data_t* data, int mode, int nbytes);
void dump_get_string(char* str, size_t size, struct dump_data_t* data, int mode);

size_t dump_item_sizeof(struct dump_data_t* data);
int dump_read_data(int fd, struct dump_data_t* data);

char* dump_get_chars(char* pch, struct dump_data_t* data, size_t size);
void  dump_rev_bytes(struct dump_data_t* data, size_t size);
void dump_make_hfmts(void);

int dump_get_endianess(void);

#define ERROR(code, ...) \
    do {\
        fprintf(stderr, ##__VA_ARGS__ );\
        exit(code); \
    } while (0)

#endif // DUMP_H
