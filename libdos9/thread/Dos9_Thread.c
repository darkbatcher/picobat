/*
 *
 *   libDos9 - The Dos9 project
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../libDos9.h"


STACK* _lpcsThreadStack=NULL;

/* do not initialize this, because it might be an
   implicit data structure
 */
MUTEX _threadStack_Mutex;

int _Dos9_Thread_Init(void) {

    _lpcsThreadStack=NULL;

    return Dos9_CreateMutex(&_threadStack_Mutex);

}

#ifdef _POSIX_C_SOURCE

    #include <pthread.h>

/* the system provide posix compliant
   interfaces, then, use it */

void _Dos9_Thread_Close(void) {

    pthread_t *lpThId;

    Dos9_ClearStack(_lpcsThreadStack, free);

    return Dos9_CloseMutex(&_threadStack_Mutex);
}


/*
    begins a thread
 */
int Dos9_BeginThread(THREAD* lpThId, void(*lpFunction)(void*) , int iMemAmount, void* lpArgList)
{
    int            iRet;
    pthread_t     *lpTh2;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    iRet=pthread_create(lpThId,
                        &attr,
                        (void* (*)(void*))lpFunction,
                        (void*)lpArgList);

    if (iRet==0) {

        if (!(lpTh2=malloc(sizeof(pthread_t))))
            return -1;

        /* duplicate the pthread_t structure */
        memcpy(lpTh2, lpThId, sizeof(pthread_t));

        Dos9_LockMutex(&_threadStack_Mutex);

        /* push the pthread_t structure on the stack */
        _lpcsThreadStack=Dos9_PushStack(_lpcsThreadStack,
                                         lpTh2
                                         );

        Dos9_ReleaseMutex(&_threadStack_Mutex);

    }

    #ifndef LIBDOS9_THREAD_SILENT

    if (iRet) {

        fprintf(stderr,
                "[libDos9/Dos9_BeginThread()] Error: %s\n",
                strerror(errno));


    }

    #endif

    pthread_attr_destroy(&attr);

    return iRet;
}

/*
    Ends a thread
 */
LIBDOS9 void     Dos9_EndThread(void* iReturn)
{
    /* ends the current thread */
    pthread_t thSelfId,
             *thId;

    STACK    *lpThreadStack,
             *lpLastThreadStack=NULL;

    /* gets the current thread image */
    thSelfId=pthread_self();

    /* lock the mutex to acces to
       the thread stack */

    Dos9_LockMutex(&_threadStack_Mutex);

    lpThreadStack=_lpcsThreadStack;
    lpLastThreadStack=NULL;

    /* picks the stack element that matche
       curent thread pthread_t structure */
    while (lpThreadStack) {

        /* gets the value from the stack */
        Dos9_GetStack(_lpcsThreadStack, &thId);

        if (pthread_equal(*thId, thSelfId)) {
            /* both threads are the same */

            if (lpLastThreadStack) {

                lpLastThreadStack->lpcsPrevious=
                    lpThreadStack->lpcsPrevious;

                free(lpThreadStack);

            } else {

                _lpcsThreadStack=
                    Dos9_PopStack(_lpcsThreadStack, free);

            }

            /* free the structure we got from the
               stack */
            free(thId);

            break;

        }

        /* if we hav'nt found anything, dig deeper
           into the stack */
        lpLastThreadStack=lpThreadStack;
        lpThreadStack=lpThreadStack->lpcsPrevious;

    }

    Dos9_ReleaseMutex(&_threadStack_Mutex);

    pthread_exit(lpReturn);
}

/*
    Waits for a thread and get its return value
 */
LIBDOS9 int     Dos9_WaitForThread(THREAD thId, void* lpRet)
{

    /* join the thread to get the exit code */
    return pthread_join(thId,lpReturn);
}

/*
   Waits for all remaining threads, ie, threads that have
   not terminated yet

   if an error occurs, it returns the error code from the
   POSIX thread library
 */
LIBDOS9 int     Dos9_WaitForAllThreads(void)
{
    int     iRet;
    THREAD  thId;

    Dos9_LockMutex(&_lpcsThreadStack);

    while (_lpcsThreadStack) {

        thId=(THREAD*)Dos9_GetStack(_lpcsThreadStack,
                               NULL);


        if (!(iRet=pthread_join(thId, NULL)))
            return iRet;

        _lpcsThreadStack=Dos9_PopStack(_lpcsThreadStack, free);

    }

    Dos9_ReleaseMutex(&_lpcsThreadStack);

    return 0;
}

LIBDOS9 int Dos9_CreateMutex(MUTEX* lpMuId)
{
    pthread_mutexattr_t attr;
    int                 iRet;

    pthread_mutexattr_init(&attr);

    /* create the mutex */
    iRet=pthread_mutex_init(lpMuId, &attr);

    #ifndef LIBDOS9_THREAD_SILENT

    if (iRet) {

        fprintf(stderr,
                "[libDos9/Dos9_CreateMutex()] Error: %s\n",
                strerror(errno));

        exit(-1);

    }

    #endif

    pthread_mutexattr_destroy(&attr);



    return iRet;

}

LIBDOS9 int     Dos9_CloseMutex(MUTEX* lpmuId)
{
    int iRet;

    iRet=pthread_mutex_destroy(lpMuId)

    #ifndef LIBDOS9_THREAD_SILENT

    if (iRet) {

        fprintf(stderr,
                "[libDos9/Dos9_CloseMutex()] Error: %s\n",
                strerror(errno));

        exit(-1);


    }

    #endif

    return iRet;
}

LIBDOS9 int     Dos9_LockMutex(MUTEX* lpmuId)
{
    int iRet;

    iRet=pthread_mutex_lock(lpMuId);

    #ifndef LIBDOS9_THREAD_SILENT

    if (iRet) {

        fprintf(stderr,
                "[libDos9/Dos9_LockMutex()] Error: %s\n",
                strerror(errno));

        exit(-1);

    }

    #endif

    return iRet;

}

LIBDOS9 int     Dos9_ReleaseMutex(MUTEX lpmuId)
{
    int iRet;

    iRet=pthread_mutex_unlock(lpMuId)

    #ifndef LIBDOS9_THREAD_SILENT

    if (iRet) {

        fprintf(stderr,
                "[libDos9/Dos9_ReleaseMutex()] Error: %s\n",
                strerror(errno));


        exit(-1);

    }

    #endif

    return iRet;

}

#else

    #include <windows.h>

    /* use the windows interface */

void _Dos9_Thread_Close(void)
{
    Dos9_LockMutex(&_threadStack_Mutex);

    Dos9_ClearStack(_lpcsThreadStack, NULL);

    Dos9_ReleaseMutex(&_threadStack_Mutex);

    Dos9_CloseMutex(&_threadStack_Mutex);

}

LIBDOS9 int  Dos9_BeginThread(THREAD* lpThId, void(*pFunc)(void*), int iMemAmount, void* arg)
{

    int iId;

    *lpThId=CreateThread(NULL,
                         0,
                         (LPTHREAD_START_ROUTINE)pFunc,
                         (PVOID)arg,
                         0,
                         (PDWORD)&iId
                         );

    #ifndef LIBDOS9_THREAD_SILENT

    if (*lpThId==INVALID_HANDLE_VALUE) {

        fprintf(stderr,
                "[libDos9/Dos9_BeginThread()] Error: Unable to start thread : %d.\n",
                (int)GetLastError()
                );

        exit(-1);

    }

    #endif

    if (*lpThId!=INVALID_HANDLE_VALUE) {

        Dos9_LockMutex(&_threadStack_Mutex);

        _lpcsThreadStack=
            Dos9_PushStack(_lpcsThreadStack,
                           (void*)iId
                           );

        Dos9_ReleaseMutex(&_threadStack_Mutex);

    }

    return (*lpThId == INVALID_HANDLE_VALUE) ? -1 : 0;
}

LIBDOS9 void     Dos9_EndThread(void* iReturn)
{

    STACK *lpStackElement,
          *lpLastStackElement;
    int   iCurrent,
          iSearch;

    iCurrent=(int)GetCurrentThreadId();

    Dos9_LockMutex(&_threadStack_Mutex);

    lpLastStackElement=NULL;
    lpStackElement=_lpcsThreadStack;

    while (lpStackElement) {

        Dos9_GetStack(lpLastStackElement, (void**)&iSearch);

        if (iCurrent == iSearch) {

            if (lpLastStackElement) {

                lpLastStackElement->lpcsPrevious=
                    lpStackElement->lpcsPrevious;


                free(lpStackElement);

            } else {

                _lpcsThreadStack=Dos9_PopStack(_lpcsThreadStack, NULL);

            }

            break;

        }

        lpLastStackElement=lpStackElement;
        lpStackElement=lpStackElement->lpcsPrevious;

    }

    Dos9_ReleaseMutex(&_threadStack_Mutex);

    ExitThread((DWORD)iReturn);
}

LIBDOS9 int      Dos9_WaitForThread(THREAD thId, void* lpRet)
{
    int iRet;

    iRet=WaitForSingleObject((HANDLE)thId, INFINITE);

    #ifndef LIBDOS9_THREAD_SILENT

    switch(iRet) {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            fprintf(stderr, "[libDos9/Dos9_WaitForThread()] Error : Unable to join the process.\n");
            exit(-1);

        case WAIT_FAILED:
            fprintf(stderr, "[libDos9/Dos9_WaitForThread()] Error : Wait failed : %d", (int)GetLastError());
            exit(-1);
    }

    #endif

    if (iRet)
        return iRet;

    iRet=GetExitCodeThread((HANDLE)thId, (PDWORD)lpRet);

    return iRet;

}

LIBDOS9 int      Dos9_WaitForAllThreads(void)
{

    int iWait;

    Dos9_LockMutex(&_threadStack_Mutex);

    while (_lpcsThreadStack) {

        Dos9_GetStack(_lpcsThreadStack,
                      (void**)&iWait
                      );

        WaitForSingleObject((HANDLE)iWait, INFINITE);

        _lpcsThreadStack=
            Dos9_PopStack(_lpcsThreadStack, NULL);
    }

    Dos9_ReleaseMutex(&_threadStack_Mutex);

    return 0;
}

LIBDOS9 int      Dos9_CreateMutex(MUTEX* lpMuId)
{
    *lpMuId=CreateMutex(NULL, FALSE, NULL);

    #ifndef LIBDOS9_THREAD_SILENT

    if (*lpMuId==NULL) {

        fprintf(stderr,
                "[libDos9/Dos9_CreateMutex()]Error : Unable create mutex object : %d",
                (int)GetLastError()
                );

        exit(-1);
    }

    #endif

    return (*lpMuId==NULL);
}

LIBDOS9 int      Dos9_CloseMutex(MUTEX* lpMuId)
{
    CloseHandle(*lpMuId);

    return 0;
}

LIBDOS9 int      Dos9_LockMutex(MUTEX* lpMuId)
{
    int iRet;

    iRet=WaitForSingleObject(*lpMuId, INFINITE);

    #ifndef LIBDOS9_THREAD_SILENT

    switch(iRet) {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            fprintf(stderr, "[libDos9/Dos9_LockMutex()] Error : Unable to get mutex back.\n");
            exit(-1);

        case WAIT_FAILED:
            fprintf(stderr, "[libDos9/Dos9_LockMutex()] Error : Try to get mutex failed : %d", (int)GetLastError());
            exit(-1);
    }

    #endif

    return iRet;
}

LIBDOS9 int      Dos9_ReleaseMutex(MUTEX* lpMuId)
{
    int iRet;

    iRet=ReleaseMutex(*lpMuId);

    #ifndef LIBDOS9_THREAD_SILENT

    if (iRet == 0) {

            fprintf(stderr, "Error : Unable to release Mutex : %d\n", (int)GetLastError());
            exit(-1);

    }

    #endif

    return iRet;

}

#endif
