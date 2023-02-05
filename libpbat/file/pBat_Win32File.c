/*
 *
 *   libpBat - The pBat project
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
#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef PBAT_USE_LIBCU8
#include <libcu8.h>
#endif

#include "pBat_File.h"

#ifdef WIN32

static char lpCurrentDir[FILENAME_MAX+3]="CD=";

int pBat_GetExeFilename(char* lpBuf, size_t iBufSize)
{
#ifdef PBAT_USE_LIBCU8
    wchar_t wname[MAX_PATH+1];
    char* str;
    size_t cnt;

    GetModuleFileNameW(NULL, wname, MAX_PATH+1);

    if ((str = libcu8_xconvert(LIBCU8_FROM_U16, wname,
                    (wcslen(wname)+1)*sizeof(wchar_t), &cnt)) == NULL) {

        return 0;

    }

    snprintf(lpBuf, iBufSize, "%s", str);
    free(str);

    return 1;
#else

    return GetModuleFileName(NULL, lpBuf, iBufSize);

#endif
}

int pBat_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpToken;
    pBat_GetExeFilename(lpBuf, iBufSize);
    if ((lpToken=strrchr(lpBuf, '\\'))) {
        *lpToken='\0';
    } else {
        lpBuf[0]='\0';
    }
    return 0;
}


int pBat_FileExists(const char* ptrName)
{
#ifdef PBAT_USE_LIBCU8
    int iAttrib;
    wchar_t* name;
    size_t conv;

    if ((name = (wchar_t*) libcu8_xconvert(LIBCU8_TO_U16, ptrName,
                strlen(ptrName)+1, &conv)) == NULL)
        return 0;

    iAttrib = GetFileAttributesW(name);
    free(name);

#else

    int iAttrib = GetFileAttributes(ptrName);

#endif
    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            !(iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int pBat_DirExists(const char *ptrName)
{
#ifdef PBAT_USE_LIBCU8
    int iAttrib;
    wchar_t* name;
    size_t conv;

    if ((name = (wchar_t*) libcu8_xconvert(LIBCU8_TO_U16, ptrName,
                strlen(ptrName)+1, &conv)) == NULL)
        return 0;

    iAttrib = GetFileAttributesW(name);
    free(name);

#else

    int iAttrib = GetFileAttributes(ptrName);

#endif

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            (iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#endif
