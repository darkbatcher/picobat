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

#ifndef DOS9_CMD_COPY_H
#define DOS9_CMD_COPY_H

int Dos9_CmdCopy(char* lpLine);
int Dos9_CmdCopyFile(const char* file, const char* dest, int* flags);
int Dos9_CmdCopyRecursive(const char* file, const char* dest, short attr, int* flags);

int Dos9_CopyFile(const char* file, const char* dest);
int Dos9_MoveFile(const char* file, const char* dest);
#define DOS9_COPY_SILENCE   0x01
#define DOS9_COPY_MULTIPLE  0x02
#define DOS9_COPY_MOVE      0x04
#define DOS9_COPY_RECURSIVE 0x08

#endif // DOS9_CMD_ECHO_H
