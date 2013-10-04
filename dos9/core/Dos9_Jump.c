#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

#include <errno.h>
#include <string.h>

int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName)
{
    size_t iSize=strlen(lpLabelName);
    char* lpName=lpFileName;
    char lpBuf[FILENAME_MAX];
    FILE* pFile;
    ESTR* lpLine=Dos9_EsInit();
    if ((lpFileName==NULL)) {
        Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_GETFILE, lpBuf, (void*)FILENAME_MAX);
        lpName=lpBuf;
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
                    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETFILE, (void*)lpFileName, NULL);
            }
            Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETPOS, (void*)ftell(pFile), NULL);
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
