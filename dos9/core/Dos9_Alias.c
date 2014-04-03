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
#include <libDos9.h>

#include "Dos9_Core.h"

void Dos9_ExpandAlias(ESTR* lpRet, char* lpCh, char* lpExp)
{
    ESTR* lpEsRet=Dos9_EsInit();

    Dos9_ReMapCommandInfo(lpclCommands);

    Dos9_EsCpy(lpEsRet, lpExp);
    Dos9_EsCat(lpEsRet, " ");
    Dos9_EsCat(lpEsRet, lpCh);

    Dos9_EsCpyE(lpRet, lpEsRet);
    Dos9_EsFree(lpEsRet);
}
