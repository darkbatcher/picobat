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
 */

#include "libDos9.h"
#include "Dos9_Core.h"

int bDelayedExpansion=FALSE;
int bUseFloats=FALSE;
int bCmdlyCorrect=FALSE;
int bEchoOn=TRUE;
int iErrorLevel=0;

int bAbortCommand=FALSE;
LPCOMMANDLIST lpclCommands;
LOCAL_VAR_BLOCK* lpvLocalVars;
LPSTREAMSTACK lppsStreamStack;
COLOR colColor;

int iInputD=0,
    iOutputD=0;

INPUT_FILE ifIn;

void(*pErrorHandler)(void)=NULL;

#ifdef WIN32
    #define environ _environ
#else
    extern char** environ;
#endif

char* lpInitVar[]={
    "DOS9_VERSION=" DOS9_VERSION,
#ifdef WIN32
    "DOS9_OS=WINDOWS",
#elif defined _POSIX_C_SOURCE
    "DOS9_OS=*NIX",
#else
    "DOS9_OS=UNKNOWN"
#endif
    NULL,
    NULL
};
