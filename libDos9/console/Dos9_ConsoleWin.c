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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

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

void Dos9_GetMousePos(char* move, CONSOLECOORD* coords, int* type)
{
    type = 0;
    coords->X = 0;
    coords->Y = 0;
}

#elif defined(WIN32)
#include <conio.h>

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

/*

 The following functions are derived from the core library of darkbox
 from Teddy Astie.

 Darkbox - A Fast and Portable Console IO Server
 Copyright (c) 2016 Teddy ASTIE (TSnake41)

 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of the name of Teddy Astie (TSnake41) nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY TEDDY ASTIE AND CONTRIBUTORS ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL TEDDY ASTIE AND CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
static char tomouse_b(DWORD m_bs, DWORD m_ef)
{
	if (m_bs & FROM_LEFT_1ST_BUTTON_PRESSED) /* left clic */
		return (m_ef & DOUBLE_CLICK) ? D_LEFT_BUTTON : LEFT_BUTTON;

	else if (m_bs & RIGHTMOST_BUTTON_PRESSED) /* right clic */
		return (m_ef & DOUBLE_CLICK) ? D_RIGHT_BUTTON : RIGHT_BUTTON;

	else if (m_bs & FROM_LEFT_2ND_BUTTON_PRESSED) /* middle clic */
		return MIDDLE_BUTTON;

	else if (m_ef & MOUSE_WHEELED) /* mouse scrolling */
		return HIWORD(m_bs) > 0 ? SCROLL_UP : SCROLL_DOWN;

	else /* mouse moved */
		return NOTHING;
}

LIBDOS9 void Dos9_GetMousePos(char on_move, CONSOLECOORD* coords, int *b)
{
	HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hin, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	DWORD e;
	INPUT_RECORD ir;

	*b = NOTHING;

	do {

		do
			ReadConsoleInput(hin, &ir, 1, &e);
		while (ir.EventType != MOUSE_EVENT);

		COORD mouse_pos = ir.Event.MouseEvent.dwMousePosition;
		coords->X = mouse_pos.X;
		coords->Y = mouse_pos.Y;

		*b = tomouse_b(ir.Event.MouseEvent.dwButtonState, ir.Event.MouseEvent.dwEventFlags);

	} while (!on_move && *b == NOTHING);
}

#endif

#ifndef WIN32

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

/*
 Dos9_Getch : Derived from darkbox getch by Teddy ASTIE

 Darkbox - A Fast and Portable Console IO Server
 Copyright (c) 2016 Teddy ASTIE (TSnake41)

*/

LIBDOS9 int Dos9_Getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(fileno(stdin), &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr(fileno(stdin), TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(fileno(stdin), TCSANOW, &oldattr);

    /* Handle special chracters */
    if (ch == '\033' && Dos9_Getch() == '[')
            switch (Dos9_Getch()) {
                case 'A': /* up arrow */
                    return 72;
                    break;
                case 'B': /* down arrow */
                    return 80;
                    break;
                case 'C': /* right arrow */
                    return 77;
                    break;
                case 'D': /* left arrow */
                    return 75;
                    break;
                case 'F': /* end */
                    return 79;
                    break;
                case 'H': /* begin */
                    return 71;
                    break;

                case '2': /* insert */
                    Dos9_Getch(); /* ignore the next character */
                    return 82;
                    break;
                case '3': /* delete */
                    Dos9_Getch();
                    return 83;
                    break;
                case '5': /* page up */
                    Dos9_Getch();
                    return 73;
                    break;
                case '6': /* page down */
                    Dos9_Getch();
                    return 81;
                    break;

                default:
                    return -1; /* unmanaged/unknown key */
                    break;
            }

    else return ch;
}


/* Morgan McGuire, morgan@cs.brown.edu */
int Dos9_Kbhit(void)
{
    static char initialized = 0;

    if (! initialized) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(fileno(stdin), &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(fileno(stdin), TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = 1;
    }

    int bytesWaiting;
    ioctl(fileno(stdin), FIONREAD, &bytesWaiting);
    return bytesWaiting;

}

#elif defined(WIN32)

int Dos9_Getch(void)
{
    return getch();
}

int Dos9_Kbhit(void)
{
    return kbhit();
}

#endif
