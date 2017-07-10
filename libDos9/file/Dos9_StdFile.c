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
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "../libDos9.h"
#include "../libDos9-int.h"
#include "../../config.h"

#if defined(DOS9_USE_LIBCU8)
#include <libcu8.h>
#endif /* DOS9_USE_LIBCU8 */

#include "Dos9_File.h"

#ifndef WIN32

#include <sys/stat.h>

int Dos9_GetExeFilename(char* lpBuf, size_t iBufSize)
{
    #ifdef DOS9_FILE_SYM_LINK

        if (readlink(DOS9_FILE_SYM_LINK, lpBuf, iBufSize)==-1)
        	return -1;

    #else

        /* if no OS-specific symlink is available, give up with
           dynamic executable path finding and return a hard-coded
           string  */

        snprintf(lpBuf, iBufSize, BINARY_PATH "/dos9");
    #endif

    return 0;
}


int Dos9_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpDelim;

    #ifdef DOS9_FILE_SYM_LINK

        if (readlink(DOS9_FILE_SYM_LINK, lpBuf, iBufSize)==-1)
        	return -1;

        if ((lpDelim=strrchr(lpBuf, '/')))
            *lpDelim='\0';

    #else
        /* if no OS-specific symlink is available, give up with
           dynamic executable path finding and return a hard-coded
           string  */

        snprintf(lpBuf, iBufSize, BINARY_PATH);

    return 0;
}

int Dos9_DirExists(const char *ptrName)
{
    #endif

    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISDIR(sStat.st_mode);

}

int Dos9_FileExists(const char* ptrName)
{
    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISREG(sStat.st_mode);

}

#endif

char* Dos9_GetFirstExistingFile(char** files)
{
	int i = 0;

	while (files[i] && !Dos9_FileExists(files[i]))
		i++;

	return files[i];
}

int Dos9_SetFileMode(const char* file, int attr)
{
#if defined(DOS9_USE_LIBCU8)
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
#endif /* DOS9_USE_LIBCU8 */

    return 0;
}
