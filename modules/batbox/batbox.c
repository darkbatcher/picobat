/* Batbox module for Dos9
   Copyright (C) 2018 Romain GARBI

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

      1. Redistributions of source code must retain the above copyright notice,
         this list of conditions and the following disclaimer.

      2. Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.

      3. Neither the name of the copyright holder nor the names of its
         contributors may be used to endorse or promote products derived from
         this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
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
        Dos9_ShowErrorMessage(DOS9_FILE_ERROR, TTY_PATH, -1)

    Dos9_RegisterCommand("BATBOX", batbox);
}

/* usage: batbox [/a chr] [/b] [/k[_] [name] | /l[_] [name]]
                 [/w duration] [/c color] [/g X Y] [/d string]
                 [/j] [/h show]...

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
    - /g X Y : change cursor position

    - /h show : show or hide cursor.

    - /m[_] [X Y type] or /y [X Y type] : mouse

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

        } else if (fast_switch_cmp(param->str, 'g')) {

            CONSOLECOORD coord;

            if (cmd = Dos9_GetNextParameterEs(cmd, param)) {

                coord.X = args_get_number(param->str);
                if (cmd = Dos9_GetNextParameterEs(cmd, param)) {
                    coord.Y = args_get_number(param->str);
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
    return  status; /* DOS9_NO_ERROR */
}
