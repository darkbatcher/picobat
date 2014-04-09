#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

#include <errno.h>
#include <string.h>

int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName)
{
    size_t iSize=strlen(lpLabelName);
    char* lpName=lpFileName;
    FILE* pFile;
    ESTR* lpLine=Dos9_EsInit();


    if ((lpFileName==NULL)) {
        lpName=ifIn.lpFileName;
    }


    if (!(pFile=fopen(lpName, "r"))) {
        Dos9_EsFree(lpLine);

        DEBUG("unable to open file : %s");
        DEBUG(strerror(errno));

        return -1;
    }

    while (!Dos9_EsGet(lpLine, pFile)) {

        if (!strnicmp(Dos9_EsToChar(lpLine), lpLabelName, iSize)) {

            if (lpFileName) {

                /* at that time, we can assume that lpFileName is not
                   the void string, because the void string is not usually
                   a valid file name */
                if (*lpFileName=='/'
                    || !strncmp(":/", lpFileName+1, 2)
                    || !strncmp(":\\", lpFileName+1, 2)) {

                    /* the path is absolute */
                    strncpy(ifIn.lpFileName, lpFileName, sizeof(ifIn.lpFileName));
                    ifIn.lpFileName[FILENAME_MAX-1]='\0';

                } else {

                    /* the path is relative */
                    snprintf(ifIn.lpFileName,
                             sizeof(ifIn.lpFileName),
                             "%s/%s",
                             Dos9_GetCurrentDir(),
                             lpFileName
                             );

                }

            }

            ifIn.iPos=ftell(pFile);
            ifIn.bEof=feof(pFile);


            DEBUG("Freeing data");

            fclose(pFile);
            Dos9_EsFree(lpLine);

            DEBUG("Jump created with success");

            return 0;
        }
    }

    fclose(pFile);
    Dos9_EsFree(lpLine);

    DEBUG("Unable to find label");

    return -1;
}

