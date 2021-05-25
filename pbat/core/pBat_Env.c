/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../errors/pBat_Errors.h"
#include "pBat_Core.h"
#include "pBat_Env.h"

#include <libpBat.h>

int pBat_EnvCompName(const void* p1, const void* p2)
{
    ENVVAR *pVar1=*((ENVVAR**)p1), *pVar2=*((ENVVAR**)p2);

    if ((pVar1->name == NULL) || (pVar2->name == NULL))
        return pVar2->name - pVar1->name;

    return stricmp(pVar1->name, pVar2->name);
}

#define PBAT_ENV_BLOCK_MASK 0x7F

ENVVAR**  pBat_ReAllocEnvBuf(int* nb, ENVVAR** envbuf)
{
    int i=*nb;
    ENVVAR** pNewBuf;

    /* systematically round to the next 128 multiple */
    i = (i & ~PBAT_ENV_BLOCK_MASK) + PBAT_ENV_BLOCK_MASK +1 ;

    if (!(pNewBuf = realloc(envbuf, i*sizeof(ENVVAR*)))) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                __FILE__"/pBat_ReallocEnvBuf()",
                                -1
                                );

    }

    *nb = i;

    return pNewBuf;

}

ENVBUF* pBat_EnvDup(ENVBUF* pBuf)
{
    ENVBUF* pNew;

    int i;

    if (!((pNew = malloc(sizeof(ENVBUF)))
          && (pNew->envbuf = malloc(pBuf->nb*sizeof(ENVVAR*))))) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_EnvDup()",
                                -1);

    }

    pNew->nb = pBuf->nb;

    for (i=0;i < pBuf->index; i++)
        pNew->envbuf[i] = pBat_AllocEnvVar(pBuf->envbuf[i]->name,
                                        pBuf->envbuf[i]->content);

    pNew->index = pBuf->index;

    return pNew;
}

ENVVAR*   pBat_AllocEnvVar(const char* name, const char* content)
{
    ENVVAR *pRet;

    if (!(pRet = malloc(sizeof(ENVVAR)))) {

error:
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_AllocEnvVar()",
                                -1
                                );

    }


    if (name == NULL
        || content == NULL) {

        pRet->content = NULL;
        pRet->name = NULL;

    } else if (!(pRet->name = strdup(name))
            || !(pRet->content = strdup(content)))
        goto error;

    return pRet;
}

void     pBat_AdjustVarName(char* name)
{
    char* pLast=NULL;

    while (*name) {

        if ((*name != '\t') && (*name != ' '))
            pLast = name;

        name ++;
    }

    /* pLast can't be at the end of the string */
    if (pLast)
        *(++ pLast) = '\0';
}

ENVBUF* pBat_InitEnv(char** env)
{
    int nb=0;
    ENVBUF* pEnv;
    ENVVAR** pBuf;
    char   *pStr,
           *pCh;

    /* count the elements in the env array */
    while (env[nb ++] != NULL);

    /* allocate the ENVBUF consequently */
    if (!(pEnv = malloc(sizeof(ENVBUF)))) {

error:
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_InitEnv()",
                                -1);

    }

    pEnv->envbuf = pBat_ReAllocEnvBuf(&nb, NULL);

    pEnv->nb = nb;
    pBuf = pEnv->envbuf;

    nb = 0;

    /* fill environment buffer */
    while (env[nb] != NULL) {

        if (!(pStr=strdup(env[nb])))
            goto error;

        if (!(pCh = strchr(pStr, '='))) {

            pBat_ShowErrorMessage(PBAT_UNABLE_SET_ENVIRONMENT,
                                    pStr,
                                    -1);

        }

        pBat_AdjustVarName(pStr);

        *(pCh++) = '\0';

        pBuf[nb] = pBat_AllocEnvVar(pStr, pCh);

        free(pStr);

        nb ++;

    }

    pEnv->index = nb;

    /* sort the array, this is little time wasting, but it is far
       simpler to retrieve variables */

    qsort(pEnv->envbuf, nb, sizeof(ENVVAR*), pBat_EnvCompName);

    return pEnv;

}

/* Get the content of a environment variable.

    - name : The name of variable. If now variable with the name is
      fount it returns NULL.

*/
char* pBat_GetEnv(ENVBUF* pEnv, const char* name)
{
    ENVVAR **pRes,
            key={(char*)name, NULL},
            *pKey=&key;

    //printf ("looking for =\"%s\"\n", name)

    pRes = bsearch(&pKey, pEnv->envbuf, pEnv->index, sizeof(ENVVAR*),
                                                    pBat_EnvCompName);

    //printf ("got : pRes->")

    if (!pRes)
        return NULL;

    return (*pRes)->content;
}

/* Set an environment variable.

        name : the name of the variable to be set. name may contain any
        utf-8 charcter, including the equal sign. Remaining spaces and
        tab at the end of name are removed.

        content : the content of the variable to be set. If NULL, content
        is ignored and the variable is deleted from the environement.

 */
void pBat_SetEnv(ENVBUF* pEnv, char* name, const char* content)
{
    ENVVAR **pRes,
            key,
            *pKey=&key;

    char*   namecpy;

    /* int i; */

    if (content == NULL || *content == '\0') {

        pBat_UnSetEnv(pEnv, name);
    	return;

    }

    if (!(namecpy = strdup(name)))
        goto error;

    pBat_AdjustVarName(namecpy);

    key.content = NULL;
    key.name = namecpy;

    pRes = bsearch(&pKey, pEnv->envbuf, pEnv->index, sizeof(ENVVAR**),
                                                    pBat_EnvCompName);

    if (pRes) {

        /* A variable with this name is already allocated, just
           change the content field */
        free(namecpy);

        free((*pRes)->content);

        if (!((*pRes)->content=strdup(content))) {
error:
            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_SetEnv()",
                                    -1
                                    );

        }

        return;

    }

    /* try to get an unused ENVVAR struct*/

    key.name = NULL;

    pRes = bsearch(&pKey, pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), pBat_EnvCompName);

    if (pRes) {

        if (!((*pRes)->content = strdup(content)))
            goto error;

        (*pRes)->name = namecpy;

    } else {

        /* create a new variable */

        if ((pEnv->index) == pEnv->nb) {

            /* it will not fit in the array just extent it */
            pEnv->nb ++;

            pEnv->envbuf = pBat_ReAllocEnvBuf(&(pEnv->nb), pEnv->envbuf);

        }

        if (!(pEnv->envbuf[pEnv->index] = malloc(sizeof(ENVVAR))))
            goto error;

        if (!(pEnv->envbuf[pEnv->index]->content = strdup(content)))
            goto error;

        pEnv->envbuf[pEnv->index]->name = namecpy;
        ++ pEnv->index;

    }

    qsort(pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), pBat_EnvCompName);

    return;
}

/* Remove variable from environment

       - name : The name of the variable to be removed from the environment.
         name cannot be NULL, unless the behaviour is undefined.
*/

void  pBat_UnSetEnv(ENVBUF* pEnv, char* name)
{
    ENVVAR **pRes,
            key={name, NULL},
            *pKey = &key;

    pRes = bsearch(&pKey, pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), pBat_EnvCompName);

    if (pRes) {

        free((*pRes)->name);
        free((*pRes)->content);

        (*pRes)->name = NULL;
        (*pRes)->content = NULL;

        qsort(pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), pBat_EnvCompName);

    }
}

void pBat_EnvFree(ENVBUF* pEnv)
{
    int i;

    /* free vars */
    for (i=0;i < pEnv->index; i++) {

        if (pEnv->envbuf[i]->name)
            free(pEnv->envbuf[i]->name);

        if (pEnv->envbuf[i]->content)
            free(pEnv->envbuf[i]->content);

        free(pEnv->envbuf[i]);

    }

    free(pEnv->envbuf);
}

void pBat_ApplyEnv(ENVBUF* pEnv)
{
    int i;
    ESTR* exp = pBat_EsInit();

    for (i=0; i < pEnv->index; i++) {

        if (pEnv->envbuf[i]->name == NULL)
            continue;

        #if !defined(WIN32)
        setenv(pEnv->envbuf[i]->name, pEnv->envbuf[i]->content, 1);
        #elif defined(WIN32)
        SetEnvironmentVariable(pEnv->envbuf[i]->name,
                                            pEnv->envbuf[i]->content);
        #endif

    }

    pBat_EsFree(exp);
}

void pBat_UnApplyEnv(ENVBUF* pEnv)
{
    int i;
    ESTR* exp = pBat_EsInit();

    for (i=0; i < pEnv->index; i++) {

        if (pEnv->envbuf[i]->name == NULL)
            continue;

        #if !defined(WIN32)
        setenv(pEnv->envbuf[i]->name, NULL, 1);
        #elif defined(WIN32)
        SetEnvironmentVariable(pEnv->envbuf[i]->name,
                                            NULL);
        #endif

    }

    pBat_EsFree(exp);
}


void* pBat_GetEnvBlock(ENVBUF* pEnv, size_t *s)
{
    int i = 0;
    void *block, *p;
    size_t size = 0;

    for (i = 0; i < pEnv->index; i++) {

        if (pEnv->envbuf[i]->name == NULL)
            continue;

        pEnv->envbuf[i]->size = strlen(pEnv->envbuf[i]->content)
                + strlen(pEnv->envbuf[i]->name) + 2;
        size += pEnv->envbuf[i]->size;

    }

    size ++;

    if (!(block = malloc(size)))
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION |
                                    PBAT_PRINT_C_ERROR,
                                __FILE__ "/pBat_GetEnvBlock()", -1);

    p = block;

    for (i = 0; i < pEnv->index; i++) {

        if (pEnv->envbuf[i]->name == NULL)
            continue;

        strcpy(p, pEnv->envbuf[i]->name);
        strcat(p, "=");
        strcat(p, pEnv->envbuf[i]->content);

        p += pEnv->envbuf[i]->size;

    }

    *((char*)p) = '\0';
    *s = size;

    return block;
}
