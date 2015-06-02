/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2015 DarkBatcher
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


int Dos9_CmdMore(char* line)
{

    ESTR* param=Dos9_EsInit();
    int flags= DOS9_MORE_ANSICODES | DOS9_MORE_USEU8,
        tabsize=8,
        begin=0,
        status=0;

    FILELIST *list=NULL,
             *next,
             *end;

    char *ret,
         *ptr;

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

                status = -1;
                goto end;

            }

        } else if (*Dos9_EsToChar(param) == '+') {

            begin = strtol(Dos9_EsToChar(param), &ret, 0);

            if (*ret) {

                Dos9_ShowErrorMessage(DOS9_INVALID_NUMBER,
                                        Dos9_EsToChar(param),
                                        FALSE
                                      );

                status = -1;
                goto end;

            }

        } else {

            /* this is a file name */

            if (!(next = Dos9_GetMatchFileList(Dos9_EsToChar(param), 0))) {

                Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                        Dos9_EsToChar(param),
                                        FALSE
                                        );

                status = -1;
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

    if (!list) {

        Dos9_MoreFile(flags, tabsize, begin, NULL);

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

            status = -1;
            goto end;

        }

        next = list;

        while (next && !status) {

            status = Dos9_MoreFile(flags, tabsize, begin, next->lpFileName);
            next = next->lpflNext;

        }

    }

end:
    Dos9_FreeFileList(list);
    Dos9_EsFree(param);
    return status;

}

int Dos9_MoreFile(int flags, int tabsize, int begin, char* filename)
{

    FILE *file;
    char buf[80];
    int status=0,
        ok=1,
        toprint;

    if (filename) {

        if (!(file = fopen(filename, "r"))) {

            Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                    filename,
                                    FALSE
                                    );

            status = -1;

            goto end;
        }

     } else {

        file = stdin;

     }

     if (!isatty(DOS9_STDOUT)) {

        /* if we do not output in a tty, do not perform user interaction */

        while (Dos9_MoreWriteLine(&begin, flags, tabsize, file));

        goto end;

     } else {

        /* skip lines at beginning */
        while (begin && Dos9_MoreWriteLine(&begin, flags, tabsize, file));
            toprint = 23;

        /* run interractive */
        while (ok) {

            if (flags & DOS9_MORE_CLEAR) {

                Dos9_ClearConsoleScreen();
                toprint = 23;

            }

            while (toprint -- && ok)
                ok = Dos9_MoreWriteLine(&begin, flags, tabsize, file);


            if (Dos9_MorePrompt(&toprint, &begin, &ok)) {

                status = 1;
                goto end;

            }

        }

     }

end:
     if (file)
        fclose(file);

     return status;

}

int more_void(int c, FILE* p) {}

int Dos9_MoreWriteLine(int* begin, int flags, int tabsize, FILE* file)
{
    int c,
        col=0,
        ncol;

    static int blank=0,
                cr=0; /* a carriot return was included at the end of
                         the preceding line */

    int(*more_fputc)(int,FILE*)=fputc;

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
                    more_fputc(' ', stdout);

            } else {

                more_fputc('\n', stdout);

                return 1;

            }

        } else if ((flags & DOS9_MORE_ANSICODES) && c==0x1B) {

            /* if this is a csi escape code */
            more_fputc(0x1B, stdout);

            switch (c=fgetc(file)) {

                case EOF:
                    break;

                case '[':
                    more_fputc(c, stdout);

                    while (((c = fgetc(file)) != EOF) && !(c <= '~' && c >= '@'))
                        more_fputc(c, stdout);

                    more_fputc(c, stdout);

                    break;

                default:
                    more_fputc(c, stdout);
                    break;

            }

        } else if ((flags & DOS9_MORE_USEU8) && (c & 0x80)) {

            more_fputc(c, stdout);

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

                more_fputc('\n', stdout);

                return 1;

            }

            blank = 0;

            more_fputc(c, stdout);
            col ++;

        }

        cr = 0;

    }

    if (c == EOF)
        return 0;

    if (col == 79 && c != '\n') {

        more_fputc('\n', stdout);
        cr = 1;

    }

    return 1;

}

int Dos9_GetMoreNb(void)
{

    int ret=0,
        c;

    while ((c=Dos9_GetchWait()) >= '0' && c <= '9')
        ret = ret*10 + c - '0';

    return ret;

}


int Dos9_MorePrompt(int* toprint, int* skip, int* ok)
{
	int i;
    printf("-- More --");

    while (1) {

        switch(i=Dos9_GetchWait()) {

            case 'q':
            case 'Q':
                return 1;

            case 'f':
            case 'F':
                *ok = 0;
                return 0;

            case 'S':
            case 's':
                *skip = Dos9_GetMoreNb();
                *toprint=23 ; /* refresh the entire screen */
                goto end;

            case 'P':
            case 'p':
                *toprint = Dos9_GetMoreNb();
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
                Dos9_ClearConsoleLine();
                printf("-- ? : Help    Q : Quit   Sn : Skip n lines    Pn : Print n lines --");
                Dos9_GetchWait();
                Dos9_ClearConsoleLine();
                printf("-- More --");
                break;
        }

    }

end:
    Dos9_ClearConsoleLine();
    return 0;

}
