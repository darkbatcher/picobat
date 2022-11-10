/*
 *
 *   libpBat - The pBat project
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

 #if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif

#include "../libpBat.h"
#include "../../config.h"

#if !defined(LIBPBAT_NO_CONSOLE) && (defined(LIBPBAT_W10_ANSI) || !defined(WIN32))

#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef LIBPBAT_W10_ANSI
#include <conio.h>

#define isatty(fd) _isatty(fd)
#endif

void pBat_ClearConsoleLine(FILE* f)
{
    if (!isatty(fileno(f)))
        return;

    fputs("\033[1K\033[1G", f);
}

void pBat_ClearConsoleScreen(FILE* f)
{
    if (!isatty(fileno(f)))
        return;

    fputs("\033[H\033[2J", f);
}

void pBat_SetConsoleColor(FILE* f, COLOR cColor)
{
    if (!isatty(fileno(f)))
        return;

    pBat_SetConsoleTextColor(f, cColor);
    fputs("\033[H\033[2J", f);
}

void pBat_SetConsoleTextColor(FILE* f, COLOR cColor)
{
    int fore = 30, back = 40;

    if (!isatty(fileno(f)))
        return;

    if (cColor & PBAT_FOREGROUND_INT) {

        /* set foreground intensity */
        cColor^=PBAT_FOREGROUND_INT;
        fore = 90;

    }

    if (cColor & PBAT_BACKGROUND_INT) {

        /* set foreground intensity */
        cColor^=PBAT_BACKGROUND_INT;
        back = 100;

    }

    if (cColor & PBAT_BACKGROUND_DEFAULT)
        fprintf(f, "\033[49m");

    if (cColor & PBAT_FOREGROUND_DEFAULT)
        fprintf(f, "\033[39m");

    if ((cColor & PBAT_FOREGROUND_DEFAULT)
        && (cColor & PBAT_BACKGROUND_DEFAULT))
            return;

    if (!(cColor & PBAT_BACKGROUND_DEFAULT))
        fprintf(f, "\033[%dm",PBAT_GET_BACKGROUND_(cColor)+back);

    if (!(cColor & PBAT_FOREGROUND_DEFAULT))
        fprintf(f, "\033[%dm", PBAT_GET_FOREGROUND_(cColor)+fore);
}

void pBat_SetConsoleTitle(FILE* f, char* lpTitle)
{
    if (!isatty(fileno(f)))
        return;

    fprintf(f, "\033]0;%s\007", lpTitle);
}

LIBPBAT void pBat_SetConsoleCursorPosition(FILE* f, CONSOLECOORD iCoord)
{
    if (!isatty(fileno(f)))
        return;

    fprintf(f, "\033[%d;%dH", iCoord.Y+1, iCoord.X+1);
}


LIBPBAT CONSOLECOORD pBat_GetConsoleCursorPosition(FILE *f)
{
    return (CONSOLECOORD){0,0};
}

LIBPBAT void pBat_SetConsoleCursorState(FILE* f, int bVisible, int iSize )
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

#ifndef LIBPBAT_W10_ANSI

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

LIBPBAT void pBat_GetMousePos(FILE* f, char on_move, CONSOLECOORD* coords, int *mouse_b)
{
    if (!isatty(fileno(f))) {
        *mouse_b = -1;
        return;
    }

	core_input_initialize(f, on_move);

    int c;
    do /* Wait CSI mouse start (-2, see posix_conio.c) */
        c = pBat_Getch(f);
    while(c != CORE_KEY_MOUSE);

    *mouse_b = tomouse_b(pBat_Getch(f));

    coords->X = pBat_Getch(f) - 33;
    coords->Y = pBat_Getch(f) - 33;

	core_input_terminate(f, on_move);
}

#endif

#endif
