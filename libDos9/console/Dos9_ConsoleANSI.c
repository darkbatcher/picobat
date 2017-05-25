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

static char tomouse_b(int mouse_char)
{
	switch (mouse_char) {
		case ' ': /* Left button */
		case '@':
			return LEFT_BUTTON;

		case '"': /* Right button */
		case 'B':
		 return RIGHT_BUTTON;

		case '!': /* Middle button */
		case 'A':
			return MIDDLE_BUTTON;

		/* TODO: Add double clics. */

		case '`': /* Scroll up */
			return SCROLL_UP;

		case 'a': /* Scroll down */
			return SCROLL_DOWN;

		case '#': /* Mouse release */
			return RELEASE;

		case 'C': /* Nothing (1003 mode only) */
			return NOTHING;

		default:
			return -1;
	}
}

static void core_input_initialize(unsigned int mode)
{
    fprintf(stderr, "\033[?%dh", mode ? 1003 : 1000);
}

static void core_input_terminate(unsigned int mode)
{
    fprintf(stderr, "\033[?%dl", mode ? 1003 : 1000);
}

LIBDOS9 void Dos9_GetMousePos(char on_move, CONSOLECOORD* coords, int *mouse_b)
{
	core_input_initialize(on_move);

    int c;
    do /* Wait CSI mouse  start (-1) */
        c = Dos9_Getch();
    while(c != -1);

    *mouse_b = tomouse_b(Dos9_Getch());

    coords->X = Dos9_Getch() - 33;
    coords->Y = Dos9_Getch() - 33;

	core_input_terminate(on_move);
}


#endif
