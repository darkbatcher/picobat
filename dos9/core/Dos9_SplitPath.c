#include <string.h>
#include "Dos9_Core.h"

#ifndef WIN32

void Dos9_SplitPath(char* lpPath, char* lpDisk, char* lpDir, char* lpName, char* lpExt)
{
    char* lpNextToken, *lpToken=lpPath;
    char cSaveChar;
    strcpy(lpDisk, "");

    if ((lpNextToken = strrchr(lpPath, '/'))) {
        lpNextToken++;
        cSaveChar=*lpNextToken;
        *lpNextToken='\0';
        strncpy(lpDisk, lpPath, _MAX_DIR);
        *lpNextToken=cSaveChar;
        lpToken=lpNextToken;
        if ((lpNextToken = strrchr (lpToken, '.'))) {
            *lpNextToken='\0';
            strncpy(lpName, lpToken, _MAX_FNAME);
            lpToken=lpNextToken+1;
            strncpy(lpExt, lpToken, _MAX_EXT);
        } else {
            strncpy(lpName, lpToken, _MAX_FNAME);
            strcpy(lpExt, "");
        }

    } else {
        if ((lpNextToken = strrchr (lpToken, '.'))) {
            *lpNextToken='\0';
            strncpy(lpName, lpToken, _MAX_FNAME);
            lpToken=lpNextToken+1;
            strncpy(lpExt, lpToken, _MAX_EXT);
        } else {
            strncpy(lpName, lpToken, _MAX_FNAME);
            strcpy(lpExt, "");
        }
    }
}
#endif
