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

#include "pBat_Core.h"

/* List of all tags to be used */
enum EstrTag {
    TAG_DELAYED_EXPAND_BUF0 = 0,
    TAG_DELAYED_EXPAND_BUF1,
    TAG_RUNBLOCK_LINE,
    TAG_RUNBLOCK_BLOCK,
    TAG_PARSE_OUTPUT_FINAL,
    TAG_PARSE_OUTPUT_PARAM,
    TAG_BLOCK_NEXT_BLOCK,
    TAG_CALL_PARAMETER,
    TAG_CALL_LABEL,
    TAG_CALL_FILE,
    TAG_CALL_CALLFILE_PARAM,
    TAG_CALL_CALLEXTERNAL_LINE,
    TAG_CALL_CALLEXTERNAL_CMD,
    TAG_ECHO_PARAMETER,
    TAG_ARGS_PARAMETER,
    TAG_PARSE_CMDLINE1,
    TAG_LATEST // also tag count
};

typedef struct ESTRCACHE {
    ESTR lpesPool[TAG_LATEST]; /* ESTR buffers */
    _Bool lpbStatus[TAG_LATEST]; /* ESTR buffers status (true is used) */
} ESTRCACHE;

int pBat_EsCacheBuild(ESTRCACHE *lpecCache);
void pBat_EsCacheDrop(ESTRCACHE *lpecCache);
ESTR *pBat_EsCacheInit(ESTRCACHE *lpecCache, enum EstrTag tag);
void pBat_EsCacheFree(ESTRCACHE *lpecCache, ESTR *lpEstr);

#define pBat_EsInit_Cached(tag) pBat_EsCacheInit(&ecEstrCache, tag)
#define pBat_EsFree_Cached(estr) pBat_EsCacheFree(&ecEstrCache, estr)

#endif /* PBAT_ES_CACHE_H */