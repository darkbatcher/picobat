/*
 *
 *   libDos9 - The Dos9 project
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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../libDos9.h"
#include "../libDos9-int.h"

int _Dos9_Thread_Init(void) {
	return 0;
}

#ifndef WIN32

#include <pthread.h>

/* the system provide posix compliant
   interfaces, then, use it */

void _Dos9_Thread_Close(void) {
}


/*
    begins a thread
 */
int Dos9_BeginThread(THREAD* lpThId, void(*lpFunction)(void*) , int iMemAmount, void* lpArgList)
{
    int            iRet;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    iRet=pthread_create(lpThId,
                        &attr,
                        (void* (*)(void*))lpFunction,
                        (void*)lpArgList);

    pthread_attr_destroy(&attr);

    return iRet;
}

LIBDOS9 void Dos9_AbortThread(THREAD* thSelfId)
{
    pthread_cancel(*thSelfId);
}

/*
    Ends a thread
 */
LIBDOS9 void     Dos9_EndThread(void* iReturn)
{
    pthread_exit(iReturn);
}

/*
    Waits for a thread and get its return value
 */
LIBDOS9 int     Dos9_WaitForThread(THREAD* thId, void** lpRet)
{

    /* join the thread to get the exit code */
    return pthread_join(*thId,lpRet);
}

LIBDOS9 int Dos9_CreateMutex(MUTEX* lpMuId)
{
    pthread_mutexattr_t attr;
    int                 iRet;

    pthread_mutexattr_init(&attr);

    /* create the mutex */
    iRet=pthread_mutex_init(lpMuId, &attr);

    pthread_mutexattr_destroy(&attr);

    return iRet;
}

LIBDOS9 int     Dos9_CloseMutex(MUTEX* lpMuId)
{
    return pthread_mutex_destroy(lpMuId);;
}

LIBDOS9 int     Dos9_LockMutex(MUTEX* lpMuId)
{
    return pthread_mutex_lock(lpMuId);
}

LIBDOS9 int     Dos9_ReleaseMutex(MUTEX* lpMuId)
{
    return pthread_mutex_unlock(lpMuId);
}

LIBDOS9 void     Dos9_CloseThread(THREAD* thId)
{
    pthread_detach(*thId);
}

#else

    #include <windows.h>

void _Dos9_Thread_Close(void)
{
}

LIBDOS9 int  Dos9_BeginThread(THREAD* lpThId, void(*pFunc)(void*), int iMemAmount, void* arg)
{
    *lpThId=CreateThread(NULL,
                         0,
                         (LPTHREAD_START_ROUTINE)pFunc,
                         (PVOID)arg,
                         0,
                         NULL
                         );

    if (*lpThId!=INVALID_HANDLE_VALUE)
        return 0;

    return -1;
}

LIBDOS9 void     Dos9_AbortThread(THREAD* lpThId)
{
    TerminateThread(*lpThId, 0);
}

LIBDOS9 void     Dos9_EndThread(void* iReturn)
{
    ExitThread((DWORD)iReturn);
}

LIBDOS9 int      Dos9_WaitForThread(THREAD* thId, void** lpRet)
{
    return WaitForSingleObject(*thId, INFINITE);;
}

LIBDOS9 int      Dos9_CreateMutex(MUTEX* lpMuId)
{
    *lpMuId=CreateMutex(NULL, FALSE, NULL);

    return (*lpMuId!=NULL);
}

LIBDOS9 int      Dos9_CloseMutex(MUTEX* lpMuId)
{
    CloseHandle(*lpMuId);
    return 0;
}

LIBDOS9 int      Dos9_LockMutex(MUTEX* lpMuId)
{
    return WaitForSingleObject(*lpMuId, INFINITE);
}

LIBDOS9 int      Dos9_ReleaseMutex(MUTEX* lpMuId)
{
    return !ReleaseMutex(*lpMuId);
}

LIBDOS9 void     Dos9_CloseThread(THREAD* thId)
{
    CloseHandle(*thId);
}

#endif

LIBDOS9 int      Dos9_WaitForAllThreads(void)
{
    return 0;
}

LIBDOS9 void     Dos9_AbortAllThreads(void)
{
}
