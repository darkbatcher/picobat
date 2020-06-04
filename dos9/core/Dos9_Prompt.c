/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI, Teddy ASTIE
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
#include <string.h>
#include <ctype.h>

#include <libDos9.h>
#include "Dos9_Core.h"

int Dos9_GetColorCode(char* lpArg);

void Dos9_OutputPromptString(const char* prompt)
{
    char *pch, *t;
    size_t count, dir_count;
    char buf[30];
    int code;

    while (pch = strchr(prompt, '$')) {

        count = pch - prompt;
        fwrite(prompt, 1, count, fOutput);

        pch ++;

        switch (tolower(*pch)) {

        case 'a':
            fputc('&', fOutput);
            break;

        case 'b':
            fputc('|', fOutput);
            break;

        case 'c':
            fputc('(', fOutput);
            break;

        case 'd':
            Dos9_GetTimeBasedVar('D', buf, sizeof(buf));
            fputs(buf, fOutput);
            break;

        case 'e':
            fputc(27, fOutput);
            break;

        case 'f':
            fputc(')', fOutput);
            break;

        case 'g':
            fputc('>', fOutput);
            break;

        case 'h':
            fputc('\b', fOutput);
            break;

        case 'l':
            fputc('<', fOutput);
            break;

        case 'm':
            /* NOTE: Not supported (UNC path name). */
            break;

        case 'n':
            fputc(lpCurrentDir[0], fOutput);
            break;

        case 'p':
            fputs(lpCurrentDir, fOutput);
            break;

        case 'q':
            fputc('=', fOutput);
            break;

        case 's':
            fputc(' ', fOutput);
            break;

        case 't':
            Dos9_GetTimeBasedVar('T', buf, sizeof(buf));
            fputs(buf, fOutput);
            break;

        case 'u':
            t = Dos9_GetEnv(lpeEnv, "USERNAME");
            if (t == NULL)
                t = "";

            fputs(t, fOutput);
            break;

        case 'v':
            /* NOTE: Not supported (version number). */
            break;

        case 'w':
            t = Dos9_GetEnv(lpeEnv, "USERDOMAIN");
            if (t == NULL)
                t = "";

            fputs(t, fOutput);
            break;

        /* set color */
        case 'x':
            pch ++;
            if (*pch && *(pch + 1) == ';') {
                buf[0] = *pch;
                buf[1] = '\0';
                pch ++;

                if ((code = Dos9_GetColorCode(buf)) != -1)
                    Dos9_SetConsoleTextColor(fOutput, code);

            } else if (*pch && *(pch + 1) && *(pch + 2) == ';') {
                buf[0] = *pch;
                buf[1] = *(pch + 1);
                buf[2] = '\0';
                pch +=2;

                if ((code = Dos9_GetColorCode(buf)) != -1)
                    Dos9_SetConsoleTextColor(fOutput, code);

            }
            break;

        case '_':
            fputs(DOS9_NL, fOutput);
            break;

        case '$':
            fputc('$', fOutput);
            break;

        case '+':
            dir_count = dsDirStack.count;
            while (dir_count --)
                fputc('+', fOutput);

            break;

        }

        if (*pch)
            pch ++;

        prompt = pch;

    }

    count = strlen(prompt);
    fwrite(prompt, 1, count, fOutput);
}


void Dos9_OutputPrompt(void)
{
    char *prompt;
    prompt = Dos9_GetEnv(lpeEnv, "PROMPT");

    if (prompt == NULL)
        prompt = "";

    Dos9_OutputPromptString(prompt);

}
