/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"
#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_ShowHelp.h"

#include "Dos9_Find.h"

char* Dos9_FindRegExpMatch(const char* s, const char* exp)
{
    return (char*)Dos9_RegExpMatch(exp, s);
}

char* Dos9_FindRegExpCaseMatch(const char* s, const char* exp)
{
    return (char*)Dos9_RegExpCaseMatch(exp, s);
}

/* This function come from FreeBSD (and, BSD I think) implementation of
   strcasestr(), renamed for conflict avoiding purposes */
char* Dos9_strcasestr(const char* s, const char* find)
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
		} while (strncasecmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}

void Dos9_FileFind(char* str, char* name, int count, int flag, int reverse)
{
    FILE* pFile;
    ESTR* lpEsLine=Dos9_EsInit();

    int i,
        line;
    char* res;

    char*(*searchfn)(const char*,const char*);

    if (!name) {

        pFile = fInput;

    } else if (!(pFile = fopen(TRANS(name), "r"))) {

        Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                TRANS(name),
                                FALSE
                                );

        goto end;

    }

    switch (flag) {

    case DOS9_FIND_CASE_SENSITIVE |
        DOS9_FIND_TRADITIONAL:
        searchfn = strstr;
        break;

    case DOS9_FIND_CASE_UNSENSITIVE |
        DOS9_FIND_TRADITIONAL:
        searchfn = Dos9_strcasestr;
        break;

    case DOS9_FIND_CASE_UNSENSITIVE |
        DOS9_FIND_REGEXP:
        searchfn = Dos9_FindRegExpCaseMatch;
        break;

    default:
        searchfn = Dos9_FindRegExpMatch;

    }

    i = line = 0;

    while (!Dos9_EsGet(lpEsLine, pFile)) {

        line++;

        Dos9_RmTrailingNl(lpEsLine->str);

        res = *str ? searchfn(Dos9_EsToChar(lpEsLine), str) : 0;

        if (!reverse ^ !res) {

            if (count == 1)
                fprintf(fOutput, "[%d]%s" DOS9_NL, line, Dos9_EsToChar(lpEsLine));
            else if (count == 0)
                fprintf(fOutput, "%s" DOS9_NL, Dos9_EsToChar(lpEsLine));

            i++;
        }


    }

    if (count == 2)
        fprintf(fOutput, "%d" DOS9_NL, i);


end:
    Dos9_EsFree(lpEsLine);
    if (name)
        fclose(pFile); /* do no close stdin, please */
    else if (isatty(fileno(fInput)))
        clearerr(fInput);

    return;

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

int Dos9_CmdFind(char* lpLine)
{

    ESTR* lpEsParam=Dos9_EsInit();

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

    while ((lpLine = Dos9_GetNextParameterEs(lpLine, lpEsParam))) {


        if (!strcmp(Dos9_EsToChar(lpEsParam), "/?")) {

            Dos9_ShowInternalHelp(DOS9_HELP_FIND);

            goto end;

        } else if (!stricmp(Dos9_EsToChar(lpEsParam), "/C")){

            /* display only a count */
            count = 2;

        } else if (!stricmp(Dos9_EsToChar(lpEsParam), "/N")) {

            count = 1;

        } else if (!stricmp(Dos9_EsToChar(lpEsParam), "/I")) {

            flag |= DOS9_FIND_CASE_UNSENSITIVE;

        } else if (!stricmp(Dos9_EsToChar(lpEsParam), "/E")) {

            flag |= DOS9_FIND_REGEXP;

        } else if (!stricmp(Dos9_EsToChar(lpEsParam), "/V")) {

            reverse = 1;

        } else  {

            if (!str) {

                if (!(str=strdup(Dos9_EsToChar(lpEsParam)))) {

                    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                            | DOS9_PRINT_C_ERROR,
                                        __FILE__ "/Dos9_CmdFind()",
                                        FALSE
                                        );

                    status = -1;
                    goto end;

                 }

                 continue;

            }

            passed_names = 1;

            if (!(pTmp = Dos9_GetMatchFileList(Dos9_EsToFullPath(lpEsParam),
                                                    DOS9_SEARCH_DEFAULT))) {

                Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                       Dos9_EsToChar(lpEsParam),
                                       FALSE
                                       );

                status = -1;
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

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE,
                                "find",
                                FALSE
                                );

        status = -1;
        goto end;

    }

    /* split directories from input */
    Dos9_AttributesSplitFileList(DOS9_CMD_ATTR_DIR_N,
                                    pBegin,
                                    &pBegin,
                                    &pTmp
                                    );

    if (pTmp)
        Dos9_FreeFileList(pTmp);

    if (!pBegin && passed_names) {

        Dos9_ShowErrorMessage(DOS9_NO_VALID_FILE,
                                "FIND",
                                FALSE
                                );

        status = -1;
        goto end;

    }

    pEnd = pBegin;

    while (pEnd && pEnd->lpflNext)
        pEnd = pEnd->lpflNext;

    /* find in files */

    if (!pBegin) {

        Dos9_FileFind(str, NULL, count, flag, reverse);

    } else if (pBegin == pEnd) {

        Dos9_FileFind(str, pBegin->lpFileName, count, flag, reverse);

    } else {

        pTmp = pBegin;

        while (pTmp) {

            fprintf(fOutput, "---------- %s" DOS9_NL, pTmp->lpFileName);
            Dos9_FileFind(str, pTmp->lpFileName, count, flag, reverse);

            pTmp = pTmp->lpflNext;

        }

    }


end:

    if (str)
        free(str);

    if (pBegin)
        Dos9_FreeFileList(pBegin);

    Dos9_EsFree(lpEsParam);

    return status;

 }
