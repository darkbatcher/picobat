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

#include "../libDos9.h"
#include "../../config.h"


#if !defined(LIBDOS9_NO_CONSOLE) && defined(WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <windows.h>

static HANDLE __inline__ _Dos9_GetHandle(FILE* f)
{
    HANDLE h = (HANDLE)_get_osfhandle(fileno(f));
    DWORD mode;

    if (GetConsoleMode(h, &mode) == 0 &&
            GetLastError() == ERROR_INVALID_HANDLE)
        return (HANDLE)-1;

    return h;
}

#ifndef LIBDOS9_W10_ANSI

void Dos9_ClearConsoleLine(FILE* f)
{
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD pt;
    DWORD written, x;

    if ((hConsole = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return;

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

void Dos9_ClearConsoleScreen(FILE* f)
{
    COORD coordScreen = { 0, 0 };    /* here's where we'll home the
                                        cursor */
    HANDLE hConsole;
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
    DWORD dwConSize;                 /* number of character cells in
                                        the current buffer */

    if ((hConsole = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return;

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


void Dos9_SetConsoleColor(FILE* f, COLOR cColor)
{
    CONSOLE_SCREEN_BUFFER_INFO csbiScreenInfo;
    HANDLE hOutput;
    int iSize;
    const COORD cBegin={0,0};

    if ((hOutput = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return;

    GetConsoleScreenBufferInfo(hOutput, &csbiScreenInfo);
    iSize=csbiScreenInfo.dwSize.X * csbiScreenInfo.dwSize.Y;
    FillConsoleOutputAttribute(hOutput, cColor, iSize, cBegin, (PDWORD)&iSize);
    SetConsoleTextAttribute(hOutput, cColor);
}

void Dos9_SetConsoleTextColor(FILE* f, COLOR cColor)
{
    HANDLE hOutput;

    if ((hOutput = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return;

    SetConsoleTextAttribute(hOutput, cColor);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(FILE* f, CONSOLECOORD iCoord)
{
    HANDLE hOutput;

    if ((hOutput = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return;

    SetConsoleCursorPosition(hOutput, (COORD)iCoord);
}

LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(FILE* f)
{
    HANDLE hOutput;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;

    if ((hOutput = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return  csbi.dwCursorPosition;

    GetConsoleScreenBufferInfo(hOutput, &csbi);
    return csbi.dwCursorPosition;
}

LIBDOS9 void Dos9_SetConsoleCursorState(FILE* f, int bVisible, int iSize)
{
    HANDLE hOutput;
    CONSOLE_CURSOR_INFO cciInfo;

    if ((hOutput = _Dos9_GetHandle(f)) == (HANDLE)-1)
        return;

    cciInfo.bVisible=(BOOL)bVisible;
    cciInfo.dwSize=iSize;
    SetConsoleCursorInfo(hOutput, &cciInfo);
}

void Dos9_SetConsoleTitle(FILE* f, char* lpTitle)
{
    SetConsoleTitle(lpTitle);
}

#endif

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
static __thread int latest = CORE_NOTHING;

static int tomouse_b(DWORD m_bs, DWORD m_ef)
{
  /* Redefine latest and return button. */
  #define return_button(button) return (latest = (button))

  if (m_bs & FROM_LEFT_1ST_BUTTON_PRESSED) /* left clic */
    return_button((m_ef & DOUBLE_CLICK)
      ? CORE_D_LEFT_BUTTON
      : CORE_LEFT_BUTTON);

  else if (m_bs & RIGHTMOST_BUTTON_PRESSED) /* right clic */
    return_button((m_ef & DOUBLE_CLICK)
      ? CORE_D_RIGHT_BUTTON
      : CORE_RIGHT_BUTTON);

  else if (m_bs & FROM_LEFT_2ND_BUTTON_PRESSED) /* middle clic */
      return_button(CORE_MIDDLE_BUTTON);

  else if (m_ef & MOUSE_WHEELED) /* mouse scrolling */
      return (int)m_bs < 0 ? CORE_SCROLL_UP : CORE_SCROLL_DOWN;

  else {
    /* mouse moved */
    if (latest != CORE_NOTHING) {
      /* Button release */
      latest = CORE_NOTHING;
      return CORE_RELEASE;
    }

    return CORE_NOTHING;
  }
}


LIBDOS9 void Dos9_GetMousePos(FILE* f, char on_move, CONSOLECOORD* coords, int *b)
{

    HANDLE hin;

    if ((hin = _Dos9_GetHandle(f)) == (HANDLE)-1) {
        *b = -1;
        return;
    }

    SetConsoleMode(hin, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	DWORD e;
	INPUT_RECORD ir;

	*b = CORE_NOTHING;

	do {

		do
			ReadConsoleInput(hin, &ir, 1, &e);
		while (ir.EventType != MOUSE_EVENT);

		COORD mouse_pos = ir.Event.MouseEvent.dwMousePosition;
		coords->X = mouse_pos.X;
		coords->Y = mouse_pos.Y;

		*b = tomouse_b(ir.Event.MouseEvent.dwButtonState, ir.Event.MouseEvent.dwEventFlags);

	} while (!on_move && *b == CORE_NOTHING);
}

#endif
