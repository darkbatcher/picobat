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
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "pBat_Clone.h"
#include "pBat_Core.h"

#include "../errors/pBat_Errors.h"

//#define PBAT_DBG_MODE
#include "pBat_Debug.h"

/* Duplicate the current instance of pBat (much like fork() in unix systems) */
THREAD pBat_CloneInstance(void(*func)(void*), void* arg)
{
    struct clone_data_t* data;
    THREAD th;

    if ((data = malloc(sizeof(struct clone_data_t))) == NULL)
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_CloneInstance()", -1);

    pBat_DuplicateData(data);
    data->fn = func;
    data->arg = arg;

    if (pBat_BeginThread(&th, pBat_CloneTrampoline, 0, data))
        pBat_ShowErrorMessage(PBAT_FAILED_FORK | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_CloneInstance()", -1);

    return th;
}

void* pBat_CloneTrampoline(void* data)
{
    struct clone_data_t* cloned = data;
    void(*func)(void*);
    void* arg;

    bDelayedExpansion = cloned->bDelayedExpansion;
    bEchoOn = cloned->bEchoOn;
    iErrorLevel = cloned->iErrorLevel;
    bIsScript = cloned->bIsScript;
    bCmdlyCorrect = cloned->bCmdlyCorrect;
    bAbortCommand = cloned->bAbortCommand;
    lpclCommands = cloned->lpclCommands;
    lpvLocalVars = cloned->lpvLocalVars;
    lpvArguments = cloned->lpvArguments;
    dsDirStack = cloned->dsDirStack;
    lppsStreamStack = cloned->lppsStreamStack;
    colColor = cloned->colColor;
    lpeEnv = cloned->lpeEnv;
    strncpy(ifIn.lpFileName, cloned->ifIn.lpFileName, FILENAME_MAX);
    strncpy(lpCurrentDir, cloned->lpCurrentDir, FILENAME_MAX);
    ifIn.bEof = cloned->ifIn.bEof;
    ifIn.iPos = cloned->ifIn.iPos;
    *(ifIn.batch.name) = '\0';
    fInput = cloned->fInput;
    fOutput = cloned->fOutput;
    fError = cloned->fError;
    _fError = fError;
    _fOutput = fOutput;

    lpAltPromptString = NULL;

    func = cloned->fn;
    arg = cloned->arg;

    free(data);

    func(arg);

    pBat_Exit();

    pBat_EndThread((void*)iErrorLevel);

    return (void*)iErrorLevel;
}

/* Duplicate pBat internal structures */
int pBat_DuplicateData(struct clone_data_t* data)
{
    int fd;

    data->bDelayedExpansion = bDelayedExpansion;
    data->bEchoOn = bEchoOn;
    data->iErrorLevel = iErrorLevel;
    data->bIsScript = bIsScript;
    data->bCmdlyCorrect = bCmdlyCorrect;
    data->bAbortCommand = bAbortCommand;

    if ((data->lpclCommands = pBat_DuplicateCommandList(lpclCommands)) == NULL)
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                               __FILE__ "/pBat_DuplicateData()", -1);

    data->lpvLocalVars = pBat_DuplicateLocalVar(lpvLocalVars);
    data->lpvArguments = pBat_DuplicateLocalVar(lpvArguments);

    pBat_DirStackCopy(&data->dsDirStack);

    data->lppsStreamStack = NULL;
    data->colColor = colColor;
    data->lpeEnv = pBat_EnvDup(lpeEnv);
    strncpy(data->lpCurrentDir, lpCurrentDir, FILENAME_MAX);
    strncpy(data->ifIn.lpFileName, ifIn.lpFileName, FILENAME_MAX);
    data->ifIn.bEof = ifIn.bEof;
    data->ifIn.iPos = ifIn.iPos;

#define DUPLICATE_STREAM(dest, s, mode) \
    if ((fd = dup(fileno(s))) == -1) \
        pBat_ShowErrorMessage(PBAT_UNABLE_DUPLICATE_FD \
                               | PBAT_PRINT_C_ERROR, \
                               __FILE__ "/pBat_DuplicateData()", -1); \
    \
    if ((dest = fdopen(fd, mode)) == NULL) \
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION \
                                | PBAT_PRINT_C_ERROR, \
                                __FILE__ "/pBat_DuplicateData()", -1)

    DUPLICATE_STREAM(data->fInput, fInput, "r");
    DUPLICATE_STREAM(data->fOutput, fOutput, "w");
    DUPLICATE_STREAM(data->fError, fError, "w");

    return 0;
}
