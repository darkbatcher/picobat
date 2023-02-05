/*
 *
 *   libpBat - The pBat project
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include "../libpBat.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <strings.h>
#endif

#define DEFAULT_ESTR 256

//#define PBAT_ESTR_SIZE(s) ((s) / DEFAULT_ESTR + 1) * DEFAULT_ESTR
#define PBAT_ESTR_SIZE(s) __fast_compute_size(s)

static int __inline__ __fast_compute_size(size_t s)
{
    size_t r = DEFAULT_ESTR;
    while (r < s)
        r <<= 1;
    return r;
}

int _pBat_NewLine=PBAT_NEWLINE_LINUX;

int _pBat_Estr_Init(void)
{
    return 0;
}


void _pBat_Estr_Close(void)
{

}

char* pBat_strdup(const char* src)
{
    char* ret;

    if (!(ret = malloc(strlen(src)+1)))
        return NULL;

    strcpy(ret, src);
    return ret;
}

ESTR* pBat_EsInit(void)
{
    ESTR* ptrESTR=NULL;

    ptrESTR=malloc(sizeof(ESTR));

    if (ptrESTR)
    {

        ptrESTR->str=malloc(DEFAULT_ESTR);

        if (!(ptrESTR->str))
            goto pBat_EsInit_Error;

        ptrESTR->len=DEFAULT_ESTR;

    } else {

        pBat_EsInit_Error:

        puts("Error : Not Enough memory to run pBat. Exiting...");
        exit(-1);

        return NULL;
        /* if exist fails */

    }

    *(ptrESTR->str)='\0';

    /* Release the lock */

    return ptrESTR;
}

#if 0
struct mes {
    int len;
    struct mes* next;
};

static struct mes* pBat_AddMesure(struct mes* m, int len)
{
    struct mes *ret=m,
        *tmp;

    if ((ret == NULL)
        || (len < m->len)) {

        if ((ret = malloc(sizeof(struct mes))) == NULL)
            return m;

        ret->len = len;
        ret->next = m;

        return ret;

    }

    while(m) {

        if (len >= m->len) {

            if ((tmp = malloc(sizeof(struct mes))) == NULL)
                return ret;

            tmp->len = len;
            tmp->next = m->next;
            m->next = tmp;
            break;

        }

        m = m->next;

    }

    return ret;
}

static int pBat_MedMesure(struct mes* m)
{
    int count=0,
        i=0;
    struct mes* orig = m;

    while(m) {

        count ++;
        m = m->next;

    }

    if (count & 1)
        count += 1; /* make count even */

    count /= 2;
    m = orig;

    while (i ++ < count)
        m = m->next;

    return m->len;
}
#endif


LIBPBAT int pBat_EsFree(ESTR* ptrESTR)
{
    #if 0

    static struct mes *mesure = NULL;

    if (ptrESTR == NULL)
        return pBat_MedMesure(mesure);

    mesure = pBat_AddMesure(mesure, strlen(ptrESTR->str));

    #endif // 0

    /* free the string buffer */
    free(ptrESTR->str);

    /* free the structure itself */
    free(ptrESTR);

    return 0;
}

LIBPBAT int pBat_EsGet(ESTR* ptrESTR, FILE* ptrFile)
{
    int iCurrentL=0;

    size_t iTotalBytesRead=0;

    char *crLf=NULL,
         *ptrCursor=NULL,
         *lpResult;

    ptrCursor=ptrESTR->str;
    iCurrentL=ptrESTR->len-1;

    while (1)
    {

        lpResult=fgets(ptrCursor, iCurrentL+1, ptrFile);

        if(lpResult==NULL) {

            /* this means we will continue because there is no more
               stuff to get from the file */

            break;

        } else {

            /* this could appear to be the number of byte read
               however, it is just used to know whether the
               function have read some text there */

            iTotalBytesRead+=(size_t)lpResult;

        }

        crLf=strchr(ptrESTR->str, '\n');

        if (crLf!=NULL)
            break;

        iCurrentL=ptrESTR->len;

        ptrESTR->len=ptrESTR->len*2;

        crLf=realloc(ptrESTR->str, ptrESTR->len);

        if (crLf==NULL) {
                /* make if more fault tolerant, abort the loop,
                   and just read a part of the real line (not
                   so bad however). */

                ptrESTR->len=iCurrentL;

                return -1;
        }

        ptrESTR->str=crLf;

        ptrCursor=ptrESTR->str+iCurrentL-1;

    }

    switch (_pBat_NewLine) {

        case PBAT_NEWLINE_WINDOWS :

            pBat_EsReplace(ptrESTR, "\r\n", "\x1\x2\x3\x4");
            pBat_EsReplace(ptrESTR, "\n", "\r\n");
            pBat_EsReplace(ptrESTR, "\r", "\r\n");
            pBat_EsReplace(ptrESTR, "\x1\x2\x3\x4", "\r\n");

            break;

        case PBAT_NEWLINE_LINUX :

            pBat_EsReplace(ptrESTR, "\r\n", "\n");
            pBat_EsReplace(ptrESTR, "\r", "\n");

            break;

        case PBAT_NEWLINE_MAC :

            pBat_EsReplace(ptrESTR, "\r\n", "\r");
            pBat_EsReplace(ptrESTR, "\n", "\r");


    }

    //printf(" Returning\n");

    return !iTotalBytesRead;
}


LIBPBAT int pBat_EsCpy(ESTR* ptrESTR, const char* ptrChaine)
{
    size_t iLen=PBAT_ESTR_SIZE(strlen(ptrChaine) + 1);
    char* ptrBuf=ptrESTR->str;
    if (ptrESTR->len < iLen) {

        if (!(ptrBuf=realloc(ptrBuf, iLen)))
            return -1;

        ptrESTR->str=ptrBuf;
        ptrESTR->len=iLen;
    }

    strcpy(ptrBuf, ptrChaine);

    return 0;
}

LIBPBAT int pBat_EsCpyN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize)
{
    size_t iLen=PBAT_ESTR_SIZE(iSize + 1);
    char* ptrBuf=ptrESTR->str;
    if (ptrESTR->len < iLen)
    {
        if (!(ptrBuf=realloc(ptrBuf, iLen)))
            return -1;

        ptrESTR->str=ptrBuf;
        ptrESTR->len=iLen;

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

LIBPBAT int pBat_EsCat(ESTR* ptrESTR, const char* ptrChaine)
{
   int iLen= PBAT_ESTR_SIZE(strlen(ptrESTR->str) + strlen(ptrChaine) + 1);
   char *lpBuf=ptrESTR->str;

   if ((ptrESTR->len<iLen)) {

        if (!(lpBuf=realloc(lpBuf,iLen)))
            return -1;

        ptrESTR->str=lpBuf;
        ptrESTR->len=iLen;
   }

   strcat(lpBuf, ptrChaine);

   return 0;
}

LIBPBAT int pBat_EsCatN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize)
{
   int iLen=PBAT_ESTR_SIZE(strlen(ptrESTR->str) + iSize + 1);
   char *lpBuf=ptrESTR->str;
   if (ptrESTR->len<iLen) {

        if (!(lpBuf=realloc(lpBuf,iLen)))
            return -1;

        ptrESTR->str=lpBuf;
        ptrESTR->len=iLen;
   }

   strncat(lpBuf, ptrChaine, iSize);

   return 0;
}

LIBPBAT int pBat_EsCpyE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=PBAT_ESTR_SIZE(strlen(ptrSource->str) + 1);
    char* ptrBuf=ptrDest->str;

    if (iLen > ptrDest->len)
    {
        if (!(ptrBuf=realloc(ptrBuf, iLen)))
            return -1;

        ptrDest->str=ptrBuf;
        ptrDest->len=iLen;
    }

    strcpy(ptrBuf, ptrSource->str);

    return 0;
}

LIBPBAT int pBat_EsCatE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=PBAT_ESTR_SIZE(strlen(ptrDest->str) + strlen(ptrSource->str) + 1);
    char* lpBuf=ptrDest->str;

    if (ptrDest->len<iLen) {

        if (!(lpBuf=realloc(lpBuf,iLen)))
            return -1;

        ptrDest->str=lpBuf;
        ptrDest->len=iLen;
    }

    strcat(lpBuf, ptrSource->str);

    return 0;
}

LIBPBAT int pBat_EsReplace(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace)
{
    char* lpBuffer=pBat_EsToChar(ptrESTR), *lpToken;
    int iLength=strlen(ptrPattern);
    ESTR *lpReturn=pBat_EsInit();

    while ((lpToken=strstr(lpBuffer, ptrPattern))) {

        *lpToken='\0';

        pBat_EsCat(lpReturn, lpBuffer);
        pBat_EsCat(lpReturn, ptrReplace);

        lpBuffer=lpToken+iLength;

    }

    pBat_EsCat(lpReturn, lpBuffer);
    pBat_EsCpyE(ptrESTR, lpReturn);
    pBat_EsFree(lpReturn);

    return 0;
}

char * stristr(const char* str, const char *pattern)
{
    char c/*,
         *pattern_start=pattern*/;
    size_t size;

    if (*pattern == '\0')
        return (char *)str;

    c = tolower(*pattern++);
    size = strlen(pattern);

    do {

        while (*str && c != tolower(*str++));

        if (*str == '\0')
            return NULL;

    } while (strnicmp(str, pattern, size));

    return (char *)str;

}

LIBPBAT int pBat_EsReplaceI(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace)
{
    char* lpBuffer=pBat_EsToChar(ptrESTR), *lpToken;
    int iLength=strlen(ptrPattern);
    ESTR *lpReturn=pBat_EsInit();

    while ((lpToken=(char *)stristr(lpBuffer, ptrPattern))) {

        *lpToken='\0';

        pBat_EsCat(lpReturn, lpBuffer);
        pBat_EsCat(lpReturn, ptrReplace);

        lpBuffer=lpToken+iLength;

    }

    pBat_EsCat(lpReturn, lpBuffer);
    pBat_EsCpyE(ptrESTR, lpReturn);
    pBat_EsFree(lpReturn);

    return 0;
}
