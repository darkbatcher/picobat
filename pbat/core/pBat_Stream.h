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

#ifndef PBAT_MODULE_STREAM_H
#define PBAT_MODULE_STREAM_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libpBat.h>

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

#define PBAT_STDIN STDIN_FILENO
#define PBAT_STDOUT STDOUT_FILENO
#define PBAT_STDERR STDERR_FILENO

#define STREAM_MODE_ADD 0
#define STREAM_MODE_TRUNCATE 0xffffffff

#define PBAT_RESET_BUFFERING(fd, s) \
            if (isatty(fd)) \
                setvbuf(s, NULL, _IONBF, 0)

#define __PBAT_DUP_STD(fd, s) \
            if (dup2(fd, fileno(s)) == -1) \
                pBat_ShowErrorMessage(PBAT_UNABLE_DUPLICATE_FD \
                                        | PBAT_PRINT_C_ERROR, \
                                            __FILE__ "/PBAT_DUP_STD()", -1); \
            pBat_SetFdInheritance(fileno(s), 0);

#define PBAT_DUP_STD(fd, s) \
            fflush(s);\
            __PBAT_DUP_STD(fd, s) \
            PBAT_RESET_BUFFERING(fd, s)

#define PBAT_DUP_STDIN(fd, s) \
            setvbuf(s, NULL, _IONBF, 0); \
            __PBAT_DUP_STD(fd, s)

#define PBAT_XDUP(fd, s) \
            if (((fd) = dup(fileno(s))) == -1) \
                pBat_ShowErrorMessage(PBAT_UNABLE_DUPLICATE_FD \
                                        | PBAT_PRINT_C_ERROR, \
                                            __FILE__ "/PBAT_DUP_STD()", -1); \
            pBat_SetFdInheritance(fd, 0);

#define pBat_SetStreamStackLockState(stack, state) \
                                ((stack) ? (stack->lock = state) : 0)


#define pBat_GetStreamStackLockState(stack) ((stack) ? stack->lock : 1)

/* structure used to store the state of stream redirections */
typedef struct STREAMSTACK {
    int fd; /* the file descriptor that has been redirected */
    int oldfd; /* a duplicate of the previous fd associated with fd */
    int subst; /* a operation of file substitution if either 2>&1 or 1>&2 is used */
    int lock; /* a lock to prevent element from being popped */
    struct STREAMSTACK* previous;
} STREAMSTACK, *LPSTREAMSTACK;


#define PBAT_SUBST_FOUTPUT 0x1
#define PBAT_SUBST_FERROR 0x2

#define PBAT_APPLY_SUBST(state) \
    fOutput = (state & PBAT_SUBST_FOUTPUT) ? _fOutput : _fError; \
    fError = (state & PBAT_SUBST_FERROR) ? _fOutput : _fError

#define PBAT_GET_SUBST() \
    ((_fOutput == fOutput ) ? PBAT_SUBST_FOUTPUT : 0) \
                    | ( (fError == _fOutput) ? PBAT_SUBST_FERROR : 0)


/* initializes the stream stack */
STREAMSTACK* pBat_InitStreamStack(void);

/* frees the stream stack */
void pBat_FreeStreamStack(STREAMSTACK* stack);

/* Duplicate file based on a file name or a file descriptor */
STREAMSTACK* pBat_OpenOutput(STREAMSTACK* stack, char* name, int fd, int mode);
STREAMSTACK* pBat_OpenOutputD(STREAMSTACK* stack, int newfd, int fd);

/* Pop stream stack functions */
STREAMSTACK* pBat_PopStreamStack(STREAMSTACK* stack);
STREAMSTACK* pBat_PopStreamStackUntilLock(STREAMSTACK* stack);

void pBat_ApplyStreams(STREAMSTACK* stack);
void pBat_UnApplyStreams(STREAMSTACK* stack);
#endif
