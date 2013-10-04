#include <stdio.h>
#include <stdlib.h>

#include "../../Dos9_Core.h"

/* fonction principale du module */

int Dos9_ReadModule(int Msg, void* param1, void* param2)
{
    static int iPreviousPos, iLastPos, iEof;
    static char *lpFileName;
    FILE *pFile;
    switch (Msg)
    {
        case MODULE_READ_INIT: /* si il faut initialiser le Module */
            iPreviousPos^=iPreviousPos;
            iLastPos^=iLastPos;
            iEof^=iEof;
            lpFileName=NULL;
            return 0;

        case MODULE_READ_SETPOS:
            iLastPos=(int)param1;
            return 0;

        case MODULE_READ_GETPOS:
            return iPreviousPos;

        case MODULE_READ_SETFILE:
            if (lpFileName) free(lpFileName);
            iEof=0;
            iLastPos=0;
            if (param1 == NULL) {
                lpFileName=NULL;
                return 0;
            }
            lpFileName=strdup((char*)param1);
            return 0;

        case MODULE_READ_ISEOF:
            return iEof;

        case MODULE_READ_GETLINE:
            if (lpFileName==NULL) {
                pFile=stdin;
                iEof=Dos9_EsGet((ESTR*)param1, pFile);
            } else if ((pFile=fopen(lpFileName, "r"))) {
                fseek(pFile, iLastPos, SEEK_SET);
                iPreviousPos=iLastPos;
                iEof=Dos9_EsGet((ESTR*)param1, pFile);
                iLastPos=ftell(pFile);
                fclose(pFile);
            } else {
                iEof=1;
            }
            return 0;

        case MODULE_READ_GETFILE:
            if (lpFileName==NULL) return 1;
            strncpy((char*)param1, lpFileName, (size_t)param2);
            return 0;
        default:
            return -1;
    }
}
