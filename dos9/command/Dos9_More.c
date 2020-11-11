/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifndef WIN32
#include <termios.h>
#endif

#include <libDos9.h>

#include "../core/Dos9_Core.h"
#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_ShowHelp.h"

#include "Dos9_Commands.h"

#include "../../config.h"

static int Dos9_MoreWriteLine(int* begin, int flags, int tabsize, FILE* file);
static int Dos9_MorePrompt(FILE* fIn, int* toprint, int* skip, int* ok);

int Dos9_CmdMore(char* line)
{

    ESTR* param=Dos9_EsInit();
    FILE* fIn = NULL;
    int flags= DOS9_MORE_ANSICODES | DOS9_MORE_USEU8,
        tabsize=8,
        begin=0,
        status= DOS9_NO_ERROR;

    FILELIST *list=NULL,
             *next,
             *end;

    char *ret,
         *ptr;
    /* char name[FILENAME_MAX]; */

    line +=4;

    while ((line = Dos9_GetNextParameterEs(line, param))) {

        if (!strcmp(Dos9_EsToChar(param), "/?")) {

            Dos9_ShowInternalHelp(DOS9_HELP_MORE);
            goto end;

        } else if (!stricmp(Dos9_EsToChar(param), "/E")) {

            /* do nothing */

        } else if (!stricmp(Dos9_EsToChar(param), "/C")) {

            flags |= DOS9_MORE_CLEAR;

        } else if (!stricmp(Dos9_EsToChar(param), "/P")) {

            flags |= DOS9_MORE_FORMFEED;

        } else if (!stricmp(Dos9_EsToChar(param), "/S")) {

            flags |= DOS9_MORE_SQUEEZE;

        } else if (!stricmp(Dos9_EsToChar(param), "/U")) {

            flags &= ~ DOS9_MORE_USEU8;

        } else if (!stricmp(Dos9_EsToChar(param), "/A")) {

            flags &= ~ DOS9_MORE_ANSICODES;

        } else if (!strnicmp(Dos9_EsToChar(param), "/T", 2)) {

            ptr = Dos9_EsToChar(param) + 2;

            if (*ptr == ':')
                ptr ++;

            tabsize = strtol(ptr, &ret, 0);

            if (*ret) {

                Dos9_ShowErrorMessage(DOS9_INVALID_NUMBER,
                                        ptr,
                                        FALSE
                                      );

                status = DOS9_INVALID_NUMBER;
                goto end;

            }

        } else if (*Dos9_EsToChar(param) == '+') {

            begin = strtol(Dos9_EsToChar(param), &ret, 0);

            if (*ret) {

                Dos9_ShowErrorMessage(DOS9_INVALID_NUMBER,
                                        Dos9_EsToChar(param),
                                        FALSE
                                      );

                status = DOS9_INVALID_NUMBER;
                goto end;

            }

        } else {
            /* this is a file name */

            if (!(next = Dos9_GetMatchFileList(Dos9_EsToFullPath(param), 0))) {

                Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                        Dos9_EsToChar(param),
                                        FALSE
                                        );

                status = DOS9_NO_MATCH;
                goto end;

            }

            /* cat the lists */

            if (!list)
                list = next;
            else
                end->lpflNext = next;

            while (next->lpflNext)
                next = next->lpflNext;

            end = next;

        }

    }

    #if defined(LIBDOS9_NO_CONSOLE)
        /* If we decided not to include console facilites inside the
           command prompt, we must print whole pages since we can't
           return to the previous page */
        flags |= DOS9_MORE_CLEAR;
    #endif /* LIBDOS9_NO_CONSOLE */

    if (isatty(fileno(fOutput))) {
        flags |= DOS9_MORE_INTERACTIVE;

#ifdef WIN32
#define DOS9_MORE_INPUT_PATH "CONIN$"
#else
#define DOS9_MORE_INPUT_PATH "/dev/tty"
#endif

        /* We are running interactive so check that fInput is
           also a console input or open a file pointing to
           /dev/tty or equivalent on windows */
        if (isatty(fileno(fInput)))
            fIn = fInput;
        else if (!(fIn = fopen(DOS9_MORE_INPUT_PATH, "r"))) {

            Dos9_ShowErrorMessage(DOS9_FILE_ERROR,
                                        DOS9_MORE_INPUT_PATH, -1);

        }

    }

    if (!list) {

        Dos9_MoreFile(fIn, flags, tabsize, begin, NULL);

    } else {

        Dos9_AttributesSplitFileList(DOS9_ATTR_NO_DIR,
                                     list,
                                     &list,
                                     &end
                                     );

        Dos9_FreeFileList(end);

        if (!list) {

            Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                    "(all)",
                                    FALSE
                                    );

            status = DOS9_NO_MATCH;
            goto end;

        }

        next = list;

        while (next && !status) {

            status = Dos9_MoreFile(fIn, flags, tabsize, begin, next->lpFileName);
            next = next->lpflNext;

        }

    }

end:
    if (fIn && fIn != fInput)
        fclose(fIn);
    Dos9_FreeFileList(list);
    Dos9_EsFree(param);
    return status;

}

int Dos9_MoreFile(FILE* in, int flags, int tabsize, int begin, char* filename)
{

    FILE *file;
    /* char buf[80]; */
    int status=0,
        ok=1,
        toprint;

    if (filename) {

        if (!(file = fopen(filename, "r"))) {

            Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                    filename,
                                    FALSE
                                    );

            status = DOS9_FILE_ERROR;

            goto end;
        }

     } else {

        file = fInput;

     }

     if (!(flags & DOS9_MORE_INTERACTIVE)) {

        /* if we do not output in a tty, do not perform user interaction */
        while (Dos9_MoreWriteLine(&begin, flags, tabsize, file));

     } else {

        /* skip lines at beginning */
        while (begin && Dos9_MoreWriteLine(&begin, flags, tabsize, file))
            toprint = 23;

        /* run interractive */
        while (ok) {

            if (flags & DOS9_MORE_CLEAR) {

                Dos9_ClearConsoleScreen(fOutput);
                toprint = 23;

            }

            if (isatty(fileno(fOutput)))

            while (toprint -- && ok)
                ok = Dos9_MoreWriteLine(&begin, flags, tabsize, file);


            if (Dos9_MorePrompt(in, &toprint, &begin, &ok))
                goto end;

        }

     }

end:
     if (file != fInput)
        fclose(file);
     else if (isatty(fileno(fInput)))
        clearerr(fInput);

     return status;

}

int more_void(int c, FILE* p) { return 0; }

#ifdef WIN32

int more_fputc_u8_wrapper (int c, FILE* p)
{
    static __thread char chrs[5];
    static __thread int i = 0;

    //fprintf (p, "more_fputc_u8_wrapper %X %X\n", c & 0x40, c & 0x80);

    if ((c & 0x80) && (c & 0x40)) {

        if (i) {
            chrs[i]='\0';
            fprintf(p, "%s", chrs);
        }

        /* this is an utf leading byte */
        i=1;
        chrs[0] = c;

    } else if ((c & 0x80) && !(c & 0x40)) {

        /* this is an utf following byte */
        if (i==4) {
            chrs[i]='\0';
            fprintf(p, "%s", chrs);
            fputc(c,p);
            i=0;
        } else {
            chrs[i] = c;
            i++;
        }

    } else {

        if (i) {
            chrs[i]='\0';
            fprintf(p, "%s", chrs);
            i=0;
        }

        fputc(c, p);

    }

}
#endif

static int Dos9_MoreWriteLine(int* begin, int flags, int tabsize, FILE* file)
{
    int c,
        col=0,
        ncol;

    static __thread int blank=0,
                cr=0; /* a carriot return was included at the end of
                         the preceding line */

    int(*more_fputc)(int,FILE*)=fputc;

#ifdef WIN32
    if (flags & DOS9_MORE_USEU8)
        more_fputc = more_fputc_u8_wrapper;

#endif // WIN32

    if (*begin) {

        (*begin)--;
        more_fputc=more_void;

    }

    //printf("Printing line !\n");

    while ((col < 79) && ((c = fgetc(file)) != EOF)) {

        if (c=='\t') {

            blank = 0;

            ncol = tabsize - (col % tabsize);

            if (col + ncol < 80) {

                col += ncol;

                while (ncol --)
                    more_fputc(' ', fOutput);

            } else {

                more_fputc('\n', fOutput);

                return 1;

            }

        } else if ((flags & DOS9_MORE_ANSICODES) && c==0x1B) {

            /* if this is a csi escape code */
            more_fputc(0x1B, fOutput);

            switch (c=fgetc(file)) {

                case EOF:
                    break;

                case '[':
                    more_fputc(c, fOutput);

                    while (((c = fgetc(file)) != EOF) && !(c <= '~' && c >= '@'))
                        more_fputc(c, fOutput);

                    more_fputc(c, fOutput);

                    break;

                default:
                    more_fputc(c, fOutput);
                    break;

            }

        } else if ((flags & DOS9_MORE_USEU8) && (c & 0x80) && !(c & 0x40)) {

            /* this is a following utf-8 byte */
            more_fputc(c, fOutput);

        } else {

            if (c == '\n') {

                if (col == 0) {

                    if (blank && (flags & DOS9_MORE_SQUEEZE))
                        continue;

                    if (cr)
                        continue;

                    blank = 1;

                } else {

                    blank = 0;

                }

                more_fputc('\n', fOutput);

                return 1;

            }

            blank = 0;

            more_fputc(c, fOutput);
            col ++;

        }

        cr = 0;

    }

    if (c == EOF)
        return 0;

    if (col == 79 && c != '\n') {

        more_fputc('\n', fOutput);
        cr = 1;

    }

    return 1;

}

int Dos9_GetMoreNb(FILE *in)
{

    int ret=0,
        c;

    while ((c=Dos9_Getch(in)) >= '0' && c <= '9')
        ret = ret*10 + c - '0';

    return ret;

}


static int Dos9_MorePrompt(FILE* in, int* toprint, int* skip, int* ok)
{
	int i;
    fprintf(fOutput, "-- More --");

    while (1) {

        switch(i=Dos9_Getch(in)) {

            case 'q':
            case 'Q':
                return 1;

            case 'f':
            case 'F':
                *ok = 0;
                return 0;

            case 'S':
            case 's':
                *skip = Dos9_GetMoreNb(in);
                *toprint=23 ; /* refresh the entire screen */
                goto end;

            case 'P':
            case 'p':
                *toprint = Dos9_GetMoreNb(in);
                goto end;

			case '\r': /* Windows actually returns a '\r' (ie 0x0D) when the
						  user hits enter key, instead of returning '\n' */
            case '\n':
                *toprint = 23;
                goto end;

            case ' ':
                *toprint = 1;
                goto end;

            case '?':
                /* clean the line */
                Dos9_ClearConsoleLine(fOutput);
                fprintf(fOutput, "-- ? : Help    Q : Quit   Sn : Skip n lines    Pn : Print n lines --");
                Dos9_Getch(in);
                Dos9_ClearConsoleLine(fOutput);
                fprintf(fOutput, "-- More --");
                break;
        }

    }

end:
    Dos9_ClearConsoleLine(fOutput);
    return 0;

}
