/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include "libDos9.h"
#include "Dos9_Core.h"
#include "../../config.h"

int bDelayedExpansion=FALSE;
int bUseFloats=FALSE;
int bEchoOn=TRUE;
int iErrorLevel=0;
int bIsScript;

#if !defined(DOS9_STATIC_CMDLYCORRECT)
int bCmdlyCorrect=FALSE;
#endif

int iMainThreadId;

int bAbortCommand=FALSE;
LPCOMMANDLIST lpclCommands;
LOCAL_VAR_BLOCK* lpvLocalVars;
/* use a distinct local block for command arguments

    %1-%9 : arguments
    %* : full line
    %+ : remaning args */
LOCAL_VAR_BLOCK* lpvArguments;
LPSTREAMSTACK lppsStreamStack;
COLOR colColor;

ENVBUF* lpeEnv;

int iInputD=0,
    iOutputD=0;

INPUT_FILE ifIn;

void(*pErrorHandler)(void)=NULL;

#ifdef WIN32
#define environ _environ
#else
extern char** environ;
#endif

char* lpInitVar[]= {
	"DOS9_VERSION", DOS9_VERSION,
	"DOS9_OS", DOS9_OS,
	NULL, NULL, /* DOS9_PATH is dinamically generated */
    "DOS9_OS_TYPE", DOS9_OS_TYPE,
    "NUL", NUL,
    "CON", CON,
    "COM", COM,
    "PRN", PRN,
    "LPT", LPT,
	NULL, NULL
};

jmp_buf jbBreak;
