/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libDos9.h>

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#include "Dos9_FilePath.h"

int Dos9_GetFilePath(char* lpFullPath, const char* lpPartial, size_t iBufSize)
{
    ESTR* lpEsTmp=Dos9_EsInit();
    ESTR* lpEsPart=Dos9_EsInit();
    ESTR* lpEsFinalPath=Dos9_EsInit();

    char *lpPathToken=getenv("PATH");

    int   bFirstLoop=TRUE;

    #ifdef WIN32

        char *lpPathExtToken,
             *lpPathExtBegin=getenv("PATHEXT");
        int bFirstSubLoop;

    #endif // WIN32

    DOS9_DBG("[Dos9_GetFilePath()]*** Start research of file : \"%s\"\n", lpPartial);

    do {

        if (bFirstLoop) {

            Dos9_EsCpy(lpEsTmp, lpPartial);
            bFirstLoop=FALSE;

        } else {

            Dos9_MakePath(lpEsTmp, 2, Dos9_EsToChar(lpEsPart), lpPartial);

        }

        DOS9_DBG("\t[*] trying \"%s\"\n", Dos9_EsToChar(lpEsTmp));

        #ifdef WIN32

            bFirstSubLoop=TRUE;
            lpPathExtToken=lpPathExtBegin;

            do {

                if (bFirstSubLoop) {

                    Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
                    bFirstSubLoop=FALSE;

                } else {

                    Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
                    Dos9_EsCatE(lpEsFinalPath, lpEsPart);

                }

                DOS9_DBG("\t\t[*] trying \"%s\"\n", Dos9_EsToChar(lpEsFinalPath));

                if (access(Dos9_EsToChar(lpEsFinalPath), F_OK)==0)
                    goto file_found;



            } while ((lpPathExtToken=Dos9_GetPathNextPart(lpPathExtToken, lpEsPart)));

        #else

            if (access(Dos9_EsToChar(lpEsTmp), F_OK)==0) {

                Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
                goto file_found;

            }

        #endif


    } while ((lpPathToken=Dos9_GetPathNextPart(lpPathToken, lpEsPart)));

    Dos9_EsFree(lpEsPart);
    Dos9_EsFree(lpEsTmp);
    Dos9_EsFree(lpEsFinalPath);

    DOS9_DBG("[Dos9_GetFilePath()]*** Finished without match (-1)\n");

    return -1;

    file_found:

        snprintf(lpFullPath ,
                 iBufSize,
                 "%s",
                 Dos9_EsToChar(lpEsFinalPath)
                 );

        DOS9_DBG("[Dos9_GetFilePath()]*** Found \"%s\"\n", Dos9_EsToChar(lpEsFinalPath));

        Dos9_EsFree(lpEsPart);
        Dos9_EsFree(lpEsTmp);
        Dos9_EsFree(lpEsFinalPath);

        return 0;
}


char* Dos9_GetPathNextPart(char* lpPath, ESTR* lpReturn)
{
    char*   lpNextToken;

    if (*lpPath=='\0')
        return NULL;

    if (!(lpNextToken=strchr(lpPath, DOS9_PATH_DELIMITER))) {

        Dos9_EsCpy(lpReturn, lpPath);

        while (*lpPath)
            lpPath++;

        return lpPath;

    }

    Dos9_EsCpyN(lpReturn,
                lpPath,
                lpNextToken - lpPath);

    return ++lpNextToken;
}

int Dos9_MakePath(ESTR* lpReturn, int nOps, ...)
{
    va_list vaList;

    int     i;

    char   *lpBegin,
           *lpEnd;

    va_start(vaList, nOps);

    Dos9_EsCpy(lpReturn, "");

    for (i=0;i<nOps;i++) {

        lpBegin=Dos9_EsToChar(lpReturn);

        lpEnd=Dos9_GetLastChar(lpReturn);

        if (*lpBegin!='\0'
            && *lpEnd!='\\'
            && *lpEnd!='/') {

            /* if there are no dir terminating characters and still
               arguments, just cat a '/' */

            Dos9_EsCat(lpReturn, "/");

        }

        Dos9_EsCat(lpReturn, va_arg(vaList,char*));

    }

    va_end(vaList);

    return 0;
}

char* Dos9_GetLastChar(ESTR* lpReturn)
{

    char* lpCh=Dos9_EsToChar(lpReturn);

    if (*lpCh=='\0')
        return lpCh;

    while (*(lpCh+1))
        lpCh++;

    return lpCh;

}
