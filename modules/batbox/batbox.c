/*
 *   Batbox module for pBat
 *   Copyright (C) 2018 Romain GARBI
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
#include <libpBat.h>
#include <pBat_Module.h>

int batbox(char* cmd);

FILE *tty = NULL;

#ifdef WIN32
#define TTY_PATH "CONIN$"
#define TTY_MODE "r"
#else
#define TTY_PATH "/dev/tty"
#define TTY_MODE "w+"
#endif // WIN32

#ifdef WIN32
#define PORTABLE_COLOR(x) x
#else
/* On windows, colors are different than on unix :

    win32           UNIX

    1 = blue        1 = red
    2 = green       2 = green
    4 = red         4 = blue

 */
#define PORTABLE_COLOR(x) (FOREGROUND_COLOR(x) | BACKGROUND_COLOR(x))
#define BACKGROUND_COLOR(x) ((((x) & 0x80) ? PBAT_BACKGROUND_INT : 0) \
                           | (((x) & 0x10) ? PBAT_BACKGROUND_BLUE : 0) \
                           | (((x) & 0x20) ? PBAT_BACKGROUND_GREEN : 0) \
                           | (((x) & 0x40) ? PBAT_BACKGROUND_RED : 0))
#define FOREGROUND_COLOR(x) ((((x) & 8) ? PBAT_FOREGROUND_INT : 0) \
                           | (((x) & 1) ? PBAT_FOREGROUND_BLUE : 0) \
                           | (((x) & 2) ? PBAT_FOREGROUND_GREEN : 0) \
                           | (((x) & 4) ? PBAT_FOREGROUND_RED : 0))
#endif

void pBat_ModuleAttach(void)
{
    if (!tty
        && (tty = fopen(TTY_PATH, TTY_MODE)) == NULL )
        pBat_ShowErrorMessage(PBAT_FILE_ERROR, TTY_PATH, -1);

    pBat_RegisterCommand("BATBOX", batbox);
}

/* usage: batbox [/a chr] [/b] [/k[_] [name] | /l[_] [name]]
                 [/w duration] [/c color] [/g X Y] [/d string]
                 [/j] [/h show] [/n]...

    - /a chr : display a character associated with the character
                code.

    - /c color : change color

    - /d string : display string

    - /k[_] : Gets a keystroke and return a code through %name% if
                specified and return it through %ERRORLEVEL% if not
                specified.

    - /l[_] : Gets a keystroke and return a character through %name% if
                specified and return it through %ERRORLEVEL% if not
                specified.

    - /g X Y : change cursor position.

    - /o X Y : Set console cursor coordinates offset.

    - /h show : show or hide cursor.

    - /n : display a new line;

    - /m[_] [X Y type] or /y [X Y type] :  Get mouse input with or without
    waiting for a click.

    - /w duration : wait for duration milliseconds if duration is a number
                    or for %duration% to be defined.

 */

#define fast_switch_cmp(str, c) \
    ((*(str) == '/') && \
        ( *(str + 1) == c || \
            ( c >= 'a' && c <= 'z' && (*(str + 1) == c + 'A'-'a'))) && \
            (*(str + 2) == '\0' || (*(str+2) == '_' && *(str+3) == '\0')))

#define fast_noswitch_check(str) \
    (str && (*(str) != '/' && *(str) != '\0'))

int args_get_number(const char* str)
{
    char *pch, *buf;

    int ret = 0;

    ret = strtol(str, &pch, 0);

    if (pch && *pch) {

        ret = 0;
        buf = pBat_GetEnv(str);

        if (buf)
            ret = strtol(buf, &pch, 0);

    }

    return ret;
}

int batbox(char* cmd)
{
    ESTR* param = pBat_EsInit();
    int status = 0;
    static __thread CONSOLECOORD orig = {0, 0}, last = {0,0};
    cmd += 6;

    /* loop through arguments */
    while (cmd && (cmd = pBat_GetNextParameterEs(cmd, param))) {

        if (fast_switch_cmp(param->str, 'a')) {

            if (cmd = pBat_GetNextParameterEs(cmd, param))
                fprintf(pBat_GetfOutput(), "%c",
                        args_get_number(param->str));


        } else if (fast_switch_cmp(param->str, 'd')) {

            if (cmd = pBat_GetNextParameterEs(cmd, param))
                fputs(param->str, pBat_GetfOutput());

        } else if (fast_switch_cmp(param->str, 'n')) {

            last.Y ++;
            pBat_SetConsoleCursorPosition(pBat_GetfOutput(), last);

        } else if (fast_switch_cmp(param->str, 'o')) {

            if (cmd = pBat_GetNextParameterEs(cmd, param)) {

                orig.X = args_get_number(param->str);
                if (cmd = pBat_GetNextParameterEs(cmd, param))
                    orig.Y = args_get_number(param->str);

            }


        } else if (fast_switch_cmp(param->str, 'g')) {

            CONSOLECOORD coord;

            if (cmd = pBat_GetNextParameterEs(cmd, param)) {

                coord.X = orig.X + args_get_number(param->str);
                if (cmd = pBat_GetNextParameterEs(cmd, param)) {
                    coord.Y = orig.Y + args_get_number(param->str);
                    last = coord;
                    pBat_SetConsoleCursorPosition(pBat_GetfOutput(), coord);
                }

            }

        } else if (fast_switch_cmp(param->str, 'h')) {

            if (cmd = pBat_GetNextParameterEs(cmd, param))
                pBat_SetConsoleCursorState(pBat_GetfOutput(),
                                           args_get_number(param->str),100);

        } else if (fast_switch_cmp(param->str, 'w')) {

            if (cmd = pBat_GetNextParameterEs(cmd, param))
                pBat_Sleep(args_get_number(param->str));

        } else if (fast_switch_cmp(param->str, 'c')) {

            if (cmd = pBat_GetNextParameterEs(cmd, param))
                pBat_SetConsoleTextColor(pBat_GetfOutput(),
                                         PORTABLE_COLOR(args_get_number(param->str)));

        } else if (fast_switch_cmp(param->str, 'k')
                   || fast_switch_cmp(param->str, 'l')) {

            int wait = 1,
                name = 0,
                key,
                type;
            char buf[30], *tmp;

            if (*(param->str) && *(param->str + 1)
                && (*(param->str + 2) == '_'))
                wait = 0;

            type = *(param->str+1);

            if ((tmp = pBat_GetNextParameterEs(cmd, param)) == NULL
                && fast_noswitch_check(param->str)) {

                name = 1;
                cmd = tmp;

            }

            if (!wait && !pBat_Kbhit(tty))
                continue;

            key = pBat_Getch(tty);

            /* If we got an extended key */
            if (key == 224)
                key += pBat_Getch(tty);

            if (name) {

                snprintf(buf, sizeof(buf), ((type == 'l' || type == 'L')
                                           && (key < 255)) ? "%c" : "%d", key);

                pBat_SetEnv(param->str, buf);

            } else {

                /* return the key and exit */
                status = key;
                goto end;

            }

        } else if (fast_switch_cmp(param->str, 'y')
                    || fast_switch_cmp(param->str, 'm')) {

            char* tmp, onmove = 0, buf[40];
            int type, names = 0;
            ESTR *y = NULL, *x = NULL;
            CONSOLECOORD coords;

            if (*(param->str) && (*(param->str + 1) == 'y'
                || (*(param->str + 1) == 'm' && *(param->str + 2) == '_')))
                onmove = 1;

            if ((tmp = pBat_GetNextParameterEs(cmd, param)) != NULL
                && fast_noswitch_check(param->str)) {

                x = param;
                param = pBat_EsInit();

                if ((tmp = pBat_GetNextParameterEs(tmp, param)) != NULL
                    && fast_noswitch_check(param->str)) {

                    y = param;
                    param = pBat_EsInit();

                    if ((tmp = pBat_GetNextParameterEs(tmp, param)) != NULL
                        && fast_noswitch_check(param->str)) {

                        cmd = tmp;
                        names = 1;

                    }

                }

            }

            pBat_GetMousePos(tty, onmove, &coords, &type);

            if (names) {

                snprintf(buf, sizeof(buf), "%d", coords.X);
                pBat_SetEnv(x->str, buf);
                snprintf(buf, sizeof(buf), "%d", coords.Y);
                pBat_SetEnv(y->str, buf);
                snprintf(buf, sizeof(buf), "%d", type);
                pBat_SetEnv(param->str, buf);

            } else {

                fprintf(pBat_GetfOutput(), "%d:%d:%d" PBAT_NL, coords.X, coords.Y, type);

            }

            if (x)
                pBat_EsFree(x);

            if (y)
                pBat_EsFree(y);

        }

    }

end:
    pBat_EsFree(param);
    return status; /* PBAT_NO_ERROR */
}
