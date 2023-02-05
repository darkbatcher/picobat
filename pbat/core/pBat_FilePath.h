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

#ifndef PBAT_FILEPATH_H
#define PBAT_FILEPATH_H

#ifndef WIN32
#define PBAT_PATH_DELIMITER ':'
#else
#define PBAT_PATH_DELIMITER ';'
#endif

char*   pBat_EsToFullPath(ESTR* full);
int     pBat_GetFileFullPath(char* full, const char* partial, size_t size);
void    pBat_MakeFullPath(char* full, const char* partial, size_t size);
void    pBat_MakeFullPathEs(ESTR* full, const char* partial);
char*   pBat_FullPathDup(const char* path);
int     pBat_GetFilePath(char* lpFullPath, const char* lpPartial, size_t iBufSize);
char*   pBat_GetPathNextPart(char* lpPath, ESTR* lpReturn);
int     pBat_MakePath(ESTR* lpReturn, int nOps, ...);
char*   pBat_GetLastChar(ESTR* lpReturn);

#endif // PBAT_FILEPATH_H
