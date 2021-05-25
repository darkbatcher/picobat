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

#ifndef PBAT_CMD_DEL_H
#define PBAT_CMD_DEL_H

#define PBAT_ASK_CONFIRMATION 0x01
#define PBAT_DELETE_READONLY  0x02
#define PBAT_DONT_ASK_GENERIC 0x04
#define PBAT_USE_JOKER        0x08

int pBat_CmdDel(char* lpLine);
int pBat_CmdDelFile(char* file, int fmode, int param, int* choice);
int pBat_DelFile(const char* file);

#endif // PBAT_CMD_ECHO_H
