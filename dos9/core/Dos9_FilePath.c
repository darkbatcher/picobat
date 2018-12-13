/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <libDos9.h>

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#include "Dos9_Core.h"
#include "Dos9_FilePath.h"

int Dos9_GetFileFullPath(char* full, const char* p, size_t size)
{
    char* partial = TRANS(p);

    if (DOS9_TEST_ABSOLUTE_PATH(partial)) {

#ifdef WIN32
        if (DOS9_TEST_ROOT_PATH(partial)) {
            snprintf(full, size, "%c:\\%s", *lpCurrentDir, partial + 1);

            if (!Dos9_FileExists(full))
                return -1;
        }
#endif // WIN32

        if (!Dos9_FileExists(partial))
            return -1;

        strncpy(full, partial, size);
        full[size - 1] = '\0';

        return 0;
    }

    snprintf(full, size, "%s" DOS9_DEF_DELIMITER "%s", lpCurrentDir, partial);

    if (!Dos9_FileExists(full))
        return -1;

    return 0;
}

void __inline__ Dos9_MakeFullPath(char* full, const char* p, size_t size)
{
    char* partial = TRANS(p);

    if (DOS9_TEST_ABSOLUTE_PATH(partial)) {

#ifdef WIN32
        if (DOS9_TEST_ROOT_PATH(partial)) {
            snprintf(full, size, "%c:\\%s", *lpCurrentDir, partial + 1);
            return;
        }
#endif // WIN32

        strncpy(full, partial, size);
        full[size - 1] = '\0';
        return;
    }

    snprintf(full, size, "%s" DOS9_DEF_DELIMITER "%s", lpCurrentDir, partial);
}

void __inline__ Dos9_MakeFullPathEs(ESTR* full, const char* p)
{
    char begin[] = "c:/";
    char* partial = TRANS(p);

    if (DOS9_TEST_ABSOLUTE_PATH(partial)) {

#ifdef WIN32
        if (DOS9_TEST_ROOT_PATH(partial)) {

            begin[0] = *lpCurrentDir;
            Dos9_EsCpy(full, begin);
            Dos9_EsCat(full, partial + 1);
            return;

        }
#endif // WIN32

        Dos9_EsCpy(full, partial);
        return;
    }

    Dos9_EsCpy(full, lpCurrentDir);
    Dos9_EsCat(full, DOS9_DEF_DELIMITER);
    Dos9_EsCat(full, partial);
}

__inline__ char* Dos9_EsToFullPath(ESTR* full)
{
    size_t len, size;
    const char* p = TRANS(full->str);

    if (DOS9_TEST_ABSOLUTE_PATH(p)) {

#ifdef WIN32
        if (DOS9_TEST_ROOT_PATH(full->str)) {
            /* we need at least two more bytes */
            len = strlen(p) + 1;

            if (len + 2 > full->len) {
                full->len *= 2;

                if (!(full->str = realloc(full->str, full->len)))
                    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION |
                                            DOS9_PRINT_C_ERROR,
                                            __FILE__ "/Dos9_EsToFullPath()",
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
                    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION |
                                            DOS9_PRINT_C_ERROR,
                                            __FILE__ "/Dos9_EsToFullPath()",
                                            -1);
    }

    memmove(full->str + size + 1, p, len);
    memcpy(full->str, lpCurrentDir, size);
    *(full->str + size) = *(DOS9_DEF_DELIMITER); /* DEF_DELIMITER is a string literal */

    return full->str;
}

__inline__ char* Dos9_FullPathDup(const char* p)
{
    char *ret, *path = TRANS(p);
    size_t needed = strlen(path) + 1;

    if (DOS9_TEST_ABSOLUTE_PATH(path)) {
        /* this is already absolute */

#ifdef WIN32
        if (DOS9_TEST_ROOT_PATH(path)) {
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
        strcat(ret, DOS9_DEF_DELIMITER);
        strcat(ret, path);

        return ret;
    }
}

int Dos9_GetFilePath(char* lpFullPath, const char* lpPartial, size_t iBufSize)
{
	ESTR* lpEsTmp=Dos9_EsInit();
	ESTR* lpEsPart=Dos9_EsInit();
	ESTR* lpEsFinalPath=Dos9_EsInit();

	char *lpPathToken=Dos9_GetEnv(lpeEnv, "PATH");

	int   bFirstLoop=TRUE, bLoop=TRUE;

	char *lpPathExtToken,
	     *lpPathExtBegin=Dos9_GetEnv(lpeEnv, "PATHEXT");
	int bFinalSubLoop;

	if (lpPathExtBegin == NULL)
        lpPathExtBegin="";

	if (DOS9_TEST_ABSOLUTE_PATH(lpPartial)) {
		/* if the path is already absolute */
        bLoop = FALSE;

	}

/* next: */
	do {

		if (bFirstLoop) {

			Dos9_MakeFullPathEs(lpEsTmp, lpPartial);
			bFirstLoop=FALSE;

		} else {

			Dos9_MakePath(lpEsTmp, 2, Dos9_EsToChar(lpEsPart), lpPartial);

		}

		bFinalSubLoop=TRUE;
		lpPathExtToken=lpPathExtBegin;

		while ((lpPathExtToken=Dos9_GetPathNextPart(lpPathExtToken, lpEsPart))
                || bFinalSubLoop) {

			if (lpPathExtToken != NULL) {

                Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
				Dos9_EsCatE(lpEsFinalPath, lpEsPart);

			} else if (bFinalSubLoop) {

				Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
				bFinalSubLoop=FALSE;

			}

			DOS9_DBG("\t\t[*] trying \"%s\"\n", Dos9_EsToChar(lpEsFinalPath));

			if (Dos9_FileExists(Dos9_EsToChar(lpEsFinalPath)))
				goto file_found;

		}

		if (lpPathToken == NULL)
			break;


	} while ((lpPathToken=Dos9_GetPathNextPart(lpPathToken, lpEsPart))
          && bLoop);

	Dos9_EsFree(lpEsPart);
	Dos9_EsFree(lpEsTmp);
	Dos9_EsFree(lpEsFinalPath);


	return -1;

file_found:

	snprintf(lpFullPath ,
	         iBufSize,
	         "%s",
	         Dos9_EsToChar(lpEsFinalPath)
	        );

	Dos9_EsFree(lpEsPart);
	Dos9_EsFree(lpEsTmp);
	Dos9_EsFree(lpEsFinalPath);

	return 0;
}


char* Dos9_GetPathNextPart(char* lpPath, ESTR* lpReturn)
{
	char*   lpNextToken;

	if (lpPath == NULL || *lpPath=='\0')
		return NULL;

	if (!(lpNextToken=strchr(lpPath, DOS9_PATH_DELIMITER))) {

		Dos9_EsCpy(lpReturn, lpPath);

		while (*lpPath)
			lpPath++;

		return lpPath;

	}

	Dos9_EsCpyN(lpReturn,
	            lpPath,
	            lpNextToken - lpPath);

	return ++lpNextToken;
}

int Dos9_MakePath(ESTR* lpReturn, int nOps, ...)
{
	va_list vaList;

	int     i;

	char   *lpBegin,
	       *lpEnd;

	va_start(vaList, nOps);

	*Dos9_EsToChar(lpReturn) = '\0';

	for (i=0; i<nOps; i++) {

		lpBegin=Dos9_EsToChar(lpReturn);

		lpEnd=Dos9_GetLastChar(lpReturn);

		if (*lpBegin!='\0'
		    && *lpEnd!='\\'
		    && *lpEnd!='/') {

			/* if there are no dir terminating characters and still
			   arguments, just cat a '/' */

			Dos9_EsCat(lpReturn, DOS9_DEF_DELIMITER);

		}

		Dos9_EsCat(lpReturn, va_arg(vaList,char*));

	}

	va_end(vaList);

	return 0;
}

char* Dos9_GetLastChar(ESTR* lpReturn)
{

	char* lpCh=Dos9_EsToChar(lpReturn);

	if (*lpCh=='\0')
		return lpCh;

	while (*(lpCh+1))
		lpCh++;

	return lpCh;

}
