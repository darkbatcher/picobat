/*
 *
 *   libpBat - The pBat project
 *   Copyright (C) 2010-2016 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ctype.h>
#include "../libpBat.h"

short pBat_MakeFileAttributes(const char* lpToken)
{
    short wAttr=0;
    short wFlag=0;
    char bNeg=FALSE;
    for (;*lpToken;lpToken++) {
        switch(toupper(*lpToken)){
            case 'I':
                wFlag=PBAT_CMD_ATTR_VOID;
                break;
            case 'A':
                wFlag=PBAT_CMD_ATTR_ARCHIVE;
                break;
            case 'R':
                wFlag=PBAT_CMD_ATTR_READONLY;
                break;
            case 'H':
                wFlag=PBAT_CMD_ATTR_HIDEN;
                break;
            case 'S':
                wFlag=PBAT_CMD_ATTR_SYSTEM;
                break;
            case 'D':
                wFlag=PBAT_CMD_ATTR_DIR;
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

int pBat_CheckFileAttributes(short wAttr, const FILELIST* lpflList)
{
    int iResult=0;
    int iReturn=1;
    int iAttributes;

    if (!wAttr) return TRUE;

    iAttributes=pBat_GetFileMode(lpflList);

    if (wAttr & PBAT_CMD_ATTR_ARCHIVE) {
        iResult=iAttributes & PBAT_FILE_ARCHIVE;
        if (wAttr & PBAT_CMD_ATTR_ARCHIVE_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & PBAT_CMD_ATTR_HIDEN) {
        iResult=iAttributes & PBAT_FILE_HIDDEN;
        if (wAttr & PBAT_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & PBAT_CMD_ATTR_SYSTEM) {
        iResult=iAttributes & PBAT_FILE_SYSTEM;
        if (wAttr & PBAT_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & PBAT_CMD_ATTR_VOID) {
        iResult=(pBat_GetFileSize(lpflList)==0);
        if (wAttr & PBAT_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }


    if (wAttr & PBAT_CMD_ATTR_DIR) {
        iResult=iAttributes & PBAT_FILE_DIR;
        if (wAttr & PBAT_CMD_ATTR_DIR_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    #if defined WIN32

    if (wAttr & PBAT_CMD_ATTR_READONLY) {
        iResult=iAttributes & PBAT_FILE_READONLY;
        if (wAttr & PBAT_CMD_ATTR_READONLY_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    #elif !defined(WIN32)

    if (wAttr & PBAT_CMD_ATTR_READONLY) {

        iResult=(pBat_GetFileMode(lpflList) & (PBAT_FILE_READ | PBAT_FILE_WRITE)) == PBAT_FILE_READ;
        if (wAttr & PBAT_CMD_ATTR_READONLY_N) iResult=!iResult;
        iReturn=iReturn &&iResult;

    }

    #endif

    return iReturn;
}

/* A function that checks whether files in pIn are matching attributes and
   produce two FILELIST* struct from the first that contains matching and
   unmatching files*/
LIBPBAT int pBat_AttributesSplitFileList(short wAttr, FILELIST* pIn,
                            FILELIST** pSelected, FILELIST** pRemaining)
{

    FILELIST *pMatch=NULL,
             *pUnMatch=NULL;

    *pSelected = NULL;
    *pRemaining = NULL;

    while (pIn) {

        if (pBat_CheckFileAttributes(wAttr, pIn)) {

            if (!pMatch) {

                pMatch = pIn;
                *pSelected = pIn;

            } else {

                pMatch->lpflNext = pIn;
                pMatch = pIn;

            }

        } else {

            if (!pUnMatch) {

                pUnMatch = pIn;
                *pRemaining = pIn;

            } else {

                pUnMatch->lpflNext = pIn;
                pUnMatch = pIn;

            }


        }

        pIn = pIn->lpflNext;

    }

    if (pMatch)
        pMatch->lpflNext = NULL;

    if (pUnMatch)
        pUnMatch->lpflNext = NULL;

    return 0;

}
