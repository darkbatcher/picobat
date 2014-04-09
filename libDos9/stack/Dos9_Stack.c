#include "../LibDos9.h"


LIBDOS9     LPSTACK Dos9_PushStack(LPSTACK lpcsStack, void* ptrContent)
{
    LPSTACK lpcsTmp;

    if (!(lpcsTmp=malloc(sizeof(STACK))))
        return lpcsStack;

    lpcsTmp->lpcsPrevious=lpcsStack;
    lpcsTmp->ptrContent=ptrContent;

    return lpcsTmp;
}

LIBDOS9     LPSTACK Dos9_PopStack(LPSTACK lpcsStack, void(*pFunc)(void*))
{
    LPSTACK lpcsTmp;

    if (!lpcsStack) return NULL;

    lpcsTmp=lpcsStack->lpcsPrevious;

    if (lpcsStack->ptrContent!=NULL && pFunc!=NULL)
        pFunc(lpcsStack->ptrContent);

    free(lpcsStack);

    return lpcsTmp;
}

LIBDOS9     int Dos9_GetStack(LPSTACK lpcsStack, void** ptrContent)
{
    if (!lpcsStack) return -1;

    if (ptrContent) {
        *ptrContent=lpcsStack->ptrContent;
    }

    return 0;
}

LIBDOS9     int Dos9_ClearStack(LPSTACK lpcsStack, void(*pFunc)(void*))
{
    LPSTACK lpcsTmp;



    while (lpcsStack)
    {
        lpcsTmp=lpcsStack->lpcsPrevious;

        if (lpcsStack->ptrContent!=NULL && pFunc!=NULL)
            pFunc(lpcsStack->ptrContent);

        free(lpcsStack);

        lpcsStack=lpcsTmp;
    }

    return 0;
}

