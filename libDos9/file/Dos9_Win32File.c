#include "Dos9_File.h"

#ifdef WIN32

static char lpCurrentDir[FILENAME_MAX+3]="CD=";

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


int Dos9_FileExists(char* ptrName)
{
    int iAttrib = GetFileAttributes(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            !(iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int Dos9_DirExists(char *ptrName)
{
    DIR* rep = NULL;
    if ((rep = opendir(ptrName))==NULL) return 0; /* Ouverture d'un dossier */
    closedir(rep);
    return 1;
}

int Dos9_UpdateCurrentDir(void)
{
    _getcwd(lpCurrentDir+3, MAX_PATH);
    return _putenv(lpCurrentDir);
}

int Dos9_SetCurrentDir(char* lpPath)
{
    chdir(lpPath);
    _getcwd(lpCurrentDir+3, MAX_PATH);
    return _putenv(lpCurrentDir);
}
char* Dos9_GetCurrentDir(void)
{
    return lpCurrentDir+3;
}

#endif
