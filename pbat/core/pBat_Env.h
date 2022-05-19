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
#ifndef PBAT_ENV_H
#define PBAT_ENV_H

#include <stdlib.h>

/* Definition of struct that represent an environment variable */
typedef struct ENVVAR {
    char* name; /* a pointer to the name of the variable, this storage
                   way allow having equal signs within the name, though
                   it is not portable on most platforms through standard
                   libC calls */
    char* content; /* a pointer to the content of the variable */
    struct ENVVAR* previous;
    int inherit;
    size_t size; /* size for elaborating blocks */
} ENVVAR;

/* definition of stucture that represent an environment buffer */
typedef struct ENVBUF {
    int nb; /* number of elements of the environment buffer */
    int index; /* number of used elements in the buffer */
    ENVVAR** envbuf; /* environment buffer */
} ENVBUF;

/*  pBat also provide functions for environment variables, to be able to
    run various threads */

ENVVAR**  pBat_ReAllocEnvBuf(int* nb, ENVVAR** envbuf);
ENVVAR*   pBat_AllocEnvVar(const char* name, const char* content);

void     pBat_AdjustVarName(char* name);

ENVBUF* pBat_InitEnv(char** env);
ENVBUF* pBat_EnvDup(ENVBUF* pBuf);

char* pBat_GetEnv(ENVBUF* pEnv, const char* name);
void  pBat_SetEnvEx(ENVBUF* pEnv, char* name, const char* content, int inherit);
#define pBat_SetEnv(p,n,c) pBat_SetEnvEx(p,n,c,0)
void  pBat_UnSetEnv(ENVBUF* pEnv, char* name);

void* pBat_GetEnvBlock(ENVBUF* pEnv, size_t *s);

void pBat_ApplyEnv(ENVBUF* pEnv);
void pBat_UnApplyEnv(ENVBUF* pEnv);

void pBat_EnvFree(ENVBUF* pEnv);

#define PBAT_ENV_LOCAL_PREFIX '_'

void pBat_PushEnvLocals(ENVBUF *pEnv);
void pBat_PopEnvLocals(ENVBUF *pEnv);

#endif
