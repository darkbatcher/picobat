#include "../LibDos9.h"

#ifndef DOS9_MAX_FREEBUFFER_SIZE
    #define DOS9_MAX_FREEBUFFER_SIZE 64
    /*
        defines the size of string garbage collection
    */
#endif // DOS9_MAX_FREEBUFFER_SIZE



int _Dos9_NewLine=DOS9_NEWLINE_WINDOWS;

#ifdef LIBDOS9_WITH_THREAD


ESTR* _Dos9_EstrFreeBuffer_Array[DOS9_MAX_FREEBUFFER_SIZE];
int   _Dos9_EstrFreeBuffer_MaxElement=-1;

MUTEX _Dos9_EstrFreeBuffer_Mutex;

int _Dos9_Estr_Init(void)
{

    if (Dos9_CreateMutex(&_Dos9_EstrFreeBuffer_Mutex) != 0) {

        return -1;

    }


    Dos9_EsFree(NULL);
    return 0;

}


void _Dos9_Estr_Close(void) {

    Dos9_CloseMutex(&_Dos9_EstrFreeBuffer_Mutex);

}

ESTR* Dos9_EsInit(void)
{
    ESTR* ptrESTR=NULL;

    Dos9_LockMutex(&_Dos9_EstrFreeBuffer_Mutex);

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

            Dos9_ReleaseMutex(&_Dos9_EstrFreeBuffer_Mutex);
            /* Release the lock */

            return NULL;
            /* if exist fails */

        }
    }

    *(ptrESTR->ptrString)='\0';

    Dos9_ReleaseMutex(&_Dos9_EstrFreeBuffer_Mutex);

    /* Release the lock */

    return ptrESTR;
}

LIBDOS9 int Dos9_EsFree(ESTR* ptrESTR)
{
    int i;

    Dos9_LockMutex(&_Dos9_EstrFreeBuffer_Mutex);

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

    Dos9_ReleaseMutex(&_Dos9_EstrFreeBuffer_Mutex);
        /* Release the lock */

    return 0;
}

#else

int _Dos9_Estr_Init(void)
{
    return 0;
}


void _Dos9_Estr_Close(void)
{

}

ESTR* Dos9_EsInit(void)
{
    ESTR* ptrESTR=NULL;

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

        return NULL;
        /* if exist fails */

    }

    *(ptrESTR->ptrString)='\0';

    /* Release the lock */

    return ptrESTR;
}

LIBDOS9 int Dos9_EsFree(ESTR* ptrESTR)
{
    /* free the string buffer */
    free(ptrESTR->ptrString);

    /* free the structure itself */
    free(ptrESTR);

    return 0;
}


#endif // LIBDOS9_WITH_THREAD

int Dos9_EsGet(ESTR* ptrESTR, FILE* ptrFile)
{
    int iCurrentL=0;

    size_t iTotalBytesRead=0;

    char *crLf=NULL,
         *ptrCursor=NULL,
         *lpResult;

    ptrCursor=ptrESTR->ptrString;
    iCurrentL=ptrESTR->iLenght-1;

    //printf("Start of function...");

    //printf(" Beginning loop...");

    while (1)
    {

        lpResult=fgets(ptrCursor, iCurrentL+1, ptrFile);


        if(lpResult==NULL) {

            /* this means we will continue because there is no more
               stuff to get from the file */

            break;

        } else {

            /* this could appear to be the number of byte read
               however, it is just used to know wether the
               function have read some text there */

            iTotalBytesRead+=(size_t)lpResult;

        }

        crLf=strchr(ptrESTR->ptrString, '\n');

        if (crLf!=NULL)
            break;

        iCurrentL=ptrESTR->iLenght;

        ptrESTR->iLenght=ptrESTR->iLenght*2;

        crLf=realloc(ptrESTR->ptrString, ptrESTR->iLenght);

        if (crLf==NULL) {
                /* make if more fault tolerant, abort the loop,
                   and just read a part of the real line (not
                   so bad however). */
                // fprintf(stderr, "Error ! Unable to relocate string (iCurrentL=%d).\n", iCurrentL);

                ptrESTR->iLenght=iCurrentL;

                return -1;
        }

        ptrESTR->ptrString=crLf;

        ptrCursor=ptrESTR->ptrString+iCurrentL-1;

    }

    //printf(" Ending loop...");

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

    //printf(" Returning\n");

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

        /* this is some test to prevent user from experiencing
           crash in windows whenever iSize is 0. Sadly, windows
           crt's send SIGSEGV to the program, although this
           is correct for C89 standard (afaik) */

    if (iSize)
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
