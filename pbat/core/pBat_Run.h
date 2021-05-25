/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#ifndef PBAT_CORE_RUN_H
#define PBAT_CORE_RUN_H

#ifdef WIN32
#include <process.h>
#define PBAT_SPAWN_CAST const char* const*
#else
#define PBAT_SPAWN_CAST char* const*
#endif

#ifndef P_WAIT
#define P_WAIT _P_WAIT
#endif

#include <libpBat.h>
#include "pBat_Core.h"

struct pipe_launch_data_t {
    int fdin;
    int fdout;
    ESTR* str;
    PARSED_STREAM* stream;
};

/* applies redirections */
int pBat_ExecOutput(PARSED_STREAM* lppssStart);

void pBat_LaunchPipe(struct pipe_launch_data_t* infos);

/* applies conditional operators */
int pBat_ExecOperators(PARSED_LINE** lppsStream);

/* Run a command */
int pBat_RunCommand(ESTR* lpCommand, PARSED_LINE** lpplLine);

/* Run a line */
int pBat_RunLine(ESTR* lpLine);

/* Run a line already parsed */
void pBat_RunParsedLine(PARSED_LINE* lppsLine);

/* Run the content of a block */
int pBat_RunBlock(BLOCKINFO* lpbkInfo);

/* Run a batch file */
int pBat_RunBatch(INPUT_FILE* pIn);
int pBat_RunExternalCommand(char* lpCommandLine, int* error);
int pBat_RunExternalFile(char* lpFileName, char* lpFullLine, char** lpArguments);

int pBat_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments);

PARSED_LINE* pBat_LookAHeadMakeParsedLine(BLOCKINFO* block,PARSED_LINE* lookahead);

#ifndef WIN32

void pBat_SigHandlerBreak(int c);

#elif defined(WIN32)

BOOL WINAPI pBat_SigHandler(DWORD dwCtrlType);

#endif
#endif
