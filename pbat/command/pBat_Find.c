/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2014 Romain Garbi
 *   Copyright (c) 1990, 1993
 *	 The Regents of the University of California.  All rights reserved.
 *
 *   This code is derived from software contributed to Berkeley by
 *   Chris Torek.
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
#include <ctype.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"
#include "../errors/pBat_Errors.h"
#include "../lang/pBat_ShowHelp.h"

#include "pBat_Find.h"

char* pBat_FindRegExpMatch(const char* restrict s, const char* restrict exp)
{
    return (char*)pBat_RegExpMatch(exp, s);
}

char* pBat_FindRegExpCaseMatch(const char* restrict s, const char* restrict exp)
{
    return (char*)pBat_RegExpCaseMatch(exp, s);
}

/* This function come from FreeBSD (and, BSD I think) implementation of
   strcasestr(), renamed for conflict avoiding purposes */
char* pBat_strcasestr(const char* restrict s, const char* restrict find)
{
	char c, sc;
	size_t len;

	if ((c = *find++) != 0) {
		c = tolower((unsigned char)c);
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while ((char)tolower((unsigned char)sc) != c);
		} while (strnicmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}

int pBat_FileFind(char* restrict str, char* restrict name, int count, int flag, int reverse)
{
    FILE* pFile;
    ESTR* lpEsLine=pBat_EsInit();

    int i,
        line;
    char* res;

    char*(*searchfn)(const char*,const char*);

    if (!name) {

        pFile = fInput;

    } else if (!(pFile = fopen(name, "r"))) {

        /* This should not be reached anyway, so don't care about return */
        pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                name,
                                FALSE
                                );
        i = 0;

        goto end;

    }

    switch (flag) {

    case PBAT_FIND_CASE_SENSITIVE |
        PBAT_FIND_TRADITIONAL:
        searchfn = strstr;
        break;

    case PBAT_FIND_CASE_UNSENSITIVE |
        PBAT_FIND_TRADITIONAL:
        searchfn = pBat_strcasestr;
        break;

    case PBAT_FIND_CASE_UNSENSITIVE |
        PBAT_FIND_REGEXP:
        searchfn = pBat_FindRegExpCaseMatch;
        break;

    default:
        searchfn = pBat_FindRegExpMatch;

    }

    i = line = 0;

    while (!pBat_EsGet(lpEsLine, pFile)) {

        line++;

        pBat_RmTrailingNl(lpEsLine->str);

        res = *str ? searchfn(pBat_EsToChar(lpEsLine), str) : 0;

        if (!reverse ^ !res) {

            if (count == 1)
                fprintf(fOutput, "[%d]%s" PBAT_NL, line, pBat_EsToChar(lpEsLine));
            else if (count == 0)
                fprintf(fOutput, "%s" PBAT_NL, pBat_EsToChar(lpEsLine));

            i++;
        }


    }

    if (count == 2)
        fprintf(fOutput, "%d" PBAT_NL, i);


end:
    pBat_EsFree(lpEsLine);
    if (name)
        fclose(pFile); /* do no close stdin, please */
    else if (isatty(fileno(fInput)))
        clearerr(fInput);

    return !i;

}

/* Find an occurence of a string in a set of files

    FIND [/N] [/C] [/V] [/I] string [files ...]


   - /N         : Print matching lines number
   - /C         : Only display a count of matching lines
   - /V         : Reversed search
   - /I         : Ignore case
   - string     : String to be found in the file
   - files ...  : A list of files in which string should be looked for.
   If no files are specified, take input from the standard input.

*/

int pBat_CmdFind(char* lpLine)
{

    ESTR* lpEsParam=pBat_EsInit();

	FILELIST *pBegin=NULL,
             *pTmp,
             *pEnd;

    int status=0,
        count=0,
        flag=0,
        reverse=0,
        passed_names=0;

    /* char buf[8192]; */
    char* str=NULL;

    lpLine += 4;

    while ((lpLine = pBat_GetNextParameterEs(lpLine, lpEsParam))) {


        if (!strcmp(pBat_EsToChar(lpEsParam), "/?")) {

            pBat_ShowInternalHelp(PBAT_HELP_FIND);

            goto end;

        } else if (!stricmp(pBat_EsToChar(lpEsParam), "/C")){

            /* display only a count */
            count = 2;

        } else if (!stricmp(pBat_EsToChar(lpEsParam), "/N")) {

            count = 1;

        } else if (!stricmp(pBat_EsToChar(lpEsParam), "/I")) {

            flag |= PBAT_FIND_CASE_UNSENSITIVE;

        } else if (!stricmp(pBat_EsToChar(lpEsParam), "/E")) {

            flag |= PBAT_FIND_REGEXP;

        } else if (!stricmp(pBat_EsToChar(lpEsParam), "/V")) {

            reverse = 1;

        } else  {

            if (!str) {

                if (!(str=strdup(pBat_EsToChar(lpEsParam)))) {

                    pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION
                                            | PBAT_PRINT_C_ERROR,
                                        __FILE__ "/pBat_CmdFind()",
                                        FALSE
                                        );

                    status = PBAT_FAILED_ALLOCATION;
                    goto end;

                 }

                 continue;

            }

            passed_names = 1;

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
                pBegin = pEnd = pTmp;
            else
                pEnd->lpflNext = pTmp; /* catenate the lists */

            while (pEnd->lpflNext != NULL)
                pEnd=pEnd->lpflNext;

        }

    }

    if (!str) {

        pBat_ShowErrorMessage(PBAT_EXPECTED_MORE,
                                "find",
                                FALSE
                                );

        status = PBAT_EXPECTED_MORE;
        goto end;

    }

    /* split directories from input */
    pBat_AttributesSplitFileList(PBAT_CMD_ATTR_DIR_N,
                                    pBegin,
                                    &pBegin,
                                    &pTmp
                                    );

    if (pTmp)
        pBat_FreeFileList(pTmp);

    if (!pBegin && passed_names) {

        pBat_ShowErrorMessage(PBAT_NO_VALID_FILE,
                                "FIND",
                                FALSE
                                );

        status = PBAT_NO_VALID_FILE;
        goto end;

    }

    pEnd = pBegin;

    while (pEnd && pEnd->lpflNext)
        pEnd = pEnd->lpflNext;

    /* find in files */

    if (!pBegin) {

        status = pBat_FileFind(str, NULL, count, flag, reverse);

    } else if (pBegin == pEnd) {

        status = pBat_FileFind(str, pBegin->lpFileName, count, flag, reverse);

    } else {

        pTmp = pBegin;

        while (pTmp) {

            fprintf(fOutput, "---------- %s" PBAT_NL, pTmp->lpFileName);
            status |= pBat_FileFind(str, pTmp->lpFileName, count, flag, reverse);

            pTmp = pTmp->lpflNext;

        }

    }


end:

    if (str)
        free(str);

    if (pBegin)
        pBat_FreeFileList(pBegin);

    pBat_EsFree(lpEsParam);

    return status;

 }
