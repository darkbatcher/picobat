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
