#include "../LibDos9.h"


LIBDOS9     LPSTACK Dos9_PushStack(LPSTACK lpcsStack, void* ptrContent, int iFlag)
{
    LPCALLSTACK lpcsTmp;
    if (!(lpcsTmp=malloc(sizeof(CALLSTACK)))) return lpcsStack;
    lpcsTmp->lpcsPrevious=lpcsStack;
    lpcsTmp->iFlag=iFlag;
    lpcsTmp->ptrContent=ptrContent;
    return lpcsTmp;
}

LIBDOS9     LPSTACK Dos9_PopStack(LPSTACK lpcsStack)
{
    LPCALLSTACK lpcsTmp;
    if (!lpcsStack) return NULL;
    lpcsTmp=lpcsStack->lpcsPrevious;
    free(lpcsStack);
    return lpcsTmp;
}

LIBDOS9     int Dos9_GetStack(LPSTACK lpcsStack, void** ptrContent)
{
    if (!lpcsStack) return -1;
    *ptrContent=lpcsStack->ptrContent;
    return lpcsStack->iFlag;
}

LIBDOS9     int Dos9_ClearStack(LPSTACK lpcsStack)
{
    LPCALLSTACK lpcsTmp;
    if (!lpcsStack) return -1;
    lpcsTmp=lpcsStack->lpcsPrevious;
    while (lpcsStack)
    {
        free(lpcsStack->ptrContent);
        free(lpcsStack);
        lpcsStack=lpcsTmp;
        lpcsTmp=lpcsStack->lpcsPrevious;
    }
    return 0;
}

