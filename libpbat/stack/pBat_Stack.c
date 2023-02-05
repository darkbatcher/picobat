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
#include "../libpBat.h"
#include "../libpBat-int.h"

LIBPBAT     LPSTACK pBat_PushStack(LPSTACK lpcsStack, void* ptrContent)
{
    LPSTACK lpcsTmp;

    if (!(lpcsTmp=malloc(sizeof(STACK))))
        return lpcsStack;

    lpcsTmp->lpcsPrevious=lpcsStack;
    lpcsTmp->ptrContent=ptrContent;

    return lpcsTmp;
}

LIBPBAT     LPSTACK pBat_PopStack(LPSTACK lpcsStack, void(*pFunc)(void*))
{
    LPSTACK lpcsTmp;

    if (!lpcsStack) return NULL;

    lpcsTmp=lpcsStack->lpcsPrevious;

    if (lpcsStack->ptrContent!=NULL && pFunc!=NULL)
        pFunc(lpcsStack->ptrContent);

    free(lpcsStack);

    return lpcsTmp;
}

LIBPBAT     int pBat_GetStack(LPSTACK lpcsStack, void** ptrContent)
{
    if (!lpcsStack) return -1;

    if (ptrContent) {
        *ptrContent=lpcsStack->ptrContent;
    }

    return 0;
}

LIBPBAT     int pBat_ClearStack(LPSTACK lpcsStack, void(*pFunc)(void*))
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
