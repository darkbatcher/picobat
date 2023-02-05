/*
 *
 *   libpBat - The pBat project
 *   Copyright (C) 2010-2017 Romain GARBI
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#ifndef WIN32
#include <strings.h>
#endif

#include "../libpBat.h"
#include "../libpBat-int.h"

LIBPBAT int pBat_AddCommandDynamic(LPCOMMANDINFO lpciCommandInfo,
                                                LPCOMMANDLIST* lpclListEntry)
{
    int iRet;
    LPCOMMANDLIST lpclList;

    if (!*lpclListEntry)
    {

        *lpclListEntry=malloc(sizeof(COMMANDLIST));

        if (!*lpclListEntry)
            return -1;

        if ((_pBat_FillCommandList(*lpclListEntry, lpciCommandInfo))) {

            /* cleanup and quit */

            free(*lpclListEntry);

            *lpclListEntry=NULL;

            return -1;

        }

        return 0;
    }

    lpclList=*lpclListEntry;

    while ((iRet=stricmp(lpciCommandInfo->ptrCommandName,
                                            lpclList->ptrCommandName))) {

        if (iRet>0) {

            if (!lpclList->lpclRightRoot) {

                lpclList->lpclRightRoot=malloc(sizeof(COMMANDLIST));

                if (!lpclList->lpclRightRoot)
                    return -1;

                if ((_pBat_FillCommandList(lpclList->lpclRightRoot,
                                                        lpciCommandInfo))) {

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

                if ((_pBat_FillCommandList(lpclList->lpclLeftRoot, lpciCommandInfo))) {

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

int _pBat_FillCommandList(LPCOMMANDLIST lpclList, LPCOMMANDINFO lpciCommandInfo)
{

    lpclList->iLenght=strlen(lpciCommandInfo->ptrCommandName);

    if (!(lpclList->ptrCommandName=malloc(lpclList->iLenght+1)))
        return -1;

    strcpy(lpclList->ptrCommandName,lpciCommandInfo->ptrCommandName);

    lpclList->cfFlag=lpciCommandInfo->cfFlag;


    if (lpciCommandInfo->cfFlag & PBAT_DEF_FLAG) {

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

int _pBat_Sort(const void* ptrS, const void* ptrD)
{
    return stricmp(((LPCOMMANDINFO)ptrS)->ptrCommandName,
                                    ((LPCOMMANDINFO)ptrD)->ptrCommandName);
}

int _pBat_PutSeed(LPCOMMANDINFO lpciCommandInfo, int iSegBottom,
                                        int iSegTop, LPCOMMANDLIST* lpclList)
{
    int iMid=(iSegBottom+iSegTop)/2;



    if (iMid!=iSegBottom) {

        pBat_AddCommandDynamic(&(lpciCommandInfo[iMid-1]),lpclList);
        _pBat_PutSeed(lpciCommandInfo, iSegBottom, iMid, lpclList);
        _pBat_PutSeed(lpciCommandInfo, iMid, iSegTop, lpclList);

    }

    return 0;
}

LIBPBAT LPCOMMANDLIST pBat_MapCommandInfo(LPCOMMANDINFO lpciCommandInfo, int i)
{
    int Mid=i/2-1;
    LPCOMMANDLIST lpclList=NULL;

    /* Fist sort the function By alphabetical order */
    qsort(lpciCommandInfo, i, sizeof(COMMANDINFO), _pBat_Sort);

    if (Mid)
        _pBat_PutSeed(lpciCommandInfo, 0, i+1, &lpclList);

    return lpclList;
}

LIBPBAT COMMANDFLAG   pBat_GetCommandProc(char* lpCommandLine,
                LPCOMMANDLIST lpclCommandList, void** lpcpCommandProcedure)
{
    int iRet;


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

            if ((lpclCommandList->cfFlag & ~PBAT_COMMAND_FLAGS)) {

                /* if there is a limit, just check if next
                   character is a delimiter.

                   Note that if we have been here, it's because
                   the string is at least as large as the
                   comparing string */

                switch(lpCommandLine
                       [lpclCommandList->cfFlag & ~PBAT_COMMAND_FLAGS]) {

                    case ' ':
                    case ',':
                    case ';':
                    case '\n':
                    case '\t':
                    case '\0':
                    case '/':
                        *lpcpCommandProcedure=lpclCommandList->lpCommandProc;
                        return lpclCommandList->cfFlag;

                }

                /* it did not match, just try again */

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

LIBPBAT int				pBat_ReplaceCommand(LPCOMMANDINFO lpciCommand, LPCOMMANDLIST lpclCommandList)
{
	int iRet;

	char  *lpCommandLine=lpciCommand->ptrCommandName,
		  *lpBuf;

    while (lpclCommandList) {

        iRet=strnicmp(lpCommandLine,
                      lpclCommandList->ptrCommandName,
                      lpclCommandList->iLenght);

        if (iRet>0) {

           lpclCommandList=lpclCommandList->lpclRightRoot;

        } else if (iRet<0) {

            lpclCommandList=lpclCommandList->lpclLeftRoot;

        } else {

			/* substitute the given COMMANDINFO structure */

			if (lpciCommand->cfFlag & PBAT_DEF_FLAG) {

				lpBuf=malloc(strlen((char*)lpciCommand->lpCommandProc)+1);

				if (!lpBuf)
					return -1;

				if (lpclCommandList->cfFlag & PBAT_DEF_FLAG)
					free(lpclCommandList->lpCommandProc);

				strcpy(lpBuf, lpciCommand->lpCommandProc);
				lpclCommandList->lpCommandProc=lpBuf;

			}

			lpclCommandList->cfFlag=lpciCommand->cfFlag;

			return 0;

        }

    }

    return -1;
}

LIBPBAT int pBat_FreeCommandList(LPCOMMANDLIST lpclList)
{
    if (lpclList) {

        /* printf("Freeing %p (\"%s\") : next (%p;%p)\n",
                lpclList, lpclList->ptrCommandName, lpclList->lpclLeftRoot,
                    lpclList->lpclRightRoot); */

        free(lpclList->ptrCommandName);

        /* if it is in fact an alias */
        if (lpclList->cfFlag & PBAT_DEF_FLAG)
            free(lpclList->lpCommandProc);

        pBat_FreeCommandList(lpclList->lpclLeftRoot);
        pBat_FreeCommandList(lpclList->lpclRightRoot);
        free(lpclList);

    }

    return 0;
}

LIBPBAT LPCOMMANDLIST   pBat_DuplicateCommandList(LPCOMMANDLIST lpclList)
{
    LPCOMMANDLIST lpclRet = NULL;

    if (lpclList && (lpclRet = malloc(sizeof(COMMANDLIST)))) {

        if (!(lpclRet->ptrCommandName =
                    malloc(strlen(lpclList->ptrCommandName)+1)))
            goto err;

        strcpy(lpclRet->ptrCommandName, lpclList->ptrCommandName);

        lpclRet->cfFlag = lpclList->cfFlag;
        lpclRet->iLenght = lpclList->iLenght;

        if (lpclRet->cfFlag & PBAT_DEF_FLAG) {

            if (!(lpclRet->lpCommandProc = malloc(
                            strlen(lpclList->lpCommandProc)+1)))
                goto err;

            strcpy(lpclRet->lpCommandProc, lpclList->lpCommandProc);

        } else {

            lpclRet->lpCommandProc = lpclList->lpCommandProc;

        }

        lpclRet->lpclRightRoot = NULL;
        lpclRet->lpclLeftRoot = NULL;

        if ((lpclList->lpclLeftRoot && !(lpclRet->lpclLeftRoot =
                        pBat_DuplicateCommandList(lpclList->lpclLeftRoot)))
            || (lpclList->lpclRightRoot && !(lpclRet->lpclRightRoot =
                        pBat_DuplicateCommandList(lpclList->lpclRightRoot))))
            goto err;

    }

    return lpclRet;

err:
    if (lpclRet) {

        if (lpclRet->ptrCommandName) {

            free(lpclRet->ptrCommandName);

            if ((lpclRet->cfFlag & PBAT_DEF_FLAG)
                    && lpclRet->lpCommandProc) {
                free(lpclRet->lpCommandProc);

                if (lpclRet->lpclLeftRoot)
                    pBat_FreeCommandList(lpclRet->lpclLeftRoot);

            }
        }

        free(lpclRet);

    }

    return NULL;
}

LIBPBAT LPCOMMANDLIST   pBat_ReMapCommandInfo(LPCOMMANDLIST lpclCommandList)
{
    /* trying to re-balance an unbalanced tree is
       too difficult for me, thus, create a new
       commandinfo structure and Map it again */

    size_t iNewSize;
    COMMANDINFO* lpciCmdInfo;
    COMMANDLIST* lpclList;

    iNewSize=_pBat_CountListItems(lpclCommandList);

    if (!(lpciCmdInfo=malloc(iNewSize*sizeof(COMMANDINFO))))
        return NULL;

    /* fill the new command info structure from the
       old command list */
    _pBat_FillInfoFromList(lpciCmdInfo, lpclCommandList);

    lpclList=pBat_MapCommandInfo(lpciCmdInfo, iNewSize);

    free(lpciCmdInfo);

    return lpclList;
}

size_t _pBat_CountListItems(LPCOMMANDLIST lpclList)
{
    size_t iRet=0;

    if (lpclList==NULL) {

        /* nothing to count right there */

        return 0;

    }


    iRet+=_pBat_CountListItems(lpclList->lpclLeftRoot)
          + _pBat_CountListItems(lpclList->lpclRightRoot)+1;


    return iRet;

}

LPCOMMANDINFO _pBat_FillInfoFromList(LPCOMMANDINFO lpCommandInfo, LPCOMMANDLIST lpCommandList)
{

    if (lpCommandList == NULL)
        return lpCommandInfo;

    /* copy the content of the
       command list */

    lpCommandInfo->ptrCommandName = lpCommandList->ptrCommandName;
    lpCommandInfo->lpCommandProc = lpCommandList->lpCommandProc;
    lpCommandInfo->cfFlag = lpCommandList->cfFlag;

    lpCommandInfo++;

    lpCommandInfo=_pBat_FillInfoFromList(lpCommandInfo, lpCommandList->lpclLeftRoot);
    lpCommandInfo=_pBat_FillInfoFromList(lpCommandInfo, lpCommandList->lpclRightRoot);

    return lpCommandInfo;

}
