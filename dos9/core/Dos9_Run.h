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

struct pipe_launch_data_t {
    int fd;
    ESTR* str;
    PARSED_STREAM* stream;
};

/* applies redirections */
int Dos9_ExecOutput(PARSED_STREAM* lppssStart);

void Dos9_LaunchPipe(struct pipe_launch_data_t* infos);

/* applies conditional operators */
int Dos9_ExecOperators(PARSED_LINE** lppsStream);

/* Run a command */
int Dos9_RunCommand(ESTR* lpCommand, PARSED_LINE** lpplLine);

/* Run a line */
int Dos9_RunLine(ESTR* lpLine);

/* Run a line already parsed */
void Dos9_RunParsedLine(PARSED_LINE* lppsLine);

/* Run the content of a block */
int Dos9_RunBlock(BLOCKINFO* lpbkInfo);

/* Run a batch file */
int Dos9_RunBatch(INPUT_FILE* pIn);
int Dos9_RunExternalCommand(char* lpCommandLine, int* error);
int Dos9_RunExternalFile(char* lpFileName, char* lpFullLine, char** lpArguments);

int Dos9_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments);

PARSED_LINE* Dos9_LookAHeadMakeParsedLine(BLOCKINFO* block,PARSED_LINE* lookahead);

#ifndef WIN32

void Dos9_SigHandlerBreak(int c);

#elif defined(WIN32)

BOOL WINAPI Dos9_SigHandler(DWORD dwCtrlType);

#endif
#endif
