#include "../LibDos9.h"

#ifndef DOS9_MAX_FREEBUFFER_SIZE
    #define DOS9_MAX_FREEBUFFER_SIZE 64
    /*
        defines the size of string garbage collection
    */
#endif // DOS9_MAX_FREEBUFFER_SIZE

int _Dos9_NewLine=DOS9_NEWLINE_WINDOWS;


ESTR* _Dos9_EstrFreeBuffer_Array[DOS9_MAX_FREEBUFFER_SIZE];
int   _Dos9_EstrFreeBuffer_MaxElement=-1;

#ifdef _POSIX_C_SOURCE

    /* what to do if we are running under a *NIX system */


#elif defined WIN32

    /* what to do if we are running undef windows */

    #include <windows.h>

    HANDLE _Dos9_EstrFreeBuffer_Mutex;

    int _Dos9_Estr_Init(void)
    {

        if (!(_Dos9_EstrFreeBuffer_Mutex=CreateMutex(NULL, FALSE, NULL))) {

            return -1;

        }


        Dos9_EsFree(NULL);
        return 0;

    }

    inline void _Dos9_EstrFreeBuffer_LockMutex(void)
    {

        switch (WaitForSingleObject(_Dos9_EstrFreeBuffer_Mutex, INFINITE)) {

            case WAIT_ABANDONED:
            case WAIT_TIMEOUT:
                fprintf(stderr, "Error : Unable to get mutex back\n");
                exit(-1);

            case WAIT_FAILED:
                fprintf(stderr, "Error : Try to get mutex failed : %d", GetLastError());
                exit(-1);

        }

    }

    inline void _Dos9_EstrFreeBuffer_ReleaseMutex(void)
    {

        if (!ReleaseMutex(_Dos9_EstrFreeBuffer_Mutex)) {

            fprintf(stderr, "Error : Unable to release Mutex : %d\n", (int)GetLastError());
            exit(-1);

        }

    }

    void _Dos9_Estr_Close(void) {

        CloseHandle(_Dos9_EstrFreeBuffer_Mutex);

    }

#endif // _POSIX_C_SOURCE

ESTR* Dos9_EsInit(void)
{
    ESTR* ptrESTR=NULL;

    _Dos9_EstrFreeBuffer_LockMutex();

    if ((_Dos9_EstrFreeBuffer_MaxElement >= 0) && (_Dos9_EstrFreeBuffer_MaxElement < (DOS9_MAX_FREEBUFFER_SIZE -1))) {

        /* just pick the last element on the array */

        ptrESTR=_Dos9_EstrFreeBuffer_Array[_Dos9_EstrFreeBuffer_MaxElement];

        _Dos9_EstrFreeBuffer_MaxElement--;


    } else {

        ptrESTR=malloc(sizeof(ESTR));

        if (ptrESTR)
        {

            ptrESTR->ptrString=malloc(DEFAULT_ESTR);
            if (ptrESTR->ptrString) {

                ptrESTR->iLenght=DEFAULT_ESTR;

            } else {

                goto Dos9_EsInit_Error;

            }


        } else {

            Dos9_EsInit_Error:

            puts("Error : Not Enough memory to run Dos9. Exiting...");
            exit(-1);

            _Dos9_EstrFreeBuffer_ReleaseMutex();
            /* Release the lock */

            return NULL;
            /* if exist fails */

        }
    }

    *(ptrESTR->ptrString)='\0';

    _Dos9_EstrFreeBuffer_ReleaseMutex();
    /* Release the lock */

    return ptrESTR;
}

LIBDOS9 int Dos9_EsFree(ESTR* ptrESTR)
{
    int i;

    _Dos9_EstrFreeBuffer_LockMutex();

    if (ptrESTR == NULL) {

        /* if the program asked for freeing all
           old ESTR that have been saved in memeory
        */

        for (i=0;i<_Dos9_EstrFreeBuffer_MaxElement;i++) {

            free(_Dos9_EstrFreeBuffer_Array[i]->ptrString);
            free(_Dos9_EstrFreeBuffer_Array[i]);

        }

        _Dos9_EstrFreeBuffer_MaxElement=-1;


    } else if (_Dos9_EstrFreeBuffer_MaxElement < (DOS9_MAX_FREEBUFFER_SIZE - 1)) {

        /* we don't free the memory, just store it in the array */

        _Dos9_EstrFreeBuffer_MaxElement++;

        _Dos9_EstrFreeBuffer_Array[_Dos9_EstrFreeBuffer_MaxElement]=ptrESTR;
        /* Note : we should check that ptrEstr is a valid pointer */

    } else {

        free(ptrESTR->ptrString);
        free(ptrESTR);

    }

    _Dos9_EstrFreeBuffer_ReleaseMutex();
        /* Release the lock */

    return 0;
}

int Dos9_EsGet(ESTR* ptrESTR, FILE* ptrFile)
{
    int iCurrentL=0, iResult=0;
    int iTotalBytesRead=0;
    char* crLf=NULL;
    char *ptrCursor=NULL;
    ptrCursor=ptrESTR->ptrString;
    iCurrentL=ptrESTR->iLenght-1;

    while (1)
    {

        iResult=(int)fgets(ptrCursor, iCurrentL+1, ptrFile);


        if(iResult==0 || iResult==EOF) {

            break;

        } else {

            iTotalBytesRead+=iResult;

        }

        crLf=strchr(ptrESTR->ptrString, '\n');
        if (crLf!=NULL) break;
        iCurrentL=ptrESTR->iLenght;
        ptrESTR->iLenght=ptrESTR->iLenght*2;
        ptrESTR->ptrString=realloc(ptrESTR->ptrString, ptrESTR->iLenght);
        if (ptrESTR->ptrString==NULL) return -1;
        ptrCursor=ptrESTR->ptrString+iCurrentL-1;
    }

    switch (_Dos9_NewLine) {

        case DOS9_NEWLINE_WINDOWS :

            Dos9_EsReplace(ptrESTR, "\r\n", "\x1\x2\x3\x4");
            Dos9_EsReplace(ptrESTR, "\n", "\r\n");
            Dos9_EsReplace(ptrESTR, "\r", "\r\n");
            Dos9_EsReplace(ptrESTR, "\x1\x2\x3\x4", "\r\n");

            break;

        case DOS9_NEWLINE_LINUX :

            Dos9_EsReplace(ptrESTR, "\r\n", "\n");
            Dos9_EsReplace(ptrESTR, "\r", "\n");

            break;

        case DOS9_NEWLINE_MAC :

            Dos9_EsReplace(ptrESTR, "\r\n", "\r");
            Dos9_EsReplace(ptrESTR, "\n", "\r");


    }

    return !iTotalBytesRead;
}


int Dos9_EsCpy(ESTR* ptrESTR, const char* ptrChaine)
{
    size_t iLen=_Dos9_EsTotalLen(ptrChaine);
    char* ptrBuf=ptrESTR->ptrString;
    if (ptrESTR->iLenght < iLen) {

        if (!(ptrBuf=realloc(ptrBuf, iLen)))
            return -1;

        ptrESTR->ptrString=ptrBuf;
        ptrESTR->iLenght=iLen;
    }

    strcpy(ptrBuf, ptrChaine);

    return 0;
}

int Dos9_EsCpyN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize)
{
    size_t iLen=_Dos9_EsTotalLen4(iSize);
    char* ptrBuf=ptrESTR->ptrString;
    if (ptrESTR->iLenght < iLen)
    {
        if (!(ptrBuf=realloc(ptrBuf, iLen)))
            return -1;

        ptrESTR->ptrString=ptrBuf;
        ptrESTR->iLenght=iLen;

    }


    strncpy(ptrBuf, ptrChaine, iSize);
    ptrBuf[iSize]='\0';

    return 0;
}

int Dos9_EsCat(ESTR* ptrESTR, const char* ptrChaine)
{
   int iLen=_Dos9_EsTotalLen2(ptrESTR->ptrString,ptrChaine);
   char *lpBuf=ptrESTR->ptrString;

   if ((ptrESTR->iLenght<iLen)) {

        if (!(lpBuf=realloc(lpBuf,iLen)))
            return -1;

        ptrESTR->ptrString=lpBuf;
        ptrESTR->iLenght=iLen;
   }

   strcat(lpBuf, ptrChaine);

   return 0;
}

int Dos9_EsCatN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize)
{
   int iLen=_Dos9_EsTotalLen3(ptrESTR->ptrString,iSize+1);
   char *lpBuf=ptrESTR->ptrString;
   if (ptrESTR->iLenght<iLen) {

        if (!(lpBuf=realloc(lpBuf,iLen)))
            return -1;

        ptrESTR->ptrString=lpBuf;
        ptrESTR->iLenght=iLen;
   }

   strncat(lpBuf, ptrChaine, iSize);

   return 0;
}

int Dos9_EsCpyE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=_Dos9_EsTotalLen(ptrSource->ptrString);
    char* ptrBuf=ptrDest->ptrString;

    if (iLen > ptrDest->iLenght)
    {
        if (!(ptrBuf=realloc(ptrBuf, iLen)))
            return -1;

        ptrDest->ptrString=ptrBuf;
        ptrDest->iLenght=iLen;
    }

    strcpy(ptrBuf, ptrSource->ptrString);

    return 0;
}

int Dos9_EsCatE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=_Dos9_EsTotalLen2(ptrDest->ptrString, ptrSource->ptrString);
    char* lpBuf=ptrDest->ptrString;

    if (ptrDest->iLenght<iLen) {

        if (!(lpBuf=realloc(lpBuf,iLen)))
            return -1;

        ptrDest->ptrString=lpBuf;
        ptrDest->iLenght=iLen;
    }

    strcat(lpBuf, ptrSource->ptrString);

    return 0;
}

LIBDOS9 int Dos9_EsReplace(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace)
{
    char* lpBuffer=Dos9_EsToChar(ptrESTR), *lpToken;
    int iLength=strlen(ptrPattern);
    ESTR *lpReturn=Dos9_EsInit();

    while ((lpToken=strstr(lpBuffer, ptrPattern))) {
        *lpToken='\0';
        Dos9_EsCat(lpReturn, lpBuffer);
        Dos9_EsCat(lpReturn, ptrReplace);
        lpBuffer=lpToken+iLength;
    }

    Dos9_EsCat(lpReturn, lpBuffer);
    Dos9_EsCpyE(ptrESTR, lpReturn);
    Dos9_EsFree(lpReturn);
    return 0;
}


inline size_t _Dos9_EsTotalLen(const char* ptrChaine)
{
    return (strlen(ptrChaine)/DEFAULT_ESTR+1)*DEFAULT_ESTR;
}

inline size_t _Dos9_EsTotalLen2(const char* ptrChaine, const char* ptrString)
{
    return ((strlen(ptrChaine)+strlen(ptrString))/DEFAULT_ESTR+1)*DEFAULT_ESTR;
}

inline size_t _Dos9_EsTotalLen3(const char* ptrChaine, size_t iSize)
{
    return ((strlen(ptrChaine)+iSize)/DEFAULT_ESTR+1)*DEFAULT_ESTR;
}

inline size_t _Dos9_EsTotalLen4(const size_t iSize)
{
    return (iSize/DEFAULT_ESTR+1)*DEFAULT_ESTR;
}
