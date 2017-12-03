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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#ifdef WIN32
#include <io.h>
#endif
#include <fcntl.h>

#include "dump.h"

#ifndef WIN32
#include <strings.h>

#define stricmp(a, b) strcasecmp(a, b)
#define strnicmp(a , b, c) strncasecmp(a, b, c)
#endif

int main(int argc, char* argv[])
{
    int i = 1,
        mode = ADDRESSES_ON | CHARS_ON | TITLE_ON | HEX_ON,
        fd,
        endianess,
        ret,
        nfiles = 0,
        nbytes = 16;

    char  buf[8192];

    char* file[FILENAME_MAX];

    struct dump_data_t data;

    setvbuf(stdout, buf, _IOFBF, sizeof(buf));

    data.type = DATA_CHAR;
    data.rev = 0;

    if (!argv[0] || !argv[1])
        dump_help();

    endianess = dump_get_endianess();
    dump_make_hfmts();


    while (argv[i]) {

        if (!stricmp(argv[i], "/B"))
            mode &= ~ (ADDRESSES_ON | CHARS_ON | TITLE_ON);
        else if (!stricmp(argv[i], "/L"))
            dump_license();
        else if (!strcmp(argv[i], "/?"))
            dump_help();
        else if (!stricmp(argv[i], "/H"))
            mode |= HEX_ON;
        else if (!stricmp(argv[i], "/-H"))
            mode &= ~ HEX_ON;
        else if (!strnicmp(argv[i], "/T", 2)) {

            argv[i] += 2;
            if (*argv[i] == ':')
                argv[i] ++;

            if (!stricmp(argv[i], "c"))
                data.type = DATA_CHAR;
            else if (!stricmp(argv[i], "s"))
                data.type = DATA_SHORT;
            else if (!stricmp(argv[i], "i"))
                data.type = DATA_INT;
            else if (!stricmp(argv[i], "l"))
                data.type = DATA_LONG;
            else if (!stricmp(argv[i], "ll"))
                data.type = DATA_LONGLONG;
            else if (!stricmp(argv[i], "f"))
                data.type = DATA_FLOAT;
            else if (!stricmp(argv[i], "d"))
                data.type = DATA_DOUBLE;
            else if (!stricmp(argv[i], "v"))
                data.type = DATA_VOID;

        } else if (!strnicmp(argv[i], "/E", 2)) {

            argv[i] += 2;
            if (*argv[i] == ':')
                argv[i] ++;

            if (!stricmp(argv[i], "b"))
                data.rev = endianess != DUMP_BIG_ENDIAN;
            else if (!stricmp(argv[i], "v"))
                data.rev = endianess != DUMP_BIG_ENDIAN;

        } else if (!strnicmp(argv[i], "/P", 2)) {

            argv[i] += 2;
            if (*argv[i] == ':')
                argv[i] ++;

            nbytes = atoi(argv[i]);

        } else {

            if (nfiles >= FILENAME_MAX)
                ERROR(3, "dump : Can't process more than %d files.\n", nfiles);

            file[nfiles ++] = argv[i];

        }

        i ++;
    }

    for (i=0; i < nfiles; i++) {

        #ifndef WIN32
        #define O_BINARY 0
        #endif // WIN32

        memset(&data, 0, sizeof(data));

        if ((fd = open(file[i], O_BINARY | O_RDONLY)) == -1)
            ERROR(1, "dump : unable to open file \"%s\".\n", file[i]);

        if (mode & TITLE_ON)
            printf("Dumping %s\n\n", argv[i]);

        ret = dump_file(fd, &data, mode, nbytes);

        close(fd);

        if (ret)
            break;

    }


    return ret;
}

char c_hfmt[10], s_hfmt[10], i_hfmt[10], l_hfmt[10], ll_hfmt[10], v_hfmt[10];

int dump_file(int fd, struct dump_data_t* data, int mode, int nbytes)
{
    int i = 0, period;
    char* addr = NULL;
    char line[81], chars[81], *pch = chars;
    size_t size = dump_item_sizeof(data);

    if (nbytes < size)
        ERROR(2, "dump : error, type too long for maximum line bytes number");

    period = nbytes / size ;

    if (mode & ADDRESSES_ON) {
        printf(v_hfmt, addr);
        printf(" ");
    }

    while (dump_read_data(fd, data) == 0) {

        dump_get_string(line, sizeof(line), data, mode);

        if (data->rev)
            dump_rev_bytes(data, size);

        if (mode & CHARS_ON)
            pch = dump_get_chars(pch, data, size);

        printf("%s", line);

        if (mode & (CHARS_ON | ADDRESSES_ON))
            printf(" ");

        addr += size;
        i ++;

        if (i == period) {

            if (mode & CHARS_ON) {
                *pch ='\0';
                printf(" %s", chars);
                pch = chars;
            }

            printf("\n");
            i = 0;

            if (mode & ADDRESSES_ON) {
                printf(v_hfmt, addr);
                printf(" ");
            }
        }
    }

    return 0;
}

void  dump_rev_bytes(struct dump_data_t* data, size_t size)
{
    char *bytes = &(data->data.c),
         tmp;
    int i = 0;

    while (i < size / 2) {
        tmp = bytes[i];
        bytes[i] = bytes[size - 1 - i];
        bytes[size - 1 - i] = tmp;
    }
}

char* dump_get_chars(char* pch, struct dump_data_t* data, size_t size)
{
    char* chars = &(data->data.c);
    int i;

    if (data->rev) {
        /* data as already been reversed, read it from the end */
        i = size - 1;
        while (i >= 0) {
            *pch = chars[i--];
            *pch = iscntrl(*pch) ? '.' : *pch;
            pch++;
        }
        return pch;

    } else {

        i = 0;
        while (i < size) {
            *pch = chars[i++];
            *pch = iscntrl(*pch) ? '.' : *pch;
            pch++;
        }
        return pch;

    }
}

void dump_get_string(char* str, size_t size, struct dump_data_t* data, int mode)
{
    switch (data->type | (mode & HEX_ON)) {

        case DATA_CHAR | HEX_ON:
            snprintf(str, size, c_hfmt, data->data.c);
            return;
        case DATA_CHAR:
            snprintf(str, size, "%d", data->data.c);
            return;

        case DATA_SHORT | HEX_ON:
            snprintf(str, size, s_hfmt, data->data.s);
            return;
        case DATA_SHORT:
            snprintf(str, size, "%d", data->data.s);
            return;

        case DATA_INT | HEX_ON:
            snprintf(str, size, i_hfmt, data->data.i);
            return;
        case DATA_INT:
            snprintf(str, size, "%d", data->data.i);
            return;

        case DATA_LONG | HEX_ON:
            snprintf(str, size, l_hfmt, data->data.l);
            return;
        case DATA_LONG:
            snprintf(str, size, "%ld", data->data.l);
            return;

        case DATA_LONGLONG | HEX_ON:
            snprintf(str, size, ll_hfmt, data->data.ll);
            return;
        case DATA_LONGLONG:
            snprintf(str, size, "%lld", data->data.ll);
            return;

        case DATA_FLOAT| HEX_ON:
        case DATA_FLOAT:
            snprintf(str, size, "%g", data->data.f);
            return;

        case DATA_DOUBLE | HEX_ON:
        case DATA_DOUBLE:
            snprintf(str, size, "%g", data->data.d);
            return;

        case DATA_VOID| HEX_ON:
        case DATA_VOID:
            snprintf(str, size, v_hfmt, data->data.v);
            return;
    }
}

void dump_make_hfmts(void)
{
    sprintf(c_hfmt, "%%0%dx", 2*sizeof(char));
    sprintf(s_hfmt, "%%0%dx", 2*sizeof(short));
    sprintf(i_hfmt, "%%0%dx", 2*sizeof(int));
    sprintf(l_hfmt, "%%0%dlx", 2*sizeof(long));
    sprintf(ll_hfmt, "%%0%dllx", 2*sizeof(long long));
    sprintf(v_hfmt, "%%0%dp", 2*sizeof(void*));
}

#define SWITCH_CASE( def, item ) \
            case def: \
                ret = read(fd, &(item), sizeof(item)); \
                ret = (ret == -1) ? ( -1 ) : ( sizeof(item) - ret); \
                break;
int dump_read_data(int fd, struct dump_data_t* data)
{
    int ret;

    switch (data->type) {
        SWITCH_CASE(DATA_CHAR, data->data.c)
        SWITCH_CASE(DATA_SHORT, data->data.s)
        SWITCH_CASE(DATA_INT, data->data.i)
        SWITCH_CASE(DATA_LONG, data->data.l)
        SWITCH_CASE(DATA_LONGLONG, data->data.ll)
        SWITCH_CASE(DATA_FLOAT, data->data.f)
        SWITCH_CASE(DATA_DOUBLE, data->data.d)
        SWITCH_CASE(DATA_VOID, data->data.v)
    }

    return ret;
}

#define SIZEOF_CASE(def, item) \
    case def: \
        return sizeof(item);
size_t dump_item_sizeof(struct dump_data_t* data)
{
    switch(data->type) {
        SIZEOF_CASE(DATA_CHAR, data->data.c)
        SIZEOF_CASE(DATA_SHORT, data->data.s)
        SIZEOF_CASE(DATA_INT, data->data.i)
        SIZEOF_CASE(DATA_LONG, data->data.l)
        SIZEOF_CASE(DATA_LONGLONG, data->data.ll)
        SIZEOF_CASE(DATA_FLOAT, data->data.f)
        SIZEOF_CASE(DATA_DOUBLE, data->data.d)
        SIZEOF_CASE(DATA_VOID, data->data.v)
    }
}

int dump_get_endianess(void)
{
    int test=1;
    char val;

    val = *((char*)&test);

    return (val) ? (DUMP_LITTLE_ENDIAN) : (DUMP_BIG_ENDIAN);
}

void dump_help(void)
{
    char* help_fr="Dump.exe [version 1.0] - copyright (c) 2013-2016 Romain Garbi\n\
Cette commande fait partie du set de commandes Dos9\n\
Ce programme est un logiciel Libre, pour plus d'information, tapez ``dump /l''\n\
\n\
\tDUMP fichier [/P] [/H] [/T[:]type] [/B] [/E:endianess] [/L]\n\
\n\
\tAffiche le contenu d'un fichier sous forme binaire.\n\n\
\t   - fichier : Le chemin du fichier dont le contenu doit etre affiche\n\n\
\t     specifie, alors la sortie est l'ecran de la console\n\n\
\t   - /H : Active le mode hexadecimal. Ne marche que pour les types\n\
\t     entiers (``int'', ``short'', ``char'')\n\n\
\t   - /P[:]nb : Choisi le nombre maximal d'octect � afficher par ligne\n\n\
\t   - /T[:]type : Choisi le type de donn�es a afficher, parmi les\n\
\t      suivants :\n\n\
\t      C : octet                   S : mot (2 octets)\n\
\t      I : double-mot (4 octets)   L : long\n\
\t      LL: double long             V : pointeur\n\
        F : nombre flotant simple   D : nombre flotant double\n\n\
\t   - /B : Desactive le titre, les en-tetes de tableau, les addresses et\n\
\t     le char dump. Utile pour cr�er des fichier pour en generer des\n\
\t     copies portables via un script\n\n\
\t   - /E:endianess : Corrige les problemes d'endianess, marche pour les\n\
\t     types d'endianness suivants :\n\n\
\t     B : gros-boutant (Big Endian)\n\
\t     L : petit-boutant (Little Endian)\n\n\
\t   - /L : Affiche un extrait de licence\n\n\
\n\
\tPar defaut, la presentation activee est equivalente a :\n\n\
\tDUMP fichier /T:C /H\n\n\
\tPar defaut, la presentation activee est equivalente a :\n\n\
\tDUMP fichier /T:C /H\n\n\
Pour plus d'information ou d'autres commandes, visitez le site internet du\n\
projet Dos9, a <http://www.dos9.org/>\n\
Page d'aide de DUMP : <http://www.dos9.org/man/dump>\n\n";

	puts(help_fr);
    exit(0);
}

void dump_license(void)
{
	char license[]="\n\n    Dos9 DUMP, a free file binary dumper, The Dos9 Project                 \n\
    Copyright (C) 2013-2016 Romain Garbi\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n";

	puts(license);
	exit(0);
}
