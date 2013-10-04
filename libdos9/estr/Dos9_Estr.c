#include "../LibDos9.h"

int _Dos9_NewLine=DOS9_NEWLINE_WINDOWS;

ESTR* Dos9_EsInit(void)
{
    ESTR* ptrESTR=NULL;
    ptrESTR=malloc(sizeof(ESTR));
    if (ptrESTR)
    {
        ptrESTR->ptrString=malloc(DEFAULT_ESTR);
        if (ptrESTR->ptrString) ptrESTR->iLenght=DEFAULT_ESTR;
        else return NULL;
        *(ptrESTR->ptrString)='\0';
        return ptrESTR;
    } else {
        puts("Error : Not Enough memory to run Dos9. Exiting...");
        getchar();
        exit(-1);
        return NULL;
    }
}

LIBDOS9 int Dos9_EsFree(ESTR* ptrESTR)
{

    free(ptrESTR->ptrString);
    free(ptrESTR);
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
    char* ptrBuf;
    if (ptrESTR->iLenght < iLen)
    {
        if ((ptrBuf=malloc(iLen))) {
            free(ptrESTR->ptrString);
            ptrESTR->ptrString=ptrBuf;
        } else {
            return -1;
        }

        ptrESTR->iLenght=iLen;
    }
    strcpy(ptrESTR->ptrString, ptrChaine);
    return 0;
}

int Dos9_EsCpyN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize)
{
    size_t iLen=_Dos9_EsTotalLen4(iSize);
    char* ptrBuf;
    if (ptrESTR->iLenght < iLen)
    {
        if ((ptrBuf=malloc(iLen))) {
            free(ptrESTR->ptrString);
            ptrESTR->ptrString=ptrBuf;
        } else {
            return -1;
        }

        ptrESTR->iLenght=iLen;
    }

    ptrBuf=ptrESTR->ptrString;
    while(*ptrChaine && iSize>1) {
        *ptrBuf=*ptrChaine;
        ptrBuf++;
        ptrChaine++;
        iSize--;
    }
    *ptrBuf='\0';
    return 0;
}

int Dos9_EsCat(ESTR* ptrESTR, const char* ptrChaine)
{
   int iLen=_Dos9_EsTotalLen2(ptrESTR->ptrString,ptrChaine);
   char *lpBuf;
   if ((ptrESTR->iLenght<iLen))
   {
        lpBuf=realloc(ptrESTR->ptrString,iLen);
        if (lpBuf==NULL) return -1;
        ptrESTR->ptrString=lpBuf;
        ptrESTR->iLenght=iLen;
   }

   lpBuf=ptrESTR->ptrString;
   while (*lpBuf) lpBuf++;

   while (*ptrChaine) {
        *lpBuf=*ptrChaine;
        ptrChaine++;
        lpBuf++;
   }
   *lpBuf='\0';

   return 0;
}

int Dos9_EsCatN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize)
{
   int iLen=_Dos9_EsTotalLen3(ptrESTR->ptrString,iSize+1);
   char *lpBuf;
   if (ptrESTR->iLenght<iLen)
   {
        lpBuf=realloc(ptrESTR->ptrString,iLen);
        if (lpBuf==NULL) return -1;
        ptrESTR->ptrString=lpBuf;
        ptrESTR->iLenght=iLen;
   }

   lpBuf=ptrESTR->ptrString;
   while (*lpBuf) lpBuf++;

   while (*ptrChaine  && iSize) {
        *lpBuf=*ptrChaine;
        ptrChaine++;
        lpBuf++;
        iSize--;
   }
   *lpBuf='\0';

   return 0;
}

int Dos9_EsCpyE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=_Dos9_EsTotalLen(ptrSource->ptrString);
    char* ptrBuf;
    if (iLen > ptrDest->iLenght)
    {
        if ((ptrBuf=malloc(iLen))) {
            free(ptrDest->ptrString);
            ptrDest->ptrString=ptrBuf;
        } else {
            return 1;
        }
        ptrDest->iLenght=iLen;
    }

    strcpy(ptrDest->ptrString, ptrSource->ptrString);

    return 0;
}

int Dos9_EsCatE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=_Dos9_EsTotalLen2(ptrDest->ptrString, ptrSource->ptrString);
    char* lpBuf, *lpSource;
    if (ptrDest->iLenght<iLen)
    {
        lpBuf=realloc(ptrDest->ptrString,iLen);
        if (lpBuf==NULL) return -1;
        ptrDest->ptrString=lpBuf;
        ptrDest->iLenght=iLen;
    }

    lpSource=ptrSource->ptrString;
    lpBuf=ptrDest->ptrString;
    while(*lpBuf) lpBuf++;

    while (*lpSource) {
        *lpBuf=*lpSource;
        lpSource++;
        lpBuf++;
    }
    *lpBuf='\0';

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
