/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#ifndef PBAT_EXEC_H
#define PBAT_EXEC_H

#define PBAT_EXEC_WAIT 1 /* wait for the process to end */
#define PBAT_EXEC_MIN 0x2
#define PBAT_EXEC_MAX 0x4
#define PBAT_EXEC_SEPARATE_WINDOW 0x10 /* run in a separate window */

typedef struct EXECINFO {
    const char* file;
    const char* cmdline;
    const char* const* args;
    const char* dir;
    const char* title;
    int flags;
} EXECINFO;

int pBat_ExecuteFile(EXECINFO* info); /* try to execute any type of file */
int pBat_RunFile(EXECINFO* info, int* error); /* execute executables */
int pBat_StartFile(EXECINFO* info, int* error); /* Execute files or / exes*/


#endif // PBAT_EXEC_H
