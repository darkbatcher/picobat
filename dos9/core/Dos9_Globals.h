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

#ifndef DOS9_GLOBALS_H
#define DOS9_GLOBALS_H

#include <setjmp.h>
#include <stdio.h>
#include <libDos9.h>

#include "../../config.h"

#ifndef WIN32
extern char** environ;
#endif

/* static variable for initialization */
extern int iMainThreadId; /* thread id of the main thread */
extern void(*pErrorHandler)(void); /* error handler */
extern char* lpInitVar[]; /* list of variable for initialization */
extern MUTEX mThreadLock; /* a mutex for single threaded parts */
extern MUTEX mSetILock; /* a mutex for set (integers) */
extern MUTEX mSetFLock; /* a mutex for set (floats) */

extern int fdStdin; /* temporary storage for streams */
extern int fdStdout;
extern int fdStderr;


/* Current state of the interpreter associated to this
   thread */
extern __thread int bAbortCommand; /* abort the command (0: no , 1: jump to next,
                                    -1 : move to upper execution level) */

#define DOS9_ABORT_EXECUTION_LEVEL -1
#define DOS9_ABORT_COMMAND_LINE 1
#define DOS9_ABORT_COMMAND_BLOCK 2

extern __thread int bIgnoreExit; /* ignore exit */
extern __thread int bDelayedExpansion; /* state of the delayed expansion */
extern __thread int bCmdlyCorrect; /* state of cmdly correct interface */
extern __thread int bUseFloats; /* state of use of floats */
extern __thread int bIsScript; /* are we running a script ? */
extern __thread int bEchoOn; /* is echo on ? */
extern __thread int iErrorLevel; /* errorlevel state */
extern __thread LPCOMMANDLIST lpclCommands; /* binary tree of commands */
extern __thread LOCAL_VAR_BLOCK* lpvLocalVars; /* local variables array */
extern __thread LOCAL_VAR_BLOCK* lpvArguments; /* arguments array */
extern __thread LPSTREAMSTACK lppsStreamStack; /* status associated with streams */
extern __thread COLOR colColor; /* current command prompt colors */
extern __thread INPUT_FILE ifIn; /* current parsed script */
extern __thread ENVBUF* lpeEnv; /* environment variables local to threads */
extern __thread FILE *fInput; /* current thread input stream */
extern __thread FILE *fOutput, *_fOutput; /* current thread output stream */
extern __thread FILE *fError, *_fError; /* current thread error stream */
extern __thread ENVSTACK* lpesEnv;
extern __thread char lpCurrentDir[FILENAME_MAX]; /* current path */

#endif
