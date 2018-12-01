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

LIBDOS9 int Dos9_Getch(FILE *f)
{
  struct termios oldattr, newattr;
  int ch;
  tcgetattr(fileno(f), &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr(fileno(f), TCSANOW, &newattr);
    ch = fgetc(f);
  tcsetattr(fileno(f), TCSANOW, &oldattr);

  /* Handle special chracters */
  if (ch == '\033' && Dos9_Getch(f) == '[') {
    switch (Dos9_Getch(f)) {
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
        Dos9_Getch(f); /* ignore the next character */
        return 82;
        break;
      case '3': /* delete */
        Dos9_Getch(f);
        return 83;
        break;
      case '5': /* page up */
        Dos9_Getch(f);
        return 73;
        break;
      case '6': /* page down */
        Dos9_Getch(f);
        return 81;
        break;

      case 'M':
        /* Mouse input beginning sequence. */
        return -2;
        break;

      default:
        return -1; /* unmanaged/unknown key */
        break;
    }
  } else return ch;
}


/* Morgan McGuire, morgan@cs.brown.edu */
int Dos9_Kbhit(FILE *f)
{
    int bytesWaiting;
    struct termios term;

    tcgetattr(fileno(f), &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(fileno(f), TCSANOW, &term);
    setbuf(f, NULL);

    ioctl(fileno(f), FIONREAD, &bytesWaiting);
    return bytesWaiting;

}

#endif


#if !defined(WIN32) && !defined(LIBDOS9_NO_CONSOLE)

void Dos9_ClearConsoleLine(FILE* f)
{
    if (!isatty(fileno(f)))
        return;

    fputs("\033[1K\033[1G", f);
}

void Dos9_ClearConsoleScreen(FILE* f)
{
    if (!isatty(fileno(f)))
        return;

    fputs("\033[H\033[2J", f);
}

void Dos9_SetConsoleColor(FILE* f, COLOR cColor)
{
    if (!isatty(fileno(f)))
        return;

    Dos9_SetConsoleTextColor(f, cColor);
    fputs("\033[H\033[2J", f);
}

void Dos9_SetConsoleTextColor(FILE* f, COLOR cColor)
{
    int fore = 30, back = 40;

    if (!isatty(fileno(f)))
        return;

    if (cColor & DOS9_FOREGROUND_INT) {

        /* set foreground intensity */
        cColor^=DOS9_FOREGROUND_INT;
        fore = 90;

    }

    if (cColor & DOS9_BACKGROUND_INT) {

        /* set foreground intensity */
        cColor^=DOS9_BACKGROUND_INT;
        back = 100;

    }

    if (cColor & DOS9_BACKGROUND_DEFAULT)
        fprintf(f, "\033[49m");

    if (cColor & DOS9_FOREGROUND_DEFAULT)
        fprintf(f, "\033[39m");

    if ((cColor & DOS9_FOREGROUND_DEFAULT)
        && (cColor & DOS9_BACKGROUND_DEFAULT))
            return;

    if (!(cColor & DOS9_BACKGROUND_DEFAULT))
        fprintf(f, "\033[%dm",DOS9_GET_BACKGROUND_(cColor)+back);

    if (!(cColor & DOS9_FOREGROUND_DEFAULT))
        fprintf(f, "\033[%dm", DOS9_GET_FOREGROUND_(cColor)+fore);
}

void Dos9_SetConsoleTitle(FILE* f, char* lpTitle)
{
    if (!isatty(fileno(f)))
        return;

    fprintf(f, "\033]0;%s\007", lpTitle);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(FILE* f, CONSOLECOORD iCoord)
{
    if (!isatty(fileno(f)))
        return;

    fprintf(f, "\033[%d;%dH", iCoord.Y+1, iCoord.X+1);
}


LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(FILE *f)
{
    return (CONSOLECOORD){0,0};
}

LIBDOS9 void Dos9_SetConsoleCursorState(FILE* f, int bVisible, int iSize )
{
    if (!isatty(fileno(f)))
        return;

    if (bVisible) {
        fputs("\033[25h", f);
    } else {
        fputs("\033[25l", f);
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

static __thread int latest;

static int tomouse_b(int b)
{
  /* Update latest if needed and return button. */
  #define return_button(button, redefine_latest) do { \
    if (redefine_latest) latest = CORE_##button; \
    return CORE_##button; \
  } while (0)

  /* The same with D_ (double click) support. */
  #define return_button_dc(button, redefine_latest) do { \
    int temp_btn = (latest == CORE_##button) ? CORE_D_##button : CORE_##button; \
    if (redefine_latest) latest = temp_btn; \
    return temp_btn; \
  } while (0)

  /* See extras/doc/xterm-mouse-tracking-analysis.ods for more informations. */
  b -= 32;

  int btn = b & 0x3;

  /* Check scrolling flag */
  if (b & (1 << 6)) {
    /* Reset latest with a neutral/non-significant value. */
    latest = CORE_NOTHING;
    /* Currently scrolling, but up or down ? */
    /* NOTE: No idea if there is another value for the btn flag. */
    return (btn == 1) ? CORE_SCROLL_DOWN : CORE_SCROLL_UP;
  }

  /* Check movement flag */
  int moving = b & (1 << 5);

  if (moving)
    /* Reset latest */
    latest = CORE_RELEASE;

  switch (btn) {
    case 0:
      return_button_dc(LEFT_BUTTON, !moving);
      break;

    case 1:
      return_button(MIDDLE_BUTTON, !moving);
      break;

    case 2:
      return_button_dc(RIGHT_BUTTON, !moving);
      break;

    case 3:
      if (moving)
        return_button(NOTHING, 0);
      else
        return_button(RELEASE, 0);
      break;
  }

  /* Should not be achieved. */
  return -1;
}

static void core_input_initialize(FILE* f, unsigned int mode)
{
    fprintf(f, "\033[?%dh", mode ? 1003 : 1000);
    latest = CORE_RELEASE;
}

static void core_input_terminate(FILE* f, unsigned int mode)
{
    fprintf(f, "\033[?%dl", mode ? 1003 : 1000);
}

LIBDOS9 void Dos9_GetMousePos(FILE* f, char on_move, CONSOLECOORD* coords, int *mouse_b)
{
    if (!isatty(fileno(f))) {
        *mouse_b = -1;
        return;
    }

	core_input_initialize(f, on_move);

    int c;
    do /* Wait CSI mouse start (-2, see posix_conio.c) */
        c = Dos9_Getch(f);
    while(c != CORE_KEY_MOUSE);

    *mouse_b = tomouse_b(Dos9_Getch(f));

    coords->X = Dos9_Getch(f) - 33;
    coords->Y = Dos9_Getch(f) - 33;

	core_input_terminate(f, on_move);
}


#endif
