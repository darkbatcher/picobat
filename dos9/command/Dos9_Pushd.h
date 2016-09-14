/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *   Copyright (C) 2016      Teddy ASTIE
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

#ifndef DOS9_CMD_PUSHD_H
#define DOS9_CMD_PUSHD_H

#define MAX_STACK_SIZE 1024

int Dos9_CmdPushd(char *lpLine);

// Directory stack specific
void Dos9_Pushd_InitStack();
void Dos9_Pushd_PushDirectory(char*);
void Dos9_CmdPushd_PopDirectory();
char* Dos9_Pushd_GetDirectory();

#endif // DOS9_CMD_POPD_H
