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

#ifndef DOS9_MODULE_STREAM_H
#define DOS9_MODULE_STREAM_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libDos9.h>

#ifdef WIN32
/* used for compatibility purpose */
#include <io.h>
#define pipe(a,b,c) _pipe(a,b,c)
#define flushall() _flushall()
#define dup(a) _dup(a)
#define dup2(a,b) _dup2(a,b)

#define O_WRONLY _O_WRONLY
#define O_RDONLY _O_RDONLY
#define O_TRUNC _O_TRUNC

#ifndef S_IRUSR
#define S_IRUSR _S_IREAD
#endif /* S_IRUSR */

#ifndef S_IWUSR
#define S_IWUSR _S_IWRITE
#endif /* S_IWUSR */
#else /* WIN32 */
#include <unistd.h>
#define flushall()
#endif /* WIN32 */

#define DOS9_STDIN STDIN_FILENO
#define DOS9_STDOUT STDOUT_FILENO
#define DOS9_STDERR STDERR_FILENO

#define STREAM_MODE_ADD 0
#define STREAM_MODE_TRUNCATE 0xffffffff

#define DOS9_RESET_BUFFERING(fd, s) \
            if (isatty(fd)) \
                setvbuf(s, NULL, _IONBF, 0)

#define __DOS9_DUP_STD(fd, s) \
            fflush(s);\
            if (dup2(fd, fileno(s)) == -1) \
                Dos9_ShowErrorMessage(DOS9_UNABLE_DUPLICATE_FD \
                                        | DOS9_PRINT_C_ERROR, \
                                            __FILE__ "/DOS9_DUP_STD()", -1); \
            Dos9_SetFdInheritance(fileno(s), 0);

#define DOS9_DUP_STD(fd, s) \
            __DOS9_DUP_STD(fd, s) \
            DOS9_RESET_BUFFERING(fd, s)

#define DOS9_DUP_STDIN(fd, s) \
            __DOS9_DUP_STD(fd, s)

#define DOS9_XDUP(fd, s) \
            if (((fd) = dup(fileno(s))) == -1) \
                Dos9_ShowErrorMessage(DOS9_UNABLE_DUPLICATE_FD \
                                        | DOS9_PRINT_C_ERROR, \
                                            __FILE__ "/DOS9_DUP_STD()", -1); \
            Dos9_SetFdInheritance(fd, 0);

#define Dos9_SetStreamStackLockState(stack, state) \
                                ((stack) ? (stack->lock = state) : 0)


#define Dos9_GetStreamStackLockState(stack) ((stack) ? stack->lock : 1)

/* structure used to store the state of stream redirections */
typedef struct STREAMSTACK {
    int fd; /* the file descriptor that has been redirected */
    int oldfd; /* a duplicate of the previous fd associated with fd */
    int subst; /* a operation of file substitution if either 2>&1 or 1>&2 is used */
    int lock; /* a lock to prevent element from being popped */
    struct STREAMSTACK* previous;
} STREAMSTACK, *LPSTREAMSTACK;


#define DOS9_SUBST_FOUTPUT 0x1
#define DOS9_SUBST_FERROR 0x2

#define DOS9_APPLY_SUBST(state) \
    fOutput = (state & DOS9_SUBST_FOUTPUT) ? _fOutput : _fError; \
    fError = (state & DOS9_SUBST_FERROR) ? _fOutput : _fError

#define DOS9_GET_SUBST() \
    ((_fOutput == fOutput ) ? DOS9_SUBST_FOUTPUT : 0) \
                    | ( (fError == _fOutput) ? DOS9_SUBST_FERROR : 0)


/* initializes the stream stack */
STREAMSTACK* Dos9_InitStreamStack(void);

/* frees the stream stack */
void Dos9_FreeStreamStack(STREAMSTACK* stack);

/* Duplicate file based on a file name or a file descriptor */
STREAMSTACK* Dos9_OpenOutput(STREAMSTACK* stack, char* name, int fd, int mode);
STREAMSTACK* Dos9_OpenOutputD(STREAMSTACK* stack, int newfd, int fd);

/* Pop stream stack functions */
STREAMSTACK* Dos9_PopStreamStack(STREAMSTACK* stack);
STREAMSTACK* Dos9_PopStreamStackUntilLock(STREAMSTACK* stack);

void Dos9_ApplyStreams(STREAMSTACK* stack);
void Dos9_UnApplyStreams(STREAMSTACK* stack);
#endif
