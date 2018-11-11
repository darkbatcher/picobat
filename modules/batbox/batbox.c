/*
 *   Batbox module for Dos9
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
#include <libDos9.h>
#include <Dos9_Module.h>

int batbox(char* cmd);

FILE *tty = NULL;

#ifdef WIN32
#define TTY_PATH "CONIN$"
#define TTY_MODE "r"
#else
#define TTY_PATH "/dev/tty"
#define TTY_MODE "w+"
#endif // WIN32

void Dos9_ModuleAttach(void)
{
    if (!tty
        && (tty = fopen(TTY_PATH, TTY_MODE)) == NULL )
        Dos9_ShowErrorMessage(DOS9_FILE_ERROR, TTY_PATH, -1);

    Dos9_RegisterCommand("BATBOX", batbox);
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
        buf = Dos9_GetEnv(str);

        if (buf)
            ret = strtol(buf, &pch, 0);

    }

    return ret;
}

int batbox(char* cmd)
{
    ESTR* param = Dos9_EsInit();
    int status = 0;
    static __thread CONSOLECOORD orig = {0, 0}, last = {0,0};
    cmd += 6;

    /* loop through arguments */
    while (cmd && (cmd = Dos9_GetNextParameterEs(cmd, param))) {

        if (fast_switch_cmp(param->str, 'a')) {

            if (cmd = Dos9_GetNextParameterEs(cmd, param))
                fprintf(Dos9_GetfOutput(), "%c",
                        args_get_number(param->str));


        } else if (fast_switch_cmp(param->str, 'd')) {

            if (cmd = Dos9_GetNextParameterEs(cmd, param))
                fputs(param->str, Dos9_GetfOutput());

        } else if (fast_switch_cmp(param->str, 'n')) {

            last.Y ++;
            Dos9_SetConsoleCursorPosition(Dos9_GetfOutput(), last);

        } else if (fast_switch_cmp(param->str, 'o')) {

            if (cmd = Dos9_GetNextParameterEs(cmd, param)) {

                orig.X = args_get_number(param->str);
                if (cmd = Dos9_GetNextParameterEs(cmd, param))
                    orig.Y = args_get_number(param->str);

            }


        } else if (fast_switch_cmp(param->str, 'g')) {

            CONSOLECOORD coord;

            if (cmd = Dos9_GetNextParameterEs(cmd, param)) {

                coord.X = orig.X + args_get_number(param->str);
                if (cmd = Dos9_GetNextParameterEs(cmd, param)) {
                    coord.Y = orig.Y + args_get_number(param->str);
                    last = coord;
                    Dos9_SetConsoleCursorPosition(Dos9_GetfOutput(), coord);
                }

            }

        } else if (fast_switch_cmp(param->str, 'h')) {

            if (cmd = Dos9_GetNextParameterEs(cmd, param))
                Dos9_SetConsoleCursorState(Dos9_GetfOutput(),
                                           args_get_number(param->str),100);

        } else if (fast_switch_cmp(param->str, 'w')) {

            if (cmd = Dos9_GetNextParameterEs(cmd, param))
                Dos9_Sleep(args_get_number(param->str));

        } else if (fast_switch_cmp(param->str, 'c')) {

            if (cmd = Dos9_GetNextParameterEs(cmd, param))
                Dos9_SetConsoleTextColor(Dos9_GetfOutput(),
                                         args_get_number(param->str));

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

            if ((tmp = Dos9_GetNextParameterEs(cmd, param)) == NULL
                && fast_noswitch_check(param->str)) {

                name = 1;
                cmd = tmp;

            }

            if (!wait && !Dos9_Kbhit(tty))
                continue;

            key = Dos9_Getch(tty);

            /* If we got an extended key */
            if (key == 224)
                key += Dos9_Getch(tty);

            if (name) {

                snprintf(buf, sizeof(buf), ((type == 'l' || type == 'L')
                                           && (key < 255)) ? "%c" : "%d", key);

                Dos9_SetEnv(param->str, buf);

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

            if ((tmp = Dos9_GetNextParameterEs(cmd, param)) != NULL
                && fast_noswitch_check(param->str)) {

                x = param;
                param = Dos9_EsInit();

                if ((tmp = Dos9_GetNextParameterEs(tmp, param)) != NULL
                    && fast_noswitch_check(param->str)) {

                    y = param;
                    param = Dos9_EsInit();

                    if ((tmp = Dos9_GetNextParameterEs(tmp, param)) != NULL
                        && fast_noswitch_check(param->str)) {

                        cmd = tmp;
                        names = 1;

                    }

                }

            }

            Dos9_GetMousePos(tty, onmove, &coords, &type);

            if (names) {

                snprintf(buf, sizeof(buf), "%d", coords.X);
                Dos9_SetEnv(x->str, buf);
                snprintf(buf, sizeof(buf), "%d", coords.Y);
                Dos9_SetEnv(y->str, buf);
                snprintf(buf, sizeof(buf), "%d", type);
                Dos9_SetEnv(param->str, buf);

            } else {

                fprintf(Dos9_GetfOutput(), "%d:%d:%d" DOS9_NL, coords.X, coords.Y, type);

            }

            if (x)
                Dos9_EsFree(x);

            if (y)
                Dos9_EsFree(y);

        }

    }

end:
    Dos9_EsFree(param);
    return status; /* DOS9_NO_ERROR */
}
