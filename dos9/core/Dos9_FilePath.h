/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef DOS9_FILEPATH_H
#define DOS9_FILEPATH_H

#ifndef WIN32

#define DOS9_PATH_DELIMITER ':'

#else

#define DOS9_PATH_DELIMITER ';'

#endif

#ifndef WIN32
#define TEST_ABSOLUTE_PATH(p) (*p == '/')
#define DEF_DELIMITER "/"
#elif defined WIN32
#define TEST_ABSOLUTE_PATH(p) ((*p && *(p+1)==':' && (*(p+2)=='\\' || *(p+2)=='/')) || (*p == '/'))
#define DEF_DELIMITER "\\"
#endif // _POSIX_C_SOURCE

char*   Dos9_EsToFullPath(ESTR* full);
int     Dos9_GetFileFullPath(char* full, const char* partial, size_t size);
void    Dos9_MakeFullPath(char* full, const char* partial, size_t size);
void    Dos9_MakeFullPathEs(ESTR* full, const char* partial);
char*   Dos9_FullPathDup(const char* path);
int     Dos9_GetFilePath(char* lpFullPath, const char* lpPartial, size_t iBufSize);
char*   Dos9_GetPathNextPart(char* lpPath, ESTR* lpReturn);
int     Dos9_MakePath(ESTR* lpReturn, int nOps, ...);
char*   Dos9_GetLastChar(ESTR* lpReturn);

#endif // DOS9_FILEPATH_H
