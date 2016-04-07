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

LIBDOS9 int Dos9_LibInit(void)
{
    /* instanciation routine */

    if (_Dos9_Estr_Init() != 0)
        return -1;

    if (_Dos9_Thread_Init() != 0)
        return -1;

    return 0;

}

LIBDOS9 void Dos9_LibClose(void)
{

    #ifdef LIBDOS9_WITH_THREADS
    Dos9_EsFree(NULL);
        /* free all remaining string buffers */

    #endif

    _Dos9_Estr_Close();
    _Dos9_Thread_Close();

}
