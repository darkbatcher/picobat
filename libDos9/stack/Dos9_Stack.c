/*
 *
 *   libDos9 - The Dos9 project
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
#include "../libDos9.h"
#include "../libDos9-int.h"

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
