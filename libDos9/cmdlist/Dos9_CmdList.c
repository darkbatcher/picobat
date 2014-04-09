/*
 *
 *   libDos9 - The Dos9 project
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

#include "../LibDos9.h"

LIBDOS9 int Dos9_AddCommandDynamic(LPCOMMANDINFO lpciCommandInfo, LPCOMMANDLIST* lpclListEntry)
{
    int iRet;
    LPCOMMANDLIST lpclList;

    if (!*lpclListEntry)
    {

        *lpclListEntry=malloc(sizeof(COMMANDLIST));

        if (!*lpclListEntry)
            return -1;

        if ((_Dos9_FillCommandList(*lpclListEntry, lpciCommandInfo))) {

            /* cleanup and quit */

            free(*lpclListEntry);

            *lpclListEntry=NULL;

            return -1;

        }

        return 0;
    }

    lpclList=*lpclListEntry;

    while ((iRet=stricmp(lpciCommandInfo->ptrCommandName,lpclList->ptrCommandName))) {

        if (iRet>0) {

            if (!lpclList->lpclRightRoot) {

                lpclList->lpclRightRoot=malloc(sizeof(COMMANDLIST));

                if (!lpclList->lpclRightRoot)
                    return -1;

                if ((_Dos9_FillCommandList(lpclList->lpclRightRoot, lpciCommandInfo))) {

                    /* cleanup and exit */

                    free(lpclList->lpclRightRoot);

                    return -1;

                }

                return 0;

            }

            lpclList=lpclList->lpclRightRoot;

        } else if (iRet<0) {

            if (!lpclList->lpclLeftRoot) {

                lpclList->lpclLeftRoot=malloc(sizeof(COMMANDLIST));

                if (!lpclList)
                    return -1;

                if ((_Dos9_FillCommandList(lpclList->lpclLeftRoot, lpciCommandInfo))) {

                    free(lpclList->lpclLeftRoot);

                    return -1;

                }

                return 0;

            }

            lpclList=lpclList->lpclLeftRoot;

        } else {

            /* if this is indeed equal to 0 */

        }

    }

    return 0;

}

int _Dos9_FillCommandList(LPCOMMANDLIST lpclList, LPCOMMANDINFO lpciCommandInfo)
{

    lpclList->iLenght=strlen(lpciCommandInfo->ptrCommandName);

    if (!(lpclList->ptrCommandName=malloc(lpclList->iLenght+1)))
        return -1;

    strcpy(lpclList->ptrCommandName,lpciCommandInfo->ptrCommandName);

    lpclList->cfFlag=lpciCommandInfo->cfFlag;


    if (lpciCommandInfo->cfFlag & DOS9_ALIAS_FLAG) {

        if (!(lpclList->lpCommandProc
              =malloc(strlen(lpciCommandInfo->lpCommandProc)+1))) {

            free(lpclList->ptrCommandName);
            return -1;
        }

        strcpy(lpclList->lpCommandProc, lpciCommandInfo->lpCommandProc);

    } else {

        lpclList->lpCommandProc=lpciCommandInfo->lpCommandProc;

    }


    lpclList->lpclLeftRoot=NULL;
    lpclList->lpclRightRoot=NULL;

    return 0;
}

int _Dos9_Sort(const void* ptrS, const void* ptrD)
{
    return stricmp(((LPCOMMANDINFO)ptrS)->ptrCommandName,((LPCOMMANDINFO)ptrD)->ptrCommandName);
}

int _Dos9_PutSeed(LPCOMMANDINFO lpciCommandInfo, int iSegBottom, int iSegTop, LPCOMMANDLIST* lpclList)
{
    int iMid=(iSegBottom+iSegTop)/2;

    if (iMid!=iSegBottom) {

        Dos9_AddCommandDynamic(&(lpciCommandInfo[iMid-1]),lpclList);
        _Dos9_PutSeed(lpciCommandInfo, iSegBottom, iMid, lpclList);
        _Dos9_PutSeed(lpciCommandInfo, iMid, iSegTop, lpclList);

    }

    return 0;
}

LIBDOS9 LPCOMMANDLIST Dos9_MapCommandInfo(LPCOMMANDINFO lpciCommandInfo, int i)
{
    int Mid=i/2-1;
    LPCOMMANDLIST lpclList=NULL;

    /* Fist sort the function By alphabetical order */
    qsort(lpciCommandInfo, i, sizeof(COMMANDINFO), _Dos9_Sort);

    if (Mid)
        _Dos9_PutSeed(lpciCommandInfo, 0, i+1, &lpclList);

    return lpclList;
}

LIBDOS9 COMMANDFLAG   Dos9_GetCommandProc(char* lpCommandLine, LPCOMMANDLIST lpclCommandList, void** lpcpCommandProcedure)
{
    int iRet;
    LPCOMMANDLIST lpclCommandReturn=NULL;


    while (lpclCommandList) {

        iRet=strnicmp(lpCommandLine,
                      lpclCommandList->ptrCommandName,
                      lpclCommandList->iLenght);

        loop_again:

        if (iRet>0) {

           lpclCommandList=lpclCommandList->lpclRightRoot;

        } else if (iRet<0) {

            lpclCommandList=lpclCommandList->lpclLeftRoot;

        } else {

            if ((lpclCommandList->cfFlag & ~DOS9_ALIAS_FLAG)) {

                /* if there is a limit, just check if next
                   character is a delimiter.

                   Note that if we have been here, it's because
                   the string is at least as large as the
                   comparing string */

                switch(lpCommandLine
                       [lpclCommandList->cfFlag & ~DOS9_ALIAS_FLAG]) {

                    case ' ':
                    case '\n':
                    case '\t':
                    case '\0':
                        *lpcpCommandProcedure=lpclCommandList->lpCommandProc;
                        return lpclCommandList->cfFlag;

                }

                /* it did not matched, just try again */

                iRet=stricmp(lpCommandLine, lpclCommandList->ptrCommandName);

                goto loop_again;

            } else {

                /* if there is no delimiter to check, return
                   that, because it's obviously the right
                   command (match the shortest match found) */

                *lpcpCommandProcedure=lpclCommandList->lpCommandProc;
                return lpclCommandList->cfFlag;

            }

        }

    }

    return -1;
}

LIBDOS9 int Dos9_FreeCommandList(LPCOMMANDLIST lpclList)
{
    if (lpclList) {

        free(lpclList->ptrCommandName);

        /* if it is in fact an alias */
        if (lpclList->cfFlag & DOS9_ALIAS_FLAG)
            free(lpclList->lpCommandProc);

        Dos9_FreeCommandList(lpclList->lpclLeftRoot);
        Dos9_FreeCommandList(lpclList->lpclRightRoot);
        free(lpclList);

    }

    return 0;
}

LIBDOS9 LPCOMMANDLIST   Dos9_ReMapCommandInfo(LPCOMMANDLIST lpclCommandList)
{
    /* trying to re-balance an unbalanced tree is
       to much difficult for me, thus, create a new
       commandinfo structure and Map it again */

    size_t iNewSize;
    size_t i;
    COMMANDINFO* lpciCmdInfo;
    COMMANDLIST* lpclList;

    iNewSize=_Dos9_CountListItems(lpclCommandList);

    if (!(lpciCmdInfo=malloc(iNewSize*sizeof(COMMANDINFO))))
        return NULL;

    /* fill the new command info structure from the
       old command list */
    _Dos9_FillInfoFromList(lpciCmdInfo, lpclCommandList);

    lpclList=Dos9_MapCommandInfo(lpciCmdInfo, iNewSize);

    free(lpciCmdInfo);

    return lpclList;
}

size_t _Dos9_CountListItems(LPCOMMANDLIST lpclList)
{
    size_t iRet=0;

    if (lpclList==NULL) {

        /* nothing to count right there */

        return 0;

    }


    iRet+=_Dos9_CountListItems(lpclList->lpclLeftRoot)
          + _Dos9_CountListItems(lpclList->lpclRightRoot)+1;


    return iRet;

}

LPCOMMANDINFO _Dos9_FillInfoFromList(LPCOMMANDINFO lpCommandInfo, LPCOMMANDLIST lpCommandList)
{

    if (lpCommandList == NULL)
        return lpCommandInfo;

    /* copy the content of the
       command list */

    lpCommandInfo->ptrCommandName = lpCommandList->ptrCommandName;
    lpCommandInfo->lpCommandProc = lpCommandList->lpCommandProc;
    lpCommandInfo->cfFlag = lpCommandList->cfFlag;

    lpCommandInfo++;

    lpCommandInfo=_Dos9_FillInfoFromList(lpCommandInfo, lpCommandList->lpclLeftRoot);
    lpCommandInfo=_Dos9_FillInfoFromList(lpCommandInfo, lpCommandList->lpclRightRoot);

    return lpCommandInfo;

}
