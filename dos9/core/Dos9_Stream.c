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

#include "Dos9_Stream.h"
#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

/* initializes the stream stack */
STREAMSTACK* Dos9_InitStreamStack(void)
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
void Dos9_FreeStreamStack(STREAMSTACK* stack)
{
    /* STREAMSTACK* tmp; */

    /* Here, speed is not critical at all, so do things
       consciously, hopefully, it is simple, normally no
       file redirection is on, so just close descriptors */

    while (stack) {

        if (Dos9_GetStreamStackLockState(stack))
            Dos9_SetStreamStackLockState(stack, 0);

        stack = Dos9_PopStreamStack(stack);

    }
}

/* Duplicate file based on a file name or a file descriptor */
STREAMSTACK* Dos9_OpenOutput(STREAMSTACK* stack, char* name, int fd, int mode)
{
    int newfd, fmode;
    STREAMSTACK* item;

    /* try to malloc a new stack item */
    if (!(item = malloc(sizeof(STREAMSTACK))))
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_OpenOutput", -1);


    item->previous = stack;

    switch (fd) {

    case DOS9_STDIN:
        fmode = O_RDONLY | O_BINARY;
        break;

    default:
        fmode = O_WRONLY | O_CREAT | O_APPEND | O_BINARY
            | ((mode & PARSED_STREAM_MODE_TRUNCATE) ? O_TRUNC : 0);

    }

    /* Note that cmd.exe implements a mechanism to create path that do not exist
       upon opening of the file. This is not implemented yet. */

    if ((newfd = open(name, fmode, S_IREAD | S_IWRITE)) == -1) {

        Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                name, 0);
        free(item);
        return stack;

    }

    Dos9_SetFdInheritance(newfd, 0); /* do not inherit this file descriptor
                                        the only inheritable file descriptors
                                        should be standard fds */

    item->lock = 0;
    item->fd = fd;
    item->subst = DOS9_GET_SUBST();

    /* do not forget to update the standard thread specific streams */
    switch (fd) {

    case DOS9_STDIN:
        DOS9_XDUP(item->oldfd, fInput);
        DOS9_DUP_STDIN(newfd, fInput);
        close(newfd);
        break;

    case DOS9_STDOUT:
        DOS9_XDUP(item->oldfd, _fOutput);
        DOS9_DUP_STD(newfd, _fOutput);
        fOutput = _fOutput;
        close(newfd);
        break;

    case DOS9_STDERR:
        DOS9_XDUP(item->oldfd, _fError);
        DOS9_DUP_STD(newfd, _fError);
        fError = _fError;
        close(newfd);
        break;

    case DOS9_STDERR | DOS9_STDOUT:
        break;

    default:;
    }

    return item;
}

STREAMSTACK* Dos9_OpenOutputD(STREAMSTACK* stack, int newfd, int fd)
{
    STREAMSTACK* item;

    /* try to malloc a new stack item */
    if (!(item = malloc(sizeof(STREAMSTACK))))
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_OpenOutput", -1);


    item->previous = stack;

    if (newfd != -1)
        Dos9_SetFdInheritance(newfd, 0); /* do not inherit this file descriptor
                                        the only inheritable file descriptors
                                        should be standard fds */

    item->lock = 0;
    item->fd = fd;
    item->subst = DOS9_GET_SUBST();

    /* do not forget to update the standard thread specific streams */
    switch (fd) {

    case DOS9_STDIN:
        DOS9_XDUP(item->oldfd, fInput);
        DOS9_DUP_STDIN(newfd, fInput);
        break;

    case DOS9_STDOUT:
        DOS9_XDUP(item->oldfd, _fOutput);
        DOS9_DUP_STD(newfd, _fOutput);
        break;

    case DOS9_STDERR:
        DOS9_XDUP(item->oldfd, _fError);
        DOS9_DUP_STD(newfd, _fError);
        break;

    default:;
    }

    return item;
}

/* Pop stream stack functions */
STREAMSTACK* Dos9_PopStreamStack(STREAMSTACK* stack)
{
    STREAMSTACK* item;

    /* Do not pop if locked or if stack is NULL*/
    if (stack == NULL || stack->lock)
        return stack;

    item = stack->previous;

    switch (stack->fd) {

    case DOS9_STDIN:
        DOS9_DUP_STDIN(stack->oldfd, fInput);
        close(stack->oldfd);
        break;

    case DOS9_STDOUT:
        DOS9_DUP_STD(stack->oldfd, _fOutput);
        close(stack->oldfd);
        break;

    case DOS9_STDERR:
        DOS9_DUP_STD(stack->oldfd, _fError);
        close(stack->oldfd);
        break;

    default:;
    }

    DOS9_APPLY_SUBST(stack->subst);

    free(stack);

    return item;
}

STREAMSTACK* Dos9_PopStreamStackUntilLock(STREAMSTACK* stack)
{
    while(!Dos9_GetStreamStackLockState(stack))
        stack = Dos9_PopStreamStack(stack);

    return stack;
}

void Dos9_ApplyStreams(STREAMSTACK* stack)
{
    /* simple fast method */
    DOS9_XDUP(fdStdin, stdin);
    DOS9_XDUP(fdStdout, stdout);
    DOS9_XDUP(fdStderr, stderr);

    DOS9_DUP_STDIN(fileno(fInput), stdin);
    DOS9_DUP_STD(fileno(fOutput), stdout);
    DOS9_DUP_STD(fileno(fError), stderr);
}

void Dos9_UnApplyStreams(STREAMSTACK* stack)
{
    DOS9_DUP_STDIN(fdStdin, stdin);
    DOS9_DUP_STD(fdStdout, stdout);
    DOS9_DUP_STD(fdStderr, stderr);

    close(fdStderr);
    close(fdStdin);
    close(fdStderr);
}
