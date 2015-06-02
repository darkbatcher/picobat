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

#ifndef DOS9_CORE_RUN_H
#define DOS9_CORE_RUN_H

#ifdef WIN32
#include <process.h>
#define DOS9_SPAWN_CAST const char* const*
#else
#include <spawn.h>
#define DOS9_SPAWN_CAST char* const*
#define spawnvp(a,b,c) posix_spawnp(NULL, b, NULL, NULL, c, environ)
#endif

#ifndef P_WAIT
#define P_WAIT _P_WAIT
#endif

#include <libDos9.h>
#include "Dos9_Core.h"

/* applies redirections */
int Dos9_ExecOutput(PARSED_STREAM_START* lppssStart);

/* applies conditional operators */
int Dos9_ExecOperators(PARSED_STREAM** lppsStream);

int Dos9_RunCommand(ESTR* lpCommand); // the fucbtions that run every command
int Dos9_RunLine(ESTR* lpLine);
int Dos9_RunBlock(BLOCKINFO* lpbkInfo); // the function that run blocks
int Dos9_RunBatch(INPUT_FILE* pIn); // the function that runs the batch
int Dos9_RunExternalCommand(char* lpCommandLine);
int Dos9_RunExternalFile(char* lpFileName, char** lpArguments);

int Dos9_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments);

#ifndef WIN32

void Dos9_SigHandlerBreak(int c);

#elif defined(WIN32)

BOOL WINAPI Dos9_SigHandler(DWORD dwCtrlType);

#endif
#endif
