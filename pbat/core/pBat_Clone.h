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

#ifndef PBAT_CLONE_H
#define PBAT_CLONE_H

#include <libpBat.h>
#include "pBat_Core.h"

struct clone_data_t {
    int bDelayedExpansion;
    int bEchoOn;
    int iErrorLevel;
    int bIsScript;
    int bCmdlyCorrect;
    int bAbortCommand;
    LPCOMMANDLIST lpclCommands;
    LOCAL_VAR_BLOCK* lpvLocalVars;
    LOCAL_VAR_BLOCK* lpvArguments;
    LPSTREAMSTACK lppsStreamStack;
    struct dirstack_t dsDirStack;
    COLOR colColor;
    FILE* fInput; /* current thread input stream */
    FILE* fOutput; /* current thread output stream */
    FILE* fError; /* current thread error stream */
    ENVBUF* lpeEnv;
    INPUT_FILE ifIn;
    char lpCurrentDir[FILENAME_MAX];
    void(*fn)(void*);
    void* arg;
};

/* Clone this instance ! */
THREAD pBat_CloneInstance(void(*func)(void*), void* data);
int pBat_DuplicateData(struct clone_data_t* data);

void* pBat_CloneTrampoline(void* data);
#endif
