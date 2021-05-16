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

int iMainThreadId;
char* lpInitVar[]= {
	"DOS9_VERSION", DOS9_VERSION,
	"DOS9_OS", DOS9_OS,
	NULL, NULL, /* DOS9_PATH is dinamically generated */
    "DOS9_OS_TYPE", DOS9_OS_TYPE,
    "DOS9_START_SCRIPT", START_SCRIPT,
    "PROMPT","$P$G",
    "NUL", NUL,
    "CON", CON,
    "COM", COM,
    "PRN", PRN,
    "LPT", LPT,
	NULL, NULL
};

void(*pErrorHandler)(void)=NULL;
int fdStdin;
int fdStdout;
int fdStderr;
MUTEX mThreadLock;
MUTEX mSetLock; /* a lock to serialize calls to yacc-generated parsers */
MUTEX mRunFile; /* a lock to serialize calls to create process */
MUTEX mEchoLock; /* a lock to serialize calls to echo */
MUTEX mModLock; /* a lock to serialize calls to mod */
char lpDos9Path[FILENAME_MAX]; /* A path to the directory of the Dos9 executable */
char lpDos9Exec[FILENAME_MAX]; /* A path to the Dos9 executable */

__thread int bDelayedExpansion=TRUE;
__thread int bEchoOn=TRUE;

/* call exit or endthread uppon exit*/
__thread int bIgnoreExit=FALSE;

/* Current errorlevel value */
__thread int iErrorLevel=0;
__thread int bIsScript;
__thread int bCmdlyCorrect=FALSE;

__thread int bAbortCommand=FALSE;
__thread LPCOMMANDLIST lpclCommands;
__thread LOCAL_VAR_BLOCK* lpvLocalVars;
    /* use a distinct local block for command arguments

        %1-%9 : arguments
        %* : full line
        %+ : remaning args */
__thread LOCAL_VAR_BLOCK* lpvArguments;
__thread LPSTREAMSTACK lppsStreamStack;
__thread struct dirstack_t dsDirStack; /* current directory stack */

__thread COLOR colColor=DOS9_COLOR_DEFAULT;
__thread FILE* fInput; /* current thread input stream */
__thread FILE *fOutput, *_fOutput; /* current thread output stream */
__thread FILE *fError, *_fError; /* current thread error stream */
/* Note : the underscore prefixed version are internally used to
   handle output substitution (eg. 2>&1) to be used as backup and
   so prevent unnecessary duplication of files */

__thread ENVBUF* lpeEnv;
__thread ENVSTACK* lpesEnv = NULL;
__thread INPUT_FILE ifIn;
__thread char lpCurrentDir[FILENAME_MAX];

__thread char* lpAltPromptString = NULL;

#ifdef WIN32
#define environ _environ
#else
extern char** environ;
MUTEX mLineNoise;
#endif
