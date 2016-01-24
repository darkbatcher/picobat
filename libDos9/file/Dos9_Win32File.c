/*
 *
 *   libDos9 - The Dos9 project
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
#include "Dos9_File.h"

#ifdef WIN32

static char lpCurrentDir[FILENAME_MAX+3]="CD=";

int Dos9_GetExeFilename(char* lpBuf, size_t iBufSize)
{
    return GetModuleFileName(NULL, lpBuf, iBufSize);;
}

int Dos9_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpToken;
    GetModuleFileName(NULL, lpBuf, iBufSize);
    if ((lpToken=strrchr(lpBuf, '\\'))) {
        *lpToken='\0';
    } else {
        lpBuf[0]='\0';
    }
    return 0;
}


int Dos9_FileExists(const char* ptrName)
{
    int iAttrib = GetFileAttributes(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            !(iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int Dos9_DirExists(const char *ptrName)
{
    int iAttrib = GetFileAttributes(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            (iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#endif
