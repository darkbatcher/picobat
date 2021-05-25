/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#include "pBat_Core.h"

void pBat_PushEnvStack(void)
{
    /* ENVBUF* new; */
    ENVSTACK* item;

    if ((item = malloc(sizeof(ENVSTACK))) == NULL)
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_PushEnvStack()",
                                -1);

    item->previous = lpesEnv;
    item->lpeEnv = lpeEnv;
    item->bDelayedExpansion = bDelayedExpansion;
    item->bCmdlyCorrect = bCmdlyCorrect;

    lpeEnv = pBat_EnvDup(lpeEnv);
    lpesEnv = item;
}

void pBat_PopEnvStack(void)
{
    ENVSTACK* item;

    if (lpesEnv == NULL)
        return;

    item = lpesEnv;
    lpesEnv = lpesEnv->previous;

    pBat_EnvFree(lpeEnv);
    lpeEnv = item->lpeEnv;
    bDelayedExpansion = item->bDelayedExpansion;
    bCmdlyCorrect = item->bCmdlyCorrect;

    free(item);
}

void pBat_FreeEnvStack(void)
{
    ENVSTACK* item;

    while (lpesEnv) {

        item = lpesEnv;
        lpesEnv = item->previous;

        pBat_EnvFree(lpeEnv);
        lpeEnv = item->lpeEnv;
        bDelayedExpansion = item->bDelayedExpansion;
        bCmdlyCorrect = item->bCmdlyCorrect;

        free(item);
    }
}
