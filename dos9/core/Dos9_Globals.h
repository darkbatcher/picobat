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

#ifndef DOS9_GLOBALS_H
#define DOS9_GLOBALS_H

#include "../../config.h"

#ifdef _POSIX_C_SOURCE
extern char** environ;
#endif

extern int bAbortCommand;
extern int bDelayedExpansion;
extern int bUseFloats;

#if !defined(DOS9_STATIC_CMDLYCORRECT)
extern int bCmdlyCorrect;
#else
#define bCmdlyCorrect DOS9_STATIC_CMDLYCORRECT
#endif

extern int bEchoOn;
extern int iErrorLevel;
extern LPCOMMANDLIST lpclCommands;
extern LOCAL_VAR_BLOCK* lpvLocalVars;
extern LPSTREAMSTACK lppsStreamStack;
extern COLOR colColor;
extern INPUT_FILE ifIn;

extern ENVBUF* lpeEnv;

extern char* lpInitVar[];

extern int iInputD,
       iOutputD;
extern void(*pErrorHandler)(void);

#endif
