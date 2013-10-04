#include "../LibDos9.h"

LIBDOS9 int Dos9_AddCommandDynamic(LPCOMMANDINFO lpciCommandInfo, LPCOMMANDLIST* lpclListEntry)
{
    int iRet;
    LPCOMMANDLIST lpclList;
    if (!*lpclListEntry)
    {
        *lpclListEntry=malloc(sizeof(COMMANDLIST));
        if (!*lpclListEntry) return -1;
        _Dos9_FillCommandList(*lpclListEntry, lpciCommandInfo);
        return 0;
    }
    lpclList=*lpclListEntry;
    while ((iRet=stricmp(lpciCommandInfo->ptrCommandName,lpclList->ptrCommandName)))
    {
        if (iRet>0)
        {
            if (!lpclList->lpclRightRoot)
            {
                lpclList->lpclRightRoot=malloc(sizeof(COMMANDLIST));
                lpclList=lpclList->lpclRightRoot;
                if (!lpclList) return -1;
                _Dos9_FillCommandList(lpclList, lpciCommandInfo);
                return 0;
            }
            lpclList=lpclList->lpclRightRoot;
        }
        else if (iRet<0)
        {
            if (!lpclList->lpclLeftRoot)
            {
                lpclList->lpclLeftRoot=malloc(sizeof(COMMANDLIST));
                lpclList=lpclList->lpclLeftRoot;
                if (!lpclList) return -1;
                _Dos9_FillCommandList(lpclList, lpciCommandInfo);
                return 0;
            }
            lpclList=lpclList->lpclLeftRoot;
        }
    }
    return 0;
}

int _Dos9_FillCommandList(LPCOMMANDLIST lpclList, LPCOMMANDINFO lpciCommandInfo)
{
    lpclList->lpclLeftRoot=NULL;
    lpclList->lpclRightRoot=NULL;
    lpclList->iLenght=strlen(lpciCommandInfo->ptrCommandName);
    lpclList->cfFlag=lpciCommandInfo->cfFlag;
    lpclList->lpCommandProc=lpciCommandInfo->lpCommandProc;
    lpclList->ptrCommandName=malloc(lpclList->iLenght+1);
    lpclList->lpclLeftRoot=NULL;
    lpclList->lpclRightRoot=NULL;
    if (!lpclList->ptrCommandName) return -1;
    strcpy(lpclList->ptrCommandName,lpciCommandInfo->ptrCommandName);
    return 0;
}

int _Dos9_Sort(const void* ptrS, const void* ptrD)
{
    return stricmp(((LPCOMMANDINFO)ptrS)->ptrCommandName,((LPCOMMANDINFO)ptrD)->ptrCommandName);
}

int _Dos9_PutSeed(LPCOMMANDINFO lpciCommandInfo, int iSegBottom, int iSegTop, LPCOMMANDLIST* lpclList)
{
    int iMid=(iSegBottom+iSegTop)/2;
    if (iMid!=iSegBottom)
    {
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
    /* FistStep sort the function By alphabetical order */
    qsort(lpciCommandInfo, i, sizeof(COMMANDINFO), _Dos9_Sort);
    if (Mid) _Dos9_PutSeed(lpciCommandInfo, 0, i+1, &lpclList);
    return lpclList;
}

LIBDOS9 COMMANDFLAG   Dos9_GetCommandProc(char* lpCommandLine, LPCOMMANDLIST lpclCommandList, void** lpcpCommandProcedure)
{
    int iRet;
    while ((iRet=strnicmp(lpCommandLine,lpclCommandList->ptrCommandName,lpclCommandList->iLenght)))
    {
        if (iRet>0)
        {
            if (!(lpclCommandList=lpclCommandList->lpclRightRoot)) return -1;
        }
        else if (iRet<0)
        {
            if (!(lpclCommandList=lpclCommandList->lpclLeftRoot)) return -1;
        }
    }
    *lpcpCommandProcedure=lpclCommandList->lpCommandProc;
    return lpclCommandList->cfFlag;
}

LIBDOS9 int Dos9_FreeCommandList(LPCOMMANDLIST lpclList)
{
    /* NOT IMPLEMENTED YET */
    if (lpclList) {
        free(lpclList->ptrCommandName);
        Dos9_FreeCommandList(lpclList->lpclLeftRoot);
        Dos9_FreeCommandList(lpclList->lpclRightRoot);
        free(lpclList);
    }
    return 0;
}
