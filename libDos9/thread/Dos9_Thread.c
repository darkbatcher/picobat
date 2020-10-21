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

#ifndef WIN32

#include <pthread.h>

/* the system provide posix compliant
   interfaces, then, use it */

/*
    begins a thread
 */
int Dos9_BeginThread(THREAD* lpThId, void*(*lpFunction)(void*) , int iMemAmount, void* lpArgList)
{
    int            iRet;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    iRet=pthread_create(&(lpThId->thread),
                        &attr,
                        lpFunction,
                        (void*)lpArgList);


    lpThId->joined = 0;
    pthread_attr_destroy(&attr);

    return iRet;
}

LIBDOS9 void Dos9_AbortThread(THREAD* thSelfId)
{
    #ifdef __BIONIC__
    /* NOTE: Missing function in bionic libc. */
    #else
    pthread_cancel(thSelfId->thread);
    #endif
    thSelfId->joined = 1;
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
    int ret;
    /* join the thread to get the exit code */
    if (ret = pthread_join(thId->thread,lpRet))
        return ret;

    thId->joined = 1;

    return 0;
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
    if (thId->joined != 1)
        pthread_detach(thId->thread);
}

#else

    #include <windows.h>


struct _dos9_beginthread_t {
    void* (*func)(void *);
    void* arg;
};

LIBDOS9 WINAPI DWORD _Dos9_BeginThreadTrampoline(LPVOID data)
{
    struct _dos9_beginthread_t *s = data;
    void* ret;

    ret = s->func(s->arg);

    free(s);

    return (DWORD)ret;
}

LIBDOS9 int  Dos9_BeginThread(THREAD* lpThId, void*(*pFunc)(void*), int iMemAmount, void* arg)
{
    struct _dos9_beginthread_t *s;

    if(!(s = malloc(sizeof(struct _dos9_beginthread_t))))
        return -1;

    s->func = pFunc;
    s->arg = arg;

    *lpThId=CreateThread(NULL,
                         0,
                         (LPTHREAD_START_ROUTINE)_Dos9_BeginThreadTrampoline,
                         (PVOID)s,
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
    return WaitForSingleObject(*thId, INFINITE)
            || !GetExitCodeThread(*thId, lpRet);
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
