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

#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR
#endif

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
STREAMSTACK* pBat_OpenOutput(STREAMSTACK* stack, char* name, int fd, int mode)
{
    int newfd, fmode;
    STREAMSTACK* item;

    /* try to malloc a new stack item */
    if (!(item = malloc(sizeof(STREAMSTACK))))
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION
                                | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_OpenOutput", -1);


    item->previous = stack;

    switch (fd) {

    case PBAT_STDIN:
        fmode = O_RDONLY | O_BINARY;
        break;

    default:
        fmode = O_WRONLY | O_CREAT | O_APPEND | O_BINARY
            | ((mode & PARSED_STREAM_MODE_TRUNCATE) ? O_TRUNC : 0);

    }

    /* Note that cmd.exe implements a mechanism to create path
       that do not exist (eg. > a/b/c.txt creates a/b/ tree)
       upon opening of the file. This is not implemented yet. */

    /* Serialize this with pBat_RunFile() */
    PBAT_RUNFILE_LOCK();

    if ((newfd = open(name, fmode, S_IREAD | S_IWRITE)) == -1) {

        pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                name, 0);
        free(item);
        item = stack;
        goto error;

    }

    item->lock = 0;
    item->fd = fd;
    item->subst = PBAT_GET_SUBST();

    /* do not forget to update the standard thread specific streams */
    switch (fd) {

    case PBAT_STDIN:
        PBAT_XDUP(item->oldfd, fInput);
        PBAT_DUP_STDIN(newfd, fInput);
        close(newfd);
        break;

    case PBAT_STDOUT:
        PBAT_XDUP(item->oldfd, _fOutput);
        PBAT_DUP_STD(newfd, _fOutput);
        fOutput = _fOutput;
        close(newfd);
        break;

    case PBAT_STDERR:
        PBAT_XDUP(item->oldfd, _fError);
        PBAT_DUP_STD(newfd, _fError);
        fError = _fError;
        close(newfd);
        break;

    case PBAT_STDERR | PBAT_STDOUT:
        break;

    default:;
    }

error:
    PBAT_RUNFILE_RELEASE();
    return item;
}

STREAMSTACK* pBat_OpenOutputD(STREAMSTACK* stack, int newfd, int fd)
{
    STREAMSTACK* item;

    /* try to malloc a new stack item */
    if (!(item = malloc(sizeof(STREAMSTACK))))
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION
                                | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_OpenOutput", -1);


    item->previous = stack;

    item->lock = 0;
    item->fd = fd;
    item->subst = PBAT_GET_SUBST();

    /* Serialize this with pBat_RunFile() */
    PBAT_RUNFILE_LOCK();

    /* do not forget to update the standard thread specific streams */
    switch (fd) {

    case PBAT_STDIN:
        PBAT_XDUP(item->oldfd, fInput);
        PBAT_DUP_STDIN(newfd, fInput);
        break;

    case PBAT_STDOUT:
        PBAT_XDUP(item->oldfd, _fOutput);
        PBAT_DUP_STD(newfd, _fOutput);
        break;

    case PBAT_STDERR:
        PBAT_XDUP(item->oldfd, _fError);
        PBAT_DUP_STD(newfd, _fError);
        break;

    default:;
    }

    PBAT_RUNFILE_RELEASE();

    return item;
}

/* Pop stream stack functions */
STREAMSTACK* pBat_PopStreamStack(STREAMSTACK* stack)
{
    STREAMSTACK* item;

    /* Do not pop if locked or if stack is NULL*/
    if (stack == NULL || stack->lock)
        return stack;

    item = stack->previous;

    /* Serialize this with pBat_RunFile() */
    PBAT_RUNFILE_LOCK();

    switch (stack->fd) {

    case PBAT_STDIN:
        PBAT_DUP_STDIN(stack->oldfd, fInput);
        close(stack->oldfd);
        break;

    case PBAT_STDOUT:
        PBAT_DUP_STD(stack->oldfd, _fOutput);
        close(stack->oldfd);
        break;

    case PBAT_STDERR:
        PBAT_DUP_STD(stack->oldfd, _fError);
        close(stack->oldfd);
        break;

    default:;
    }

    PBAT_RUNFILE_RELEASE();

    PBAT_APPLY_SUBST(stack->subst);

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
    /* Serialize this with pBat_RunFile() */
    PBAT_RUNFILE_LOCK();

    /* simple fast method */
    PBAT_XDUP(fdStdin, stdin);
    PBAT_XDUP(fdStdout, stdout);
    PBAT_XDUP(fdStderr, stderr);

    PBAT_DUP_STDIN(fileno(fInput), stdin);
    PBAT_DUP_STD(fileno(fOutput), stdout);
    PBAT_DUP_STD(fileno(fError), stderr);

    PBAT_RUNFILE_RELEASE();
}

void pBat_UnApplyStreams(STREAMSTACK* stack)
{
    /* Serialize this with pBat_RunFile() */
    PBAT_RUNFILE_LOCK();

    PBAT_DUP_STDIN(fdStdin, stdin);
    PBAT_DUP_STD(fdStdout, stdout);
    PBAT_DUP_STD(fdStderr, stderr);

    PBAT_RUNFILE_RELEASE();

    close(fdStdout);
    close(fdStdin);
    close(fdStderr);
}
