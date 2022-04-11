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

#define ESTR_STATUS_ELEM_COUNT (ESTR_CACHE_SIZE / 32)

#define BIT(x) (1 << (x))

#define STATUS_ON(x) lpecCache->lpbStatus[(x) / 32] |= BIT(x % 32)
#define STATUS_OFF(x) lpecCache->lpbStatus[(x) / 32] &= ~BIT(x % 32)

int pBat_EsCacheBuild(ESTRCACHE *lpecCache)
{
  /* Consider all status bits unused. */
  for (size_t i = 0; i < ESTR_STATUS_ELEM_COUNT; i++)
    lpecCache->lpbStatus[i] = ~(uint32_t)0;
  
  /* Allocate all ESTR */
  for (size_t i = 0; i < ESTR_CACHE_SIZE; i++) {
    lpecCache->lpesPool[i].str = malloc(DEFAULT_ESTR_SIZE);

    if (lpecCache->lpesPool[i].str) {
      lpecCache->lpesPool[i].len = DEFAULT_ESTR_SIZE;
      lpecCache->lpesPool[i].str[0] = '\0';

      STATUS_OFF(i);
    } else {
      /* If this malloc fails, it's probably going to fail somewhere else, but
         it will not prevent us to build the cache but those cache entries will
         be unusable.
      */
      printf("Warning: Failed to build EsCache[%zu]\n", i);
    }
  }
}

void pBat_EsCacheDrop(ESTRCACHE *lpecCache)
{
  /* Prevent any use of a cached estr */
  for (size_t i = 0; i < ESTR_STATUS_ELEM_COUNT; i++)
    lpecCache->lpbStatus[i] = ~(uint32_t)0;
  
  for (size_t i = 0; i < ESTR_CACHE_SIZE; i++)
    if (lpecCache->lpesPool[i].str)
      free(lpecCache->lpesPool[i].str);
}

ESTR *pBat_EsCacheInit(ESTRCACHE *lpecCache)
{
  /* Find a available ESTR from cache. */
  for (size_t i = 0; i < ESTR_STATUS_ELEM_COUNT; i++) {
    register uint32_t status = lpecCache->lpbStatus[i];

    if (status != ~(uint32_t)0) {

      /* At least one bit is unused. */
      for (size_t j = 0; j < 32; j++) {
        if (!(status & 0x1)) {
          /* j bit of lpecCache->lpbStatus[i] is 0 */
          size_t cache_index = i * 32 + j;

          STATUS_ON(cache_index);
          return &lpecCache->lpesPool[cache_index];
        }

        status >>= 1;
      }
    }
  }

  /* Fallback to EsInit */
  return pBat_EsInit();
}

void pBat_EsCacheFree(ESTRCACHE *lpecCache, ESTR *lpEstr)
{
  /* The provided lpEstr may not be in cache in case of the fallback with EsInit.
     Use pointer arithmetic to check if lpEstr is an element of lpecCache->lpesPool. */
    if (
      ((uintptr_t)&lpecCache->lpesPool[0] <= (uintptr_t)lpEstr)
      && ((uintptr_t)&lpecCache->lpesPool[ESTR_CACHE_SIZE] > (uintptr_t)lpEstr)
   ) {
    /* The pointer is in the cache. */
    size_t cache_index = lpEstr - lpecCache->lpesPool;

    /* Prepare tag for next usage. */
    STATUS_OFF(cache_index);

    if (lpecCache->lpesPool[cache_index].len != DEFAULT_ESTR_SIZE) {
      /* resize if needed (len different from default size) */
      void *resized_buffer = realloc(lpecCache->lpesPool[cache_index].str, DEFAULT_ESTR_SIZE);

      if (resized_buffer) {
        lpecCache->lpesPool[cache_index].str = resized_buffer;
        lpecCache->lpesPool[cache_index].len = DEFAULT_ESTR_SIZE;
      } else {
        /* In case of failure, we can still keep the old buffer, but report a warning to the user. */
        fprintf(fError, "Warning: Failed to resize EsCache buffer of tag (%zu)\n", cache_index);
      }
    }

    if (lpecCache->lpesPool[cache_index].len > 0)
      lpecCache->lpesPool[cache_index].str[0] = '\0';
  } else {
    /* regular estr */
    pBat_EsFree(lpEstr);
  }
}