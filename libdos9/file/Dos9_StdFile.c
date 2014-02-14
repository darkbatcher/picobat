#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "Dos9_File.h"

#ifndef WIN32

static char lpCurrentDir[FILENAME_MAX+3]="CD=";

int Dos9_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpDelim;

    #ifdef __linux
        if (readlink("/proc/self/exe", lpBuf, iBufSize)==-1) return -1;
    #endif

    if ((lpDelim=strrchr(lpBuf, '/'))) {
        *lpDelim='\0';
    }

    return 0;
}

int Dos9_DirExists(char *ptrName)
{
    DIR* rep;

    if ((rep = opendir(ptrName))==NULL) return 0; /* Ouverture d'un dossier */
    closedir(rep);
    return 1;
}

int Dos9_UpdateCurrentDir(void)
{
    putenv(lpCurrentDir);
    return (int)getcwd(lpCurrentDir+3, FILENAME_MAX);
}

int Dos9_SetCurrentDir(char* lpPath)
{
    chdir(lpPath);
    return (int)getcwd(lpCurrentDir+3, FILENAME_MAX);
}
char* Dos9_GetCurrentDir(void)
{
    return lpCurrentDir+3;
}

int Dos9_FileExists(char* ptrName)
{
    FILE* pFile;
    if ((pFile=fopen(ptrName, "r"))) {
        free(pFile);
        return 1;
    }
    return 0;
}

#endif
