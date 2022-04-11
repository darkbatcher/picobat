/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2022 Astie Teddy
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

#ifndef PBAT_ES_CACHE_H
#define PBAT_ES_CACHE_H

#include <stdint.h>

#include "pBat_Core.h"

#define ESTR_CACHE_SIZE 128

typedef struct ESTRCACHE {
    ESTR lpesPool[ESTR_CACHE_SIZE]; /* ESTR buffers */
    uint32_t lpbStatus[ESTR_CACHE_SIZE / 32]; /* ESTR buffers status bitmap (1 if used) */
} ESTRCACHE;

int pBat_EsCacheBuild(ESTRCACHE *lpecCache);
void pBat_EsCacheDrop(ESTRCACHE *lpecCache);
ESTR *pBat_EsCacheInit(ESTRCACHE *lpecCache);
void pBat_EsCacheFree(ESTRCACHE *lpecCache, ESTR *lpEstr);

/* Cached variants of pBat_EsInit() with a corresponding pBat_EsFree
   Faster than regular pBat_EsInit() with less likely out of memory errors, 
   but must not be transferred between picobat instances.
*/
#define pBat_EsInit_Cached() pBat_EsCacheInit(&ecEstrCache)
#define pBat_EsFree_Cached(estr) pBat_EsCacheFree(&ecEstrCache, estr)

#endif /* PBAT_ES_CACHE_H */