/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#include <string.h>

#include <libpBat.h>

#include "pBat_Core.h"
#include "pBat_ShowIntro.h"
#include "../lang/pBat_Lang.h"

#if defined(WIN32) && !defined(LIBPBAT_W10_ANSI)
#include <windows.h>

void pBat_InitConsole(void)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cciCursorInfo;

	GetConsoleCursorInfo(hOut, &cciCursorInfo);
	cciCursorInfo.bVisible=TRUE;

	SetConsoleCursorInfo(hOut, &cciCursorInfo);
}

#else

void pBat_InitConsole(void) {}

#endif
