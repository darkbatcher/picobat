/*
 *
 *   Dos9 - The Dos9 project
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
#ifndef DOS9_CONSOLE_H
#define DOS9_CONSOLE_H

/* This console directory is a trick to accommodate the fact that standard
   stream should no more be accessed by the multithreaded interpretor. For
   this reason, this is just a re-implementation of Dos9_*Console* functions
   from libDos9 use the tls files */

#define Dos9_ClearConsoleScreen internal_Dos9_ClearConsoleScreen
#define Dos9_ClearConsoleLine internal_Dos9_ClearConsoleLine
#define Dos9_SetConsoleColor internal_Dos9_SetConsoleColor
#define Dos9_SetConsoleTextColor internal_Dos9_SetConsoleTextColor
#define Dos9_SetConsoleCursorPosition internal_Dos9_SetConsoleCursorPosition
#define Dos9_GetConsoleCursorPosition internal_Dos9_GetConsoleCursorPosition
#define Dos9_SetConsoleCursorState internal_Dos9_SetConsoleCursorState
#define Dos9_SetConsoleTitle internal_Dos9_SetConsoleTitle
#define Dos9_Kbhit internal_Dos9_Kbhit
#define Dos9_Getch internal_Dos9_Getch
#define Dos9_GetMousePos internal_Dos9_GetMousePos

void            internal_Dos9_ClearConsoleScreen(void);
void            internal_Dos9_ClearConsoleLine(void);
void            internal_Dos9_SetConsoleColor(COLOR cColor);
void            internal_Dos9_SetConsoleTextColor(COLOR cColor);
void            internal_Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord);
CONSOLECOORD    internal_Dos9_GetConsoleCursorPosition(void);
void            internal_Dos9_SetConsoleCursorState(int bVisible, int iSize);
void            internal_Dos9_SetConsoleTitle(char* lpTitle);
int             internal_Dos9_Kbhit(void);
int             internal_Dos9_Getch(void);
void            internal_Dos9_GetMousePos(char on_move, CONSOLECOORD* coords, int* type);

#endif /* DOS9_CONSOLE_H */
