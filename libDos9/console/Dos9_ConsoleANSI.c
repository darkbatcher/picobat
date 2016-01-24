/*
 *
 *   libDos9 - The Dos9 project
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
#include <fcntl.h>

#ifndef WIN32
#include <termios.h>
#endif

#include "../libDos9.h"
#include "../../config.h"



#if !defined(WIN32) && !defined(LIBDOS9_NO_CONSOLE)

void Dos9_ClearConsoleLine(void)
{
    printf("\033[1K\033[1G");
}

void Dos9_ClearConsoleScreen(void)
{
    printf("\033[H\033[2J");
}

void Dos9_SetConsoleColor(COLOR cColor)
{
    Dos9_SetConsoleTextColor(cColor);
    printf("\033[H\033[2J");
}

void Dos9_SetConsoleTextColor(COLOR cColor)
{
    if (cColor & DOS9_FOREGROUND_INT) {

        /* set foreground intensity */
        printf("\033[1m");
        cColor^=DOS9_FOREGROUND_INT;

    } else {

        printf("\033[0m");

    }

    if (cColor & DOS9_BACKGROUND_DEFAULT)
        printf("\033[49m");

    if (cColor & DOS9_FOREGROUND_DEFAULT)
        printf("\033[39m");

    if ((cColor & DOS9_FOREGROUND_DEFAULT)
        && (cColor & DOS9_BACKGROUND_DEFAULT))
            return;

    if (!(cColor & DOS9_BACKGROUND_DEFAULT))
        printf("\033[%dm",DOS9_GET_BACKGROUND_(cColor)+40);

    if (!(cColor & DOS9_FOREGROUND_DEFAULT))
        printf("\033[%dm", DOS9_GET_FOREGROUND_(cColor)+30);

}

void Dos9_SetConsoleTitle(char* lpTitle)
{
    printf("\033]0;%s\007", lpTitle);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord)
{
    printf("\033[%d;%dH", iCoord.Y+1, iCoord.X+1);
}


LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void)
{
    return (CONSOLECOORD){0,0};
}

LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize )
{
    if (bVisible) {
        printf("\033[25h");
    } else {
        printf("\033[25l");
    }
}

#endif
