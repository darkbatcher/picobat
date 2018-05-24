/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2018 Romain GARBI
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
#include "Dos9_Core.h"

void Dos9_PushEnvStack(void)
{
    ENVBUF* new;
    ENVSTACK* item;

    if ((item = malloc(sizeof(ENVSTACK))) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_PushEnvStack()",
                                -1);

    item->previous = lpesEnv;
    item->lpeEnv = lpeEnv;
    item->bDelayedExpansion = bDelayedExpansion;
    item->bUseFloats = bUseFloats;
    item->bCmdlyCorrect = bCmdlyCorrect;

    lpeEnv = Dos9_EnvDup(lpeEnv);
    lpesEnv = item;
}

void Dos9_PopEnvStack(void)
{
    ENVSTACK* item;

    if (lpesEnv == NULL)
        return;

    item = lpesEnv;
    lpesEnv = lpesEnv->previous;

    Dos9_EnvFree(lpeEnv);
    lpeEnv = item->lpeEnv;
    bDelayedExpansion = item->bDelayedExpansion;
    bUseFloats = item->bUseFloats;
    bCmdlyCorrect = item->bCmdlyCorrect;

    free(item);
}

void Dos9_FreeEnvStack(void)
{
    ENVSTACK* item;

    while (lpesEnv) {

        item = lpesEnv;
        lpesEnv = item->previous;

        Dos9_EnvFree(lpeEnv);
        lpeEnv = item->lpeEnv;
        bDelayedExpansion = item->bDelayedExpansion;
        bUseFloats = item->bUseFloats;
        bCmdlyCorrect = item->bCmdlyCorrect;

        free(item);
    }
}
