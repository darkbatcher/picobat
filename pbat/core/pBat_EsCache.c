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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "pBat_Core.h"
#include "pBat_EsCache.h"

#define DEFAULT_ESTR_SIZE 256

/* Caching ESTR buffers may be a good idea in some parts of the code. */

int pBat_EsCacheBuild(ESTRCACHE *lpecCache)
{
  for (size_t i = 0; i < TAG_LATEST; i++) {
    lpecCache->lpesPool[i].str = malloc(DEFAULT_ESTR_SIZE);

    if (lpecCache->lpesPool[i].str) {
      lpecCache->lpesPool[i].len = DEFAULT_ESTR_SIZE;
      lpecCache->lpesPool[i].str[0] = '\0';

      lpecCache->lpbStatus[i] = 0;
    } else {
      /* If this malloc fails, it's probably going to fail somewhere else, but
         it will not prevent us to build the cache but those cache entries will
         be unusable.
      */
      printf("Warning: Failed to build EsCache[%zu]\n", i);

      lpecCache->lpesPool[i].str = NULL;
      lpecCache->lpbStatus[i] = 1;
    }
  }
}

void pBat_EsCacheDrop(ESTRCACHE *lpecCache)
{
  for (size_t i = 0; i < TAG_LATEST; i++) {
    /* Prevent any use of a cached estr */
    lpecCache->lpbStatus[i] = 1;

    if (lpecCache->lpesPool[i].str)
      free(lpecCache->lpesPool[i].str);
  }
}

ESTR *pBat_EsCacheInit(ESTRCACHE *lpecCache, enum EstrTag tag)
{
  assert(tag < TAG_LATEST);

  if (lpecCache->lpbStatus[tag])
    /* Fallback to EsInit() */
    return pBat_EsInit();
  else {
    /* Use cached ESTR */
    lpecCache->lpbStatus[tag] = 1;
    return &lpecCache->lpesPool[tag];
  }
}

void pBat_EsCacheFree(ESTRCACHE *lpecCache, ESTR *lpEstr)
{
  /* The provided lpEstr may not be in cache in case of the fallback with EsInit.
     Use pointer arithmetic to check if lpEstr is an element of lpecCache->lpesPool. */
  
  /* NOTE: TAG_LATEST is not a valid tag */
  if (
      ((uintptr_t)&lpecCache->lpesPool[0] <= (uintptr_t)lpEstr)
      && ((uintptr_t)&lpecCache->lpesPool[TAG_LATEST] > (uintptr_t)lpEstr)
   ) {
    /* The pointer is in the cache. */
    size_t iTag = lpEstr - lpecCache->lpesPool;

    /* Prepare tag for next usage. */
    lpecCache->lpbStatus[iTag] = 0;

    if (lpecCache->lpesPool[iTag].len != DEFAULT_ESTR_SIZE) {
      /* resize if needed (len different from default size) */
      void *resized_buffer = realloc(lpecCache->lpesPool[iTag].str, DEFAULT_ESTR_SIZE);

      if (resized_buffer) {
        lpecCache->lpesPool[iTag].str = resized_buffer;
        lpecCache->lpesPool[iTag].len = DEFAULT_ESTR_SIZE;
      } else {
        /* In case of failure, we can still keep the old buffer, but report a warning to the user. */
        fprintf(fError, "Warning: Failed to resize EsCache buffer of tag (%zu)\n", iTag);
      }
    }

    if (lpecCache->lpesPool[iTag].len > 0)
      lpecCache->lpesPool[iTag].str[0] = '\0';
  } else {
    /* regular estr */
    pBat_EsFree(lpEstr);
  }
}