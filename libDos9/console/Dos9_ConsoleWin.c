/*
 *
 *   libDos9 - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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

#include "../libDos9.h"
#include "../../config.h"

#if defined(LIBDOS9_NO_CONSOLE)

void Dos9_ClearConsoleScreen(void)
{
}


void Dos9_SetConsoleColor(COLOR cColor)
{
}

void Dos9_SetConsoleTextColor(COLOR cColor) {
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord)
{
}

LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void)
{
    return (CONSOLECOORD){0,0};
}

LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize)
{
}

void Dos9_SetConsoleTitle(char* lpTitle)
{
}
void Dos9_ClearConsoleLine(void)
{
}

#elif defined(WIN32)

void Dos9_ClearConsoleLine(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD pt;
    DWORD written, x;

    GetConsoleScreenBufferInfo(hConsole, &info);

    pt = info.dwCursorPosition;
    pt.X = 0;

    x = info.dwCursorPosition.X;

    FillConsoleOutputCharacter(hConsole,
                                (TCHAR)' ',
                                x,
                                pt,
                                &written
                                );

    GetConsoleScreenBufferInfo(hConsole, &info);

    FillConsoleOutputAttribute(hConsole,
                                info.wAttributes,
                                x,
                                pt,
                                &written
                                );

    SetConsoleCursorPosition(hConsole, pt);
}

void Dos9_ClearConsoleScreen(void)
{
    COORD coordScreen = { 0, 0 };    /* here's where we'll home the
                                        cursor */
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
    DWORD dwConSize;                 /* number of character cells in
                                        the current buffer */

    /* get the number of character cells in the current buffer */
    GetConsoleScreenBufferInfo( hConsole, &csbi );
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    /* fill the entire screen with blanks */

    FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
       dwConSize, coordScreen, &cCharsWritten );

    /* get the current text attribute */

    GetConsoleScreenBufferInfo( hConsole, &csbi );

    /* now set the buffer's attributes accordingly */

    FillConsoleOutputAttribute( hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten );

    /* put the cursor at (0, 0) */
    SetConsoleCursorPosition( hConsole, coordScreen );
    return;
 }


void Dos9_SetConsoleColor(COLOR cColor)
{
    CONSOLE_SCREEN_BUFFER_INFO csbiScreenInfo;
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    int iSize;
    const COORD cBegin={0,0};
    GetConsoleScreenBufferInfo(hOutput, &csbiScreenInfo);
    iSize=csbiScreenInfo.dwSize.X * csbiScreenInfo.dwSize.Y;
    FillConsoleOutputAttribute(hOutput, cColor, iSize, cBegin, (PDWORD)&iSize);
    SetConsoleTextAttribute(hOutput, cColor);
}

void Dos9_SetConsoleTextColor(COLOR cColor)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOutput, cColor);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, (COORD)iCoord);
}

LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOutput, &csbi);
    return csbi.dwCursorPosition;
}

LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cciInfo;
    cciInfo.bVisible=(BOOL)bVisible;
    cciInfo.dwSize=iSize;
    SetConsoleCursorInfo(hOutput, &cciInfo);
}

void Dos9_SetConsoleTitle(char* lpTitle)
{
    SetConsoleTitle(lpTitle);
}

#endif

#ifndef WIN32

LIBDOS9 int             Dos9_GetchWait(void)
{
	char c;
	struct termios infos;
	int ret, save, min, fd;

    fflush(stdout);

	if ((fd=open("/dev/tty", O_RDONLY)) == -1)
		return 0;

	if (tcgetattr(fd, &infos))
		return 0;

	save = infos.c_lflag;
	min  = infos.c_cc[VMIN];

	infos.c_cc[VMIN] = 1;
	infos.c_lflag &= ~ECHO & ~ICANON;

	if (tcsetattr(fd, TCSANOW, &infos))
		return 0;

	ret = read(fd, &c, sizeof(c));

	infos.c_lflag = save;
	infos.c_cc[VMIN] = min;

	if (tcsetattr(fd, TCSANOW, &infos))
		return 0;

	close(fd);
	return (ret == 0) ? (0) : (c);

}

#elif defined(WIN32)

int Dos9_GetchWait(void)
{
    int c;

    while (!(c=getch()));

    return c;

}

#endif
