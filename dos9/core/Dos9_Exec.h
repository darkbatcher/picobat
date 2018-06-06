/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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
#ifndef DOS9_EXEC_H
#define DOS9_EXEC_H

#define DOS9_EXEC_WAIT 1 /* wait for the process to end */
#define DOS9_EXEC_MIN 0x2
#define DOS9_EXEC_MAX 0x4
#define DOS9_EXEC_SEPARATE_WINDOW 0x10 /* run in a separate window */

typedef struct EXECINFO {
    const char* file;
    const char* cmdline;
    const char* const* args;
    const char* dir;
    const char* title;
    int flags;
} EXECINFO;

int Dos9_ExecuteFile(EXECINFO* info);
int Dos9_RunFile(EXECINFO* info, int* error);
int Dos9_StartFile(EXECINFO* info, int* error);


#endif // DOS9_EXEC_H
