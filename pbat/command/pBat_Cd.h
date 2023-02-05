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

#ifndef PBAT_CMD_CD_H
#define PBAT_CMD_CD_H

#if defined(WIN32)
#define pBat_CmdCd pBat_CmdCd_win
#else
#define pBat_CmdCd pBat_CmdCd_nix
#endif

int pBat_Canonicalize(char* path);
int pBat_SetCurrentDir(char* lpLine);
int pBat_CmdCd_nix(char* lpLine);
int pBat_CmdCd_win(char* lpLine);

#endif // PBAT_CMD_ECHO_H
