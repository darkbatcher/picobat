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

#define S_IREAD _S_IREAD
#define O_IWRITE _S_IWRITE
#else

#include <unistd.h>
#define flushall()

#endif

#define DOS9_STDIN STDIN_FILENO
#define DOS9_STDOUT STDOUT_FILENO
#define DOS9_STDERR STDERR_FILENO

#define STREAM_MODE_ADD 0
#define STREAM_MODE_TRUNCATE 0xffffffff

#define DOS9MOD
#define MODSTREAM


typedef struct STREAMLVL {
	int iStandardDescriptors[3]; /* stores the standard outputs an output descriptors */
	int iFreeDescriptors[3];/* stores the descriptors to be feed during poping from stack */
	int iPipeIndicator; /* outdated */
	int iPopLock;
	//int iResetStdBuff;
} STREAMLVL,*LPSTREAMLVL;



typedef STACK STREAMSTACK,*LPSTREAMSTACK;


LPSTREAMSTACK Dos9_InitStreamStack(void);

void Dos9_FreeStreamStack(LPSTREAMSTACK lpssStream);

int Dos9_OpenOutput(LPSTREAMSTACK lpssStreamStack, char* lpName, int iDescriptor, int iMode);
int Dos9_OpenOutputD(LPSTREAMSTACK lpssStreamStack, int iNewDescriptor, int iDescriptor);

/* outdated */
int               Dos9_OpenPipe(LPSTREAMSTACK lpssStreamStack);
LPSTREAMSTACK     Dos9_Pipe(LPSTREAMSTACK lppsStreamStack);
/* outdated */

LPSTREAMSTACK     Dos9_PopStreamStack(LPSTREAMSTACK lppsStack);
LPSTREAMSTACK     Dos9_PopStreamStackUntilLock(LPSTREAMSTACK lppsStack);
LPSTREAMSTACK     Dos9_PushStreamStack(LPSTREAMSTACK lppsStack);

void              Dos9_DumpStreamStack(LPSTREAMSTACK lppsStack);



void  Dos9_SetStreamStackLockState(STREAMSTACK* lppsStack, int iState);
int Dos9_GetStreamStackLockState(STREAMSTACK* lppsStack);


int Dos9_GetDescriptors(int* Array);
void Dos9_FlushDescriptor(int iDescriptor, unsigned int iStd);
void Dos9_FlushDescriptors(int* Array);
void Dos9_CloseDescriptors(int* Array);
int Dos9_CreatePipe(int* Array);
int Dos9_FlushPipeDescriptors(int* Array, int iLastDescriptor, int iStdOut);
void Dos9_FlushStd(void);
void Dos9_SetStdBuffering(void);
LPSTREAMLVL Dos9_AllocStreamLvl(void);

LPSTREAMSTACK Dos9_PushStreamStackIfNotPipe(LPSTREAMSTACK lpssStreamStack);

#endif
