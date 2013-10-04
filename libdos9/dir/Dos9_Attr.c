#include "../LibDos9.h"

short Dos9_MakeFileAttributes(const char* lpToken)
{
    short wAttr=0;
    short wFlag=0;
    char bNeg=FALSE;
    for (;*lpToken;lpToken++) {
        switch(toupper(*lpToken)){
            case 'I':
                wFlag=DOS9_CMD_ATTR_VOID;
                break;
            case 'A':
                wFlag=DOS9_CMD_ATTR_ARCHIVE;
                break;
            case 'R':
                wFlag=DOS9_CMD_ATTR_READONLY;
                break;
            case 'H':
                wFlag=DOS9_CMD_ATTR_HIDEN;
                break;
            case 'S':
                wFlag=DOS9_CMD_ATTR_SYSTEM;
                break;
            case 'D':
                wFlag=DOS9_CMD_ATTR_DIR;
                break;
            case '-':
                bNeg=TRUE;
                wFlag=0;
            default:
                wFlag=0;
        }
        if (bNeg && wFlag) {
            wFlag|=wFlag<<1;
            bNeg=FALSE;
        }
        wAttr|=wFlag;
    }
    return wAttr;
}

int Dos9_CheckFileAttributes(short wAttr, const FILELIST* lpflList)
{
    int iResult=0;
    int iReturn=1;
    int iAttributes;

    if (!wAttr) return TRUE;

    iAttributes=Dos9_GetFileMode(lpflList);

    if (wAttr & DOS9_CMD_ATTR_ARCHIVE) {
        iResult=iAttributes & DOS9_FILE_ARCHIVE;
        if (wAttr & DOS9_CMD_ATTR_ARCHIVE_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & DOS9_CMD_ATTR_HIDEN) {
        iResult=iAttributes & DOS9_FILE_HIDDEN;
        if (wAttr & DOS9_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & DOS9_CMD_ATTR_SYSTEM) {
        iResult=iAttributes & DOS9_FILE_SYSTEM;
        if (wAttr & DOS9_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & DOS9_CMD_ATTR_VOID) {
        iResult=(Dos9_GetFileSize(lpflList)==0);
        if (wAttr & DOS9_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }


    if (wAttr & DOS9_CMD_ATTR_DIR) {
        iResult=iAttributes & DOS9_FILE_DIR;
        if (wAttr & DOS9_CMD_ATTR_DIR_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    #if defined WIN32

    if (wAttr & DOS9_CMD_ATTR_READONLY) {
        iResult=iAttributes & DOS9_FILE_READONLY;
        if (wAttr & DOS9_CMD_ATTR_READONLY_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    #elif defined _POSIX_C_SOURCE

    if (wAttr & DOS9_CMD_ATTR_READONLY) {

        iResult=(Dos9_GetFileAttributes(lpflList) & (DOS9_FILE_READ | DOS9_FILE_WRITE))==DOS9_FILE_READ;
        if (wAttr & DOS9_CMD_ATTR_READONLY_N) iResult=!iResult;
        iReturn=iReturn &&iResult;

    }

    #endif

    return iReturn;
}
