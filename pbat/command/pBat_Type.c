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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Type.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

/* TYPE file

   Type the file 'file'

*/

void pBat_TypeFileF(FILE* pFile)
{
    char buf[1024];
    size_t size;

    while ((size = fread(buf, 1, sizeof(buf), pFile)))
        fwrite(buf, 1, size, fOutput);

    if (isatty(fileno(pFile)))
        clearerr(pFile);
}

int pBat_TypeFile(char* lpFileName)
{
    FILE* pFile;
    char buf[1024];
    size_t size;

    if (!(pFile=fopen(lpFileName, "rb"))) {

        pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                lpFileName,
                                FALSE
                                );

        return PBAT_FILE_ERROR;
    }

    while ((size = fread(buf, 1, sizeof(buf), pFile)))
        fwrite(buf, 1, size, fOutput);

    fclose(pFile);

    return 0;
}

int pBat_CmdType(char* lpLine)
{
	ESTR* lpEsParam=pBat_EsInit_Cached();

	FILELIST *pBegin=NULL,
             *pTmp=NULL,
             *pEnd;

    int status=0, nSize;
    char *buf;


    lpLine += 4;

    /* adjust buffering size to get maximum performances */
    if (isatty(fileno(fOutput))) {

        if ((buf = malloc(8192)) == NULL) {

            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_CmdType()",
                                    0);
            status = PBAT_FAILED_ALLOCATION;
            goto end;

        }

        setvbuf(fOutput, buf, _IOFBF, 8192);

    }

    while ((lpLine = pBat_GetNextParameterEs(lpLine, lpEsParam))) {

        if (!strcmp(pBat_EsToChar(lpEsParam), "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_TYPE);

            goto end;

        } else  {

            /* Get a list of files that matches the argument */
            if (!(pTmp = pBat_GetMatchFileList(pBat_EsToFullPath(lpEsParam),
                                                    PBAT_SEARCH_DEFAULT))) {

                pBat_ShowErrorMessage(PBAT_NO_MATCH,
                                       pBat_EsToChar(lpEsParam),
                                       FALSE
                                       );

                status = PBAT_NO_MATCH;
                goto end;

            }

            if (!pBegin)
                pBegin = (pEnd = pTmp);
            else
                pEnd->lpflNext = pTmp; /* catenate the lists */

        }

    }

    if (pTmp == NULL) {
        /* If the command has no parameter */
        pBat_TypeFileF(fInput);

    } else {

        /* remove directories from input */
        pBat_AttributesSplitFileList(PBAT_ATTR_NO_DIR,
                                        pBegin,
                                        &pBegin,
                                        &pTmp
                                        );

        if (pTmp)
            pBat_FreeFileList(pTmp);

        pEnd = pBegin;

        while (pEnd && pEnd->lpflNext)
            pEnd = pEnd->lpflNext;

        if (!pBegin) {

            pBat_ShowErrorMessage(PBAT_NO_VALID_FILE,
                                    "TYPE",
                                    FALSE
                                    );

            status = PBAT_NO_VALID_FILE;
            goto end;

        }

        if (pEnd == pBegin) {

            /* only one file matching */
            status = pBat_TypeFile(pBegin->lpFileName);

        } else {

            /* several files matching */
            pTmp = pBegin;

            while (pTmp) {

                fprintf(fOutput, "---------- %s" PBAT_NL , pTmp->lpFileName);
                status |= pBat_TypeFile(pTmp->lpFileName);

                pTmp = pTmp->lpflNext;

            }

        }
    }

end:

    if (isatty(fileno(fOutput))) {

        fflush(fOutput);
        setvbuf(fOutput, NULL, _IONBF, 0);

        free(buf);

    }

    if (pBegin)
        pBat_FreeFileList(pBegin);

    pBat_EsFree_Cached(lpEsParam);

    return status;
}
