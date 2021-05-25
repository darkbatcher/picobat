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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <libpBat.h>

//#define PBAT_DBG_MODE
#include "pBat_Debug.h"

#include "pBat_Core.h"
#include "pBat_FilePath.h"

int pBat_GetFileFullPath(char* full, const char* p, size_t size)
{
    char* partial = TRANS(p);

    if (PBAT_TEST_ABSOLUTE_PATH(partial)) {

#ifdef WIN32
        if (PBAT_TEST_ROOT_PATH(partial)) {
            snprintf(full, size, "%c:\\%s", *lpCurrentDir, partial + 1);

            if (!pBat_FileExists(full))
                return -1;
        }
#endif // WIN32

        if (!pBat_FileExists(partial))
            return -1;

        strncpy(full, partial, size);
        full[size - 1] = '\0';

        return 0;
    }

    snprintf(full, size, "%s" PBAT_DEF_DELIMITER "%s", lpCurrentDir, partial);

    if (!pBat_FileExists(full))
        return -1;

    return 0;
}

void __inline__ pBat_MakeFullPath(char* full, const char* p, size_t size)
{
    char* partial = TRANS(p);

    if (PBAT_TEST_ABSOLUTE_PATH(partial)) {

#ifdef WIN32
        if (PBAT_TEST_ROOT_PATH(partial)) {
            snprintf(full, size, "%c:\\%s", *lpCurrentDir, partial + 1);
            return;
        }
#endif // WIN32

        strncpy(full, partial, size);
        full[size - 1] = '\0';
        return;
    }

    snprintf(full, size, "%s" PBAT_DEF_DELIMITER "%s", lpCurrentDir, partial);
}

void __inline__ pBat_MakeFullPathEs(ESTR* full, const char* p)
{
    char begin[] = "c:/";
    char* partial = TRANS(p);

    if (PBAT_TEST_ABSOLUTE_PATH(partial)) {

#ifdef WIN32
        if (PBAT_TEST_ROOT_PATH(partial)) {

            begin[0] = *lpCurrentDir;
            pBat_EsCpy(full, begin);
            pBat_EsCat(full, partial + 1);
            return;

        }
#endif // WIN32

        pBat_EsCpy(full, partial);
        return;
    }

    pBat_EsCpy(full, lpCurrentDir);
    pBat_EsCat(full, PBAT_DEF_DELIMITER);
    pBat_EsCat(full, partial);
}

__inline__ char* pBat_EsToFullPath(ESTR* full)
{
    size_t len, size;
    const char* p = TRANS(full->str);

    if (PBAT_TEST_ABSOLUTE_PATH(p)) {

#ifdef WIN32
        if (PBAT_TEST_ROOT_PATH(full->str)) {
            /* we need at least two more bytes */
            len = strlen(p) + 1;

            if (len + 2 > full->len) {
                full->len *= 2;

                if (!(full->str = realloc(full->str, full->len)))
                    pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION |
                                            PBAT_PRINT_C_ERROR,
                                            __FILE__ "/pBat_EsToFullPath()",
                                            -1);

            }

            memmove(full->str + 2, p, len);
            *(full->str) = *lpCurrentDir;
            *(full->str) = ':';

        }
#endif // WIN32

        /* do nothing */
        return full->str;
    }

    /* need to cat with a lpCurrentDir */
    len = strlen(p) + 1;
    size = strlen(lpCurrentDir);

    if (len + size + 1 > full->len) {
        full->len *= (((len + size + 1 ) / (full->len)) + 1);

        if (!(full->str = realloc(full->str, full->len)))
                    pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION |
                                            PBAT_PRINT_C_ERROR,
                                            __FILE__ "/pBat_EsToFullPath()",
                                            -1);
    }

    memmove(full->str + size + 1, p, len);
    memcpy(full->str, lpCurrentDir, size);
    *(full->str + size) = *(PBAT_DEF_DELIMITER); /* DEF_DELIMITER is a string literal */

    return full->str;
}

__inline__ char* pBat_FullPathDup(const char* p)
{
    char *ret, *path = TRANS(p);
    size_t needed = strlen(path) + 1;

    if (PBAT_TEST_ABSOLUTE_PATH(path)) {
        /* this is already absolute */

#ifdef WIN32
        if (PBAT_TEST_ROOT_PATH(path)) {
            ret = malloc(2 + needed);

            if (!ret)
                return NULL;

            *ret = *lpCurrentDir;
            *(ret + 1) = ':';
            strcat(ret, path);

            return ret;
        }
#endif // WIN32

        ret = malloc(needed);

        if (!ret)
            return NULL;

        strcpy(ret, path);
        return ret;

    } else {
        /* this has to be converted to absolute */
        needed += strlen(lpCurrentDir) + 1;

        ret = malloc(needed);

        if (!ret)
            return NULL;

        strcpy(ret, lpCurrentDir);
        strcat(ret, PBAT_DEF_DELIMITER);
        strcat(ret, path);

        return ret;
    }
}

int pBat_GetFilePath(char* lpFullPath, const char* lpPartial, size_t iBufSize)
{
	ESTR* lpEsTmp=pBat_EsInit();
	ESTR* lpEsPart=pBat_EsInit();
	ESTR* lpEsFinalPath=pBat_EsInit();

	char *lpPathToken=pBat_GetEnv(lpeEnv, "PATH");

	int   bFirstLoop=TRUE, bLoop=TRUE;

	char *lpPathExtToken,
	     *lpPathExtBegin=pBat_GetEnv(lpeEnv, "PATHEXT");
	int bFinalSubLoop;

	if (lpPathExtBegin == NULL)
        lpPathExtBegin="";

	if (PBAT_TEST_ABSOLUTE_PATH(lpPartial)
        || strpbrk(lpPartial, PBAT_DEF_DELIMITERS)) {
		/* if the path is already absolute, or if the path include
		   directory delimiters, do not loop through %PATH% listed
		   files. */
        bLoop = FALSE;

	}

/* next: */
	do {

		if (bFirstLoop) {

			pBat_MakeFullPathEs(lpEsTmp, lpPartial);
			bFirstLoop=FALSE;

		} else {

			pBat_MakePath(lpEsTmp, 2, pBat_EsToChar(lpEsPart), lpPartial);

		}

		bFinalSubLoop=TRUE;
		lpPathExtToken=lpPathExtBegin;

		while ((lpPathExtToken=pBat_GetPathNextPart(lpPathExtToken, lpEsPart))
                || bFinalSubLoop) {

			if (lpPathExtToken != NULL) {

                pBat_EsCpyE(lpEsFinalPath, lpEsTmp);
				pBat_EsCatE(lpEsFinalPath, lpEsPart);

			} else if (bFinalSubLoop) {

				pBat_EsCpyE(lpEsFinalPath, lpEsTmp);
				bFinalSubLoop=FALSE;

			}

			PBAT_DBG("\t\t[*] trying \"%s\"\n", pBat_EsToChar(lpEsFinalPath));

			if (pBat_FileExists(pBat_EsToChar(lpEsFinalPath)))
				goto file_found;

		}

		if (lpPathToken == NULL)
			break;


	} while ((lpPathToken=pBat_GetPathNextPart(lpPathToken, lpEsPart))
          && bLoop);

	pBat_EsFree(lpEsPart);
	pBat_EsFree(lpEsTmp);
	pBat_EsFree(lpEsFinalPath);


	return -1;

file_found:

	snprintf(lpFullPath ,
	         iBufSize,
	         "%s",
	         pBat_EsToChar(lpEsFinalPath)
	        );

	pBat_EsFree(lpEsPart);
	pBat_EsFree(lpEsTmp);
	pBat_EsFree(lpEsFinalPath);

	return 0;
}


char* pBat_GetPathNextPart(char* lpPath, ESTR* lpReturn)
{
	char*   lpNextToken;

	if (lpPath == NULL || *lpPath=='\0')
		return NULL;

	if (!(lpNextToken=strchr(lpPath, PBAT_PATH_DELIMITER))) {

		pBat_EsCpy(lpReturn, lpPath);

		while (*lpPath)
			lpPath++;

		return lpPath;

	}

	pBat_EsCpyN(lpReturn,
	            lpPath,
	            lpNextToken - lpPath);

	return ++lpNextToken;
}

int pBat_MakePath(ESTR* lpReturn, int nOps, ...)
{
	va_list vaList;

	int     i;

	char   *lpBegin,
	       *lpEnd;

	va_start(vaList, nOps);

	*pBat_EsToChar(lpReturn) = '\0';

	for (i=0; i<nOps; i++) {

		lpBegin=pBat_EsToChar(lpReturn);

		lpEnd=pBat_GetLastChar(lpReturn);

		if (*lpBegin!='\0'
#ifdef WIN32
		    && *lpEnd!='\\'
#endif
		    && *lpEnd!='/') {

			/* if there are no dir terminating characters and still
			   arguments, just cat a '/' */

			pBat_EsCat(lpReturn, PBAT_DEF_DELIMITER);

		}

		pBat_EsCat(lpReturn, va_arg(vaList,char*));

	}

	va_end(vaList);

	return 0;
}

char* pBat_GetLastChar(ESTR* lpReturn)
{

	char* lpCh=pBat_EsToChar(lpReturn);

	if (*lpCh=='\0')
		return lpCh;

	while (*(lpCh+1))
		lpCh++;

	return lpCh;

}
