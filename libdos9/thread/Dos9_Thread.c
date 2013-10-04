#include "../LibDos9.h"

#ifdef _POSIX_C_SOURCE

pthread_t Dos9_BeginThread(void(*lpFunction)(void*) , int iMemAmount, void* lpArgList)
{
    pthread_t hThread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&hThread, &attr, (void* (*)(void*))lpFunction,(void*)lpArgList);
    return hThread;
}

#endif
