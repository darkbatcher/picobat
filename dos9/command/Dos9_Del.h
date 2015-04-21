/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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

#ifndef DOS9_CMD_DEL_H
#define DOS9_CMD_DEL_H

#define DOS9_ASK_CONFIRMATION 0x01
#define DOS9_DELETE_READONLY  0x02
#define DOS9_DONT_ASK_GENERIC 0x04
#define DOS9_USE_JOKER        0x08

int Dos9_CmdDel(char* lpLine);
int Dos9_CmdDelFile(char* file, int param, int* choice);

#endif // DOS9_CMD_ECHO_H
