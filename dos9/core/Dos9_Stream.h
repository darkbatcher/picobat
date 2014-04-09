/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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

/** \file Dos9_Stream.h
    \brief A header that contains definitions of stream modules fonctions

*/


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



/** \defgroup DESCRIPTORS_CONSTANT Descriptors constants
    \{
*/
/** the descriptor number of stdout */
#define DOS9_STDIN STDIN_FILENO
/** The descriptor number of stdout */
#define DOS9_STDOUT STDOUT_FILENO
/** The descriptor number of stderr */
#define DOS9_STDERR STDERR_FILENO

/** \} */

/** \defgroup MODE_CONSTANT Redirection mode monstants
    \{
*/
/** The content redirected will be added at the end of the file */
#define STREAM_MODE_ADD 0
/** The file will be truncated before the content redirected will be add to the file */
#define STREAM_MODE_TRUNCATE 0xffffffff
/** \} */

#define DOS9MOD
#define MODSTREAM

/** \struct STREAMLVL
    \brief  A structure used to store the information about the current stream environment
*/
typedef struct STREAMLVL {
    int iStandardDescriptors[3]; /**< stores the standard outputs an output descriptors */
    int iFreeDescriptors[3];/**< stores the descriptors to be feed during poping from stack */
    int iPipeIndicator;
    int iPopLock;
    int iResetStdBuff;
 } STREAMLVL,*LPSTREAMLVL;

/** \typedef STACK STREAMSTACK,*LPSTREAMSTACK
    \brief used to make an stack of stream environment
*/

typedef STACK STREAMSTACK,*LPSTREAMSTACK;

/** \brief Initializes and returns a LPSTREAMSTACK structure
    \return Returns a pointer to the allocated STREAMSTACK. If the function fails, it returns NULL
*/
 LPSTREAMSTACK     Dos9_InitStreamStack(void);

 void              Dos9_FreeStreamStack(LPSTREAMSTACK lpssStream);

/** \brief Open a redirection of a sstandard input or output
    \param lpssStreamStack : The STREAMSTACK structures used to store redirections environment. (Can be allocated by Dos9_InitStreamStack())
    \param lpName : A pointer to a NULL-terminated c string that contains the name of the file to which (or from which) the output (or the input) will be redirected
    \param iDescriptor : A descriptor numbers that represent the standard input or output to be redirected
    Can be either DOS9_STDIN, DOS9_STDOUT, DOS9_STDERR or DOS9_STDERR | DOS9_STDOUT
    \param iMode : The redirection mode (used only for output redirection). Can be either STREAM_MODE_ADD or STREAM_MODE_TRUNCATE
    \return Returns 0 if the function has succeded, -1 otherwise.
*/
 int               Dos9_OpenOutput(LPSTREAMSTACK lpssStreamStack, char* lpName, int iDescriptor, int iMode);

 int Dos9_OpenOutputD(LPSTREAMSTACK lpssStreamStack, int iNewDescriptor, int iDescriptor);

/** \brief Open a pipe redirection
    \param lpssStreamStack : The STREAMSTACK structures used to store redirections environment. (Can be allocated by Dos9_InitStreamStack())
    \return Returns 0 if the function has succeded, -1 otherwise.
*/
 int               Dos9_OpenPipe(LPSTREAMSTACK lpssStreamStack);

/** \brief Flushes the pipe
            A pipe need to be flushed, in order to put output of a program into input of another program. Then when the pipe is no longer needed,
            it must be freed. This functions do both. When it is called for the first time, it flushes the pipe, the it frees it.
    \param lpssStreamStack : The STREAMSTACK structures used to store redirections environment. (Can be allocated by Dos9_InitStreamStack())
    \return Returns 0 if the function has succeded, -1 otherwise.
*/
 LPSTREAMSTACK     Dos9_Pipe(LPSTREAMSTACK lppsStreamStack);

/** \brief Pop the content of the STREAMSTACK stack
    \param lpssStreamStack : The STREAMSTACK structures used to store redirections environment. (Can be allocated by Dos9_InitStreamStack())
    \return Returns 0 if the function has succeded, -1 otherwise.
*/
 LPSTREAMSTACK     Dos9_PopStreamStack(LPSTREAMSTACK lppsStack);

/** \brief Pop the content of the STREAMSTACK stack
    \param lpssStreamStack : The STREAMSTACK structures used to store redirections environment. (Can be allocated by Dos9_InitStreamStack())
    \return Returns 0 if the function has succeded, -1 otherwise.
*/
 LPSTREAMSTACK     Dos9_PushStreamStack(LPSTREAMSTACK lppsStack);





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

#endif
