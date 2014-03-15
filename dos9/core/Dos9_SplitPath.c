#include <string.h>
#include "Dos9_Core.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

void Dos9_SplitPath(char* lpPath,
                    char* lpDisk, char* lpDir, char* lpName, char* lpExt)
{

    char* lpNextToken, *lpToken=lpPath;
    char cSaveChar;

    if (*lpToken) {

        if (!strncmp(lpToken+1, ":\\", 2)) {

            DOS9_DBG("Found disk_name=\"%c\"\n", *lpToken);

            if (lpDisk)
                snprintf(lpDisk, _MAX_DRIVE, "%c:\\", *lpToken);

            lpToken+=3;

        }

    } else {

        if (lpDisk)
            *lpDisk='\0';

    }

    if ((lpNextToken = Dos9_SearchLastToken(lpToken, "\\/"))) {

        lpNextToken++;
        cSaveChar=*lpNextToken;
        *lpNextToken='\0';

        DOS9_DBG("found path=\"%s\"\n", lpToken);

        if (lpDir)
            snprintf(lpDir, _MAX_DIR, "%s/", lpToken);

        *lpNextToken=cSaveChar;
        lpToken=lpNextToken;

    } else {

        if (lpDir)
            *lpDir=='\0';

    }

    if ((lpNextToken = Dos9_SearchLastChar(lpToken, '.'))) {

        cSaveChar=*lpNextToken;
        *lpNextToken='\0';

        DOS9_DBG("found name=\"%s\"\n", lpToken);


        if (lpName)
            strncpy(lpName, lpToken, _MAX_FNAME);

        *lpNextToken=cSaveChar;
        lpToken=lpNextToken+1;

        DOS9_DBG("found ext=\"%s\"\n", lpToken);

        if (lpExt)
            snprintf(lpExt, _MAX_EXT, ".%s", lpToken);

    } else {

        if (lpName)
            strncpy(lpName, lpToken, _MAX_FNAME);

        if (lpExt)
            *lpExt='\0';

    }


}
