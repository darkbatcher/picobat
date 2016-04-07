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

#include "Dos9_File.h"

char _Dos9_Currdir[FILENAME_MAX]="";

#ifndef WIN32

#include <sys/stat.h>

int Dos9_GetExeFilename(char* lpBuf, size_t iBufSize)
{
    #ifdef DOS9_FILE_SYM_LINK

        if (readlink(DOS9_FILE_SYM_LINK, lpBuf, iBufSize)==-1)
        	return -1;

    #else

        /* we shoud add custom codes for various operating systems.
           Operating systems not currently supported by are FreeBSD,
           MacOS X, and solaris.

           Anyway, for *Nix systems it isn't really requiered */


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

        /* we shoud add custom codes for various operating systems.
           Operating systems not currently supported by are FreeBSD,
           MacOS X, and solaris.

           Anyway, for *Nix systems it isn't really requiered */


    #endif

    return 0;
}

int Dos9_DirExists(const char *ptrName)
{
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

int Dos9_UpdateCurrentDir(void)
{
    return (int)getcwd(_Dos9_Currdir, FILENAME_MAX);
}

int Dos9_SetCurrentDir(const char* lpPath)
{
    int status=chdir(lpPath);

    getcwd(_Dos9_Currdir, FILENAME_MAX);

    return status;
}

char* Dos9_GetFirstExistingFile(char** files)
{
	int i = 0;

	while (files[i] && !Dos9_FileExists(files[i]))
		i++;

	return files[i];
}
