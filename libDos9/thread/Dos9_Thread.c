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

#include "../libDos9-int.h"


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

    Dos9_CloseMutex(&_threadStack_Mutex);
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

LIBDOS9 void Dos9_AbortThread(THREAD* thSelfId)
{

    /* aborts the given thread */
    pthread_t *thId;

    STACK    *lpThreadStack,
             *lpLastThreadStack=NULL;

    /* lock the mutex to acces to
       the thread stack */

    Dos9_LockMutex(&_threadStack_Mutex);

    lpThreadStack=_lpcsThreadStack;
    lpLastThreadStack=NULL;

    /* picks the stack element that matche
       curent thread pthread_t structure */
    while (lpThreadStack) {

        /* gets the value from the stack */
        Dos9_GetStack(lpThreadStack, (void**)&thId);

        if (pthread_equal(*thId, *thSelfId)) {
            /* both threads are the same */

            if (lpLastThreadStack) {

                lpLastThreadStack->lpcsPrevious=
                    lpThreadStack->lpcsPrevious;

                free(lpThreadStack);

            } else {

                _lpcsThreadStack=
                    Dos9_PopStack(_lpcsThreadStack, free);

            }

            /* don't free anything because things are freed by
               the if */

            break;

        }

        /* if we hav'nt found anything, dig deeper
           into the stack */
        lpLastThreadStack=lpThreadStack;
        lpThreadStack=lpThreadStack->lpcsPrevious;

    }

    Dos9_ReleaseMutex(&_threadStack_Mutex);

    pthread_cancel(*thSelfId);

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
        Dos9_GetStack(lpThreadStack, (void**)&thId);



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

            /* don't free anything because things are freed by
               the if */

            break;

        }

        /* if we hav'nt found anything, dig deeper
           into the stack */
        lpLastThreadStack=lpThreadStack;
        lpThreadStack=lpThreadStack->lpcsPrevious;

    }

    Dos9_ReleaseMutex(&_threadStack_Mutex);

    pthread_exit(iReturn);
}

/*
    Waits for a thread and get its return value
 */
LIBDOS9 int     Dos9_WaitForThread(THREAD* thId, void* lpRet)
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

LIBDOS9 int     Dos9_CloseMutex(MUTEX* lpMuId)
{
    int iRet;

    iRet=pthread_mutex_destroy(lpMuId);

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

LIBDOS9 int     Dos9_LockMutex(MUTEX* lpMuId)
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

LIBDOS9 int     Dos9_ReleaseMutex(MUTEX* lpMuId)
{
    int iRet;

    iRet=pthread_mutex_unlock(lpMuId);

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

    //#define _WIN32_WINNT _WIN32_WINNT_WINXP

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

    HANDLE hThread;

    hThread=CreateThread(NULL,
                         0,
                         (LPTHREAD_START_ROUTINE)pFunc,
                         (PVOID)arg,
                         0,
                         lpThId
                         );

    #ifndef LIBDOS9_THREAD_SILENT

    if (hThread==INVALID_HANDLE_VALUE) {

        fprintf(stderr,
                "[libDos9/Dos9_BeginThread()] Error: Unable to start thread : %d.\n",
                (int)GetLastError()
                );

        exit(-1);

    }

    #endif

    if (hThread!=INVALID_HANDLE_VALUE) {

        Dos9_LockMutex(&_threadStack_Mutex);

        _lpcsThreadStack=
            Dos9_PushStack(_lpcsThreadStack,
                           (void*)*lpThId
                           );


        Dos9_ReleaseMutex(&_threadStack_Mutex);

        CloseHandle(hThread);

        return 0;

    }

    return -1;
}

LIBDOS9 void     Dos9_AbortThread(THREAD* lpThId)
{

    STACK *lpStackElement,
          *lpLastStackElement;
    size_t iCurrent=*lpThId,
		   iSearch;

    HANDLE hThread;

    Dos9_LockMutex(&_threadStack_Mutex);

    lpLastStackElement=NULL;
    lpStackElement=_lpcsThreadStack;

    while (lpStackElement) {

        Dos9_GetStack(lpStackElement, (void**)&iSearch);

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

    hThread=OpenThread(THREAD_ALL_ACCESS,
                       FALSE,
                       *lpThId);

    TerminateThread(hThread, 0);

    CloseHandle(hThread);
}

LIBDOS9 void     Dos9_EndThread(void* iReturn)
{

	//fprintf(stderr, "Ending thread !\n");

    STACK *lpStackElement,
          *lpLastStackElement;
    size_t   iCurrent,
          	 iSearch;

    iCurrent=(size_t)GetCurrentThreadId();

    Dos9_LockMutex(&_threadStack_Mutex);

    lpLastStackElement=NULL;
    lpStackElement=_lpcsThreadStack;

    while (lpStackElement) {

        Dos9_GetStack(lpStackElement, (void**)&iSearch);

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

    //fprintf(stderr, "Thread ended\n");
}

LIBDOS9 int      Dos9_WaitForThread(THREAD* thId, void* lpRet)
{
    int iRet;

    HANDLE hThread;

    hThread=OpenThread(THREAD_ALL_ACCESS,
                       FALSE,
                       *thId);

    if (hThread==NULL)
        return 0;

    iRet=WaitForSingleObject(hThread, INFINITE);

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

    iRet=GetExitCodeThread(hThread, (PDWORD)lpRet);

    return iRet;

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
            fprintf(stderr,
                    "[libDos9/Dos9_LockMutex()] Error : Try to get mutex "
                    "failed : %d",
                    (int)GetLastError()
                   );

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

LIBDOS9 int      Dos9_WaitForAllThreads(int iDelay)
{
    int iContinue=TRUE,
        iAttempt=0;

    STACK* lpStack;

    #ifdef _POSIX_C_SOURCE
        struct timespec tDelay={0,iDelay*10};

    #else
        iDelay/=10;
    #endif



    do {

        Dos9_LockMutex(&_threadStack_Mutex);

        iContinue=(_lpcsThreadStack!=NULL);

        Dos9_ReleaseMutex(&_threadStack_Mutex);

        /* increment the counter to avoid deadlock if
           a process never returns, the function will
           wait for at most iDelay Milliseconds */
        iAttempt++;

        #ifdef _POSIX_C_SOURCE

            nanosleep(&tDelay, NULL);

        #elif defined WIN32

           Sleep(10);

        #endif

    } while (iContinue && (iAttempt <= iDelay));

    return (iAttempt > iDelay);
}

LIBDOS9 void     Dos9_AbortAllThreads(void)
{
    THREAD thId;
    int iContinue=TRUE;

    while (iContinue) {

        Dos9_LockMutex(&_threadStack_Mutex);

        if (_lpcsThreadStack == NULL) {

            iContinue=FALSE;
            break;

        }

        /* retrieve the next thread id */
        Dos9_GetStack(_lpcsThreadStack, (void*)&thId);

        Dos9_ReleaseMutex(&_threadStack_Mutex);

        Dos9_AbortThread(&thId);

    }

}

