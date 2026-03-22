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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include "pBat_Stream.h"
#include "pBat_Core.h"

#include "../errors/pBat_Errors.h"



//#define PBAT_DBG_MODE
#include "pBat_Debug.h"

/* initializes the stream stack */
STREAMSTACK* pBat_InitStreamStack(void)
{
    /* I guess, nothing to be done here, since there is no need
       for an initial copy of standard standard streams. This task is
       indeed accomplished by new output opening functions.

       Thus, just return NULL and let the good old others functions
       handle it;
    */
    return NULL;
}

/* frees the stream stack */
void pBat_FreeStreamStack(STREAMSTACK* stack)
{
    /* STREAMSTACK* tmp; */

    /* Here, speed is not critical at all, so do things
       consciously, hopefully, it is simple, normally no
       file redirection is on, so just close descriptors */

    while (stack) {

        if (pBat_GetStreamStackLockState(stack))
            pBat_SetStreamStackLockState(stack, 0);

        stack = pBat_PopStreamStack(stack);

    }
}

/* Duplicate file based on a file name or a file descriptor */
STREAMSTACK* pBat_OpenOutput(STREAMSTACK* stack, char* name, int mode)
{
    FILE* f;
    char *fmode;

    switch (mode & (PBAT_STDIN | PBAT_STDOUT | PBAT_STDERR
                | PBAT_STREAM_MODE_ADD | PBAT_STREAM_MODE_TRUNCATE)) {

    case PBAT_STDOUT | PBAT_STREAM_MODE_ADD:
    case PBAT_STDERR | PBAT_STREAM_MODE_ADD:
        fmode = "ab";
        break;

    case PBAT_STDOUT | PBAT_STREAM_MODE_TRUNCATE:
    case PBAT_STDERR | PBAT_STREAM_MODE_TRUNCATE:
        /* truncate mode */
        fmode = "wb";
        break;

    default:
        /* we are redirecting stdin */
        fmode = "rb";

    }

    /* Note that cmd.exe implements a mechanism to create path
       that do not exist (eg. > a/b/c.txt creates a/b/ tree)
       upon opening of the file. This is not implemented yet. */

    /* Serialize this with pBat_RunFile() */
    PBAT_RUNFILE_LOCK();

    if ((f = fopen(name, fmode)) == NULL) {
        pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                name, 0);
        PBAT_RUNFILE_RELEASE();
        return NULL;

    }

    /* Set the fd to non inheritable */
    pBat_SetStdInheritance(f, 0);

    PBAT_RUNFILE_RELEASE();

    return  pBat_OpenOutputF(stack, f, mode);
}

STREAMSTACK* pBat_OpenOutputF(STREAMSTACK* stack, FILE* f, int mode)
{
    STREAMSTACK* item;

     /* try to malloc a new stack item */
    if (!(item = malloc(sizeof(STREAMSTACK))))
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION
                                | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_OpenOutput", -1);

    item->previous = stack;
    item->mode = mode;

    /* save old file and perform the swap */
    switch (mode & (PBAT_STDIN | PBAT_STDOUT | PBAT_STDERR)) {

    case PBAT_STDIN:
        item->old = fInput;
        fInput = f;
        break;

    case PBAT_STDOUT:
        item->old = fOutput;
        fOutput = f;
        break;

    case PBAT_STDERR:
        item->old = fError;
        fError = f;
    }

    /* check if this is also either 1>&2 or 2>&1 */
    switch (mode & (PBAT_STDOUT | PBAT_STDERR | PBAT_STREAM_SUBST_FERROR
                       | PBAT_STREAM_SUBST_FOUTPUT)) {

    /* fOutput redirected to fError */
    case PBAT_STDOUT | PBAT_STREAM_SUBST_FERROR:
        item->subst = fError;
        fError = f;
        break;

    /* fError redirected to fOutput */
    case PBAT_STDERR | PBAT_STREAM_SUBST_FOUTPUT:
        item->subst = fOutput;
        fOutput = f;
        break;

    }

    return item;
}

/* Pop stream stack functions */
STREAMSTACK* pBat_PopStreamStack(STREAMSTACK* stack)
{
    STREAMSTACK* item;

    /* Do not pop if locked or if stack is NULL*/
    if (stack == NULL
        || stack->mode & PBAT_STREAM_LOCKED)
        return stack;

    item = stack->previous;

    /* try to reverse the changes made when item was pushed,
       close substituted stream and replace it by its save*/
    switch (stack->mode & (PBAT_STDIN | PBAT_STDOUT | PBAT_STDERR)) {

    case PBAT_STDIN:
        fclose(fInput);
        fInput = stack->old;
        break;

    case PBAT_STDOUT:
        fclose(fOutput);
        fOutput = stack->old;
        break;

    case PBAT_STDERR:
        fclose(fError);
        fError = stack->old;
    }

    /* check if this is also either 1>&2 or 2>&1 */
    switch (stack->mode & (PBAT_STDOUT | PBAT_STDERR | PBAT_STREAM_SUBST_FERROR
                       | PBAT_STREAM_SUBST_FOUTPUT)) {

    /* fOutput redirected to fError */
    case PBAT_STDOUT | PBAT_STREAM_SUBST_FERROR:
        fError = stack->subst;
        break;

    /* fError redirected to fOutput */
    case PBAT_STDERR | PBAT_STREAM_SUBST_FOUTPUT:
        fOutput = stack->subst;
    }

    free(stack);

    return item;
}

STREAMSTACK* pBat_PopStreamStackUntilLock(STREAMSTACK* stack)
{
    while(!pBat_GetStreamStackLockState(stack))
        stack = pBat_PopStreamStack(stack);

    return stack;
}

void pBat_ApplyStreams(STREAMSTACK* stack)
{

    /* simple fast method */
    PBAT_XDUP(fdStdin, stdin);
    PBAT_XDUP(fdStdout, stdout);
    PBAT_XDUP(fdStderr, stderr);

    PBAT_DUP_STDIN(fileno(fInput), stdin);
    PBAT_DUP_STD(fileno(fOutput), stdout);
    PBAT_DUP_STD(fileno(fError), stderr);

}

void pBat_UnApplyStreams(STREAMSTACK* stack)
{

    PBAT_DUP_STDIN(fdStdin, stdin);
    PBAT_DUP_STD(fdStdout, stdout);
    PBAT_DUP_STD(fdStderr, stderr);

    close(fdStdout);
    close(fdStdin);
    close(fdStderr);
}
