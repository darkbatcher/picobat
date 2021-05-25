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

#ifndef PBAT_CMD_COPY_H
#define PBAT_CMD_COPY_H

int pBat_CmdCopy(char* lpLine);
int pBat_CmdCopyFile(char* file, const char* dest, int* flags);
int pBat_CmdCopyRecursive(char* file, const char* dest, short attr, int* flags);

int pBat_CopyFile(const char* file, const char* dest);
int pBat_MoveFile(const char* file, const char* dest);
int pBat_CatFile(const char* file, const char* dest, int* flags);
#define PBAT_COPY_SILENCE   0x01
#define PBAT_COPY_MULTIPLE  0x02
#define PBAT_COPY_MOVE      0x04
#define PBAT_COPY_RECURSIVE 0x08
#define PBAT_COPY_CAT       0x10
#define PBAT_COPY_CAT_2ND   0x20

#endif // PBAT_CMD_ECHO_H
