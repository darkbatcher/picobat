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
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "../libpBat.h"
#include "../libpBat-int.h"
#include "../../config.h"

#if defined(PBAT_USE_LIBCU8)
#include <libcu8.h>
#endif /* PBAT_USE_LIBCU8 */

#include "pBat_File.h"

#ifndef WIN32

#include <sys/stat.h>

int pBat_GetExeFilename(char* lpBuf, size_t iBufSize)
{
    #ifdef PBAT_FILE_SYM_LINK

        if (readlink(PBAT_FILE_SYM_LINK, lpBuf, iBufSize)==-1)
        	return -1;

    #else

        /* if no OS-specific symlink is available, give up with
           dynamic executable path finding and return a hard-coded
           string  */

        snprintf(lpBuf, iBufSize, BINARY_PATH "/pbat");
    #endif

    return 0;
}


int pBat_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpDelim;

    #ifdef PBAT_FILE_SYM_LINK

        if (readlink(PBAT_FILE_SYM_LINK, lpBuf, iBufSize)==-1)
        	return -1;

        if ((lpDelim=strrchr(lpBuf, '/')))
            *lpDelim='\0';

    #else
        /* if no OS-specific symlink is available, give up with
           dynamic executable path finding and return a hard-coded
           string  */

        snprintf(lpBuf, iBufSize, BINARY_PATH);

    #endif

    return 0;
}

int pBat_DirExists(const char *ptrName)
{
    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISDIR(sStat.st_mode);

}

int pBat_FileExists(const char* ptrName)
{
    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISREG(sStat.st_mode);

}

#endif

char* pBat_GetFirstExistingFile(char** files)
{
	int i = 0;

	while (files[i] && !pBat_FileExists(files[i]))
		i++;

	return files[i];
}

int pBat_SetFileMode(const char* file, int attr)
{
#if defined(PBAT_USE_LIBCU8)
    wchar_t* wfile;
    size_t cvt;

    if ((wfile = libcu8_xconvert(LIBCU8_TO_U16,
                        file, strlen(file) + 1, &cvt)) == NULL) {

        return -1;

    }

    SetFileAttributesW(wfile, attr);

    free(wfile);
#elif defined(WIN32)
    SetFileAttributes(file, attr);
#else
    chmod(file, attr);
#endif /* PBAT_USE_LIBCU8 */

    return 0;
}
