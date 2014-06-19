#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "../libDos9-int.h"

#include "Dos9_File.h"

#ifndef WIN32

#include <sys/stat.h>

static char lpCurrentDir[FILENAME_MAX+3]="CD=";

int Dos9_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpDelim;

    #ifdef DOS9_FILE_SYM_LINK

        if (readlink(DOS9_FILE_SYM_LINK, lpBuf, iBufSize)==-1) return -1;

        if ((lpDelim=strrchr(lpBuf, '/'))) {

            *lpDelim='\0';

        }

    #else

        /* we shoud add custom codes for various operating systems.
           Operating systems not currently supported by are FreeBSD,
           MacOS X, and solaris */


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

int Dos9_UpdateCurrentDir(void)
{
    putenv(lpCurrentDir);
    return (int)getcwd(lpCurrentDir+3, FILENAME_MAX);
}

int Dos9_SetCurrentDir(const char* lpPath)
{
    chdir(lpPath);
    return (int)getcwd(lpCurrentDir+3, FILENAME_MAX);
}

char* Dos9_GetCurrentDir(void)
{
    return lpCurrentDir+3;
}

int Dos9_FileExists(const char* ptrName)
{
    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISREG(sStat.st_mode);

}

#endif
