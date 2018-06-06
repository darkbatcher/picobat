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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Cd.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* Changes the system's current directory

    CD [/d] [[drive:]path]                    (Windows)
    CD [path]                                 (Unixes)

    Changes the system current directory.

        - [path] : The path for the new current directory.

    ** Windows specific **

        - [/d] : If the combination drive:path is located in another
        directory, force drive change. If drive:path is located in another
        directory, but /d is not specified, then it changes the current
        directory on the drive, without changing the current directory on
        that drive.

    Current directory on drive are stored in environment variables (undocumented,
    but cmd-compatible), that are named following the rule : %drive=:%.

    Similar effects may be achieved using the popd and pushd in unixes commands.

    ** End Windows specific **

    NOTE a little inconsistency with cmd.exe. %CD% *never* ends with a '\' on
    windows ! This allows doing proper treatment when being a disk root ...
 */



int /*__inline__ */ Dos9_Canonicalize(char* path)
{
    char *previous = NULL, *next, *orig = path;
    size_t size = strlen(path) + 1;


    /* first pass to clean multiple separators and "." characters */
    while (*path) {

        if (TEST_SEPARATOR(path))
            *path = DEF_SEPARATOR;

        if (TEST_SEPARATOR(path)) {

            /* Try to swallow multiple delimiters */
            next = path + 1;

reloop:
            while (TEST_SEPARATOR(next))
                next ++;

            /* check this not a dull "." folder */
            if (*next == '.' && TEST_SEPARATOR((next + 1))) {
                next ++;
                goto reloop;
            } else if (*next == '.' && *(next + 1) == '\0')
                next ++;

            if (next != path + 1) {
                memmove(path + 1, next, size - (next - orig));
                size -= next - (path + 1); /* remove bytes that were swallowed */
            }

            if (*(path + 1) == '.' && *(path + 2) == '.'
                && (TEST_SEPARATOR((path + 3)) || *(path + 3) == '\0')) {

                /* Apparently, this folder is "..", find the previous dir & swallow both */
                previous = path - 1;
                while (previous >= orig && !TEST_SEPARATOR(previous))
                    previous --;

                if (previous < orig) {

                    /* this is not meant to happen unless if .. follows directly the root
                       remove it then */
#ifdef WIN32
                    *path = '\0';
#else
                    *(path + 1) = '\0';
#endif // WIN32

                } else {

                    /* squeeze the preceeding dir and the separator */
                    memmove(previous, path+3, size - ((path + 3 ) - orig));
                    size -= (path + 3) - previous;
                    path = previous;
                    continue;

                }

            }

            if (*path && *(path + 1) == '\0') {
                *path = '\0';
                break;
            }

        }

        if (*path)
            path ++;
    }

    /* Wait ... */

    return 0;
}

int Dos9_SetCurrentDir(char* lpLine)
{
    char *p/*, *p2 = NULL */;
    size_t remain;

    if (TEST_ABSOLUTE_PATH(lpLine)
        && Dos9_DirExists(lpLine)) {

 #ifdef WIN32
        /* Under windows, we do not have unique file system
           root, so that '/' refers to the current active drive, we have
           to deal with it */

        if (*lpLine == '/') {

            /* well, arguably lpCurrentDir[2] is a slash ...
               unless lpCurrentDir refers to a unc path, but this
               is not handled yet */
            strncpy(lpCurrentDir + 2, lpLine, FILENAME_MAX-2);
            lpCurrentDir[FILENAME_MAX-1];

            return Dos9_Canonicalize(lpCurrentDir);
        }
 #endif

        strncpy(lpCurrentDir, lpLine, FILENAME_MAX);
        lpCurrentDir[FILENAME_MAX - 1];
        return Dos9_Canonicalize(lpCurrentDir);

    }

    /* find the end of the current dir */
    p = lpCurrentDir;
    while (*p)
        p ++;

#ifdef WIN32
    *p = '\\';
#else
    *p = '/';
#endif

    remain = FILENAME_MAX - (p + 1 - lpCurrentDir);

    if (remain > 0) {

        strncpy(p + 1, lpLine, remain);
        lpCurrentDir[FILENAME_MAX - 1] = '\0';

        if (Dos9_DirExists(lpCurrentDir))
            return Dos9_Canonicalize(lpCurrentDir);

    }

    *p = '\0';

    errno = ENOENT;
    return -1;
}

int Dos9_CmdCd_nix(char* lpLine)
{
	char* lpNext;
	ESTR* lpEsDir=Dos9_EsInit();
	int status = DOS9_NO_ERROR,
	    quotes = 0;

	if (!strnicmp(lpLine,"CD", 2)){
		lpLine+=2;
	} else if (!strnicmp(lpLine, "CHDIR", 5)) {
		lpLine+=5;
	}

	if ((lpNext=Dos9_GetNextParameterEs(lpLine, lpEsDir))) {

		if (!strcmp(Dos9_EsToChar(lpEsDir), "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_CD);
			goto error;

		} else if (!stricmp(Dos9_EsToChar(lpEsDir), "/d")) {

			lpLine=lpNext;

		}

		while (*lpLine==' ' || *lpLine=='\t') lpLine++;

		Dos9_GetEndOfLine(lpLine, lpEsDir);

		lpLine=Dos9_SkipBlanks(Dos9_EsToChar(lpEsDir));

        if (*lpLine == '"') {

            quotes = 1;
            lpLine ++;

        }

        while (*lpLine) {

            switch(*lpLine) {
                case '"':
                    if (!quotes)
                        goto def;
                case '\t':
                case ' ':

                    if (!lpNext) lpNext=lpLine;
                    break;
def:
                default:
                    lpNext=NULL;
            }

            lpLine++;

        }

        if (lpNext)
            *lpNext = '\0';


		errno=0;

		lpLine=Dos9_SkipBlanks(Dos9_EsToChar(lpEsDir));

		if (quotes)
            lpLine ++;

		DOS9_DBG("Changing directory to : \"%s\"\n", lpLine);

		if (Dos9_SetCurrentDir(lpLine)) {


			/* do not perform errno checking
			   as long as the most important reason for
			   chdir to fail is obviously the non existence
			   or the specified directory

			   However, it appears that this is inconsistant
			   using windows as it does not returns on failure
			   every time a non-existing folder is passed to the
			   function, tried with '.. ' on my system

			*/

			Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, lpLine, FALSE);

            status = DOS9_DIRECTORY_ERROR;
			goto error;

		}

	} else {

		fputs(lpCurrentDir, fOutput);

	}

error:
	Dos9_EsFree(lpEsDir);
	return status;
}


int Dos9_CmdCd_win(char* lpLine)
{
    char varname[]="=x:",
		*lpNext,
		 current=*lpCurrentDir,
		 passed=0;
    int quotes = 0;

    ESTR* lpesStr=Dos9_EsInit();

    int force=0,
		status=0;

    if (!strnicmp("cd", lpLine, 2))
        lpLine +=2;
    else if (!strnicmp("chdir", lpLine, 5))
        lpLine +=5;


    if (!(lpNext = Dos9_GetNextParameterEs(lpLine, lpesStr))) {

        fputs(lpCurrentDir, fOutput);
        fputs(DOS9_NL, fOutput);

        status = 0;

        goto end;

    }

    if (!stricmp(Dos9_EsToChar(lpesStr), "/D")) {

        lpLine = lpNext;
        force = TRUE;

    }

    Dos9_GetEndOfLine(lpLine, lpesStr);

    lpLine = Dos9_SkipBlanks(lpesStr->str);
    lpNext = NULL;

    if (*lpLine == '"') {

        quotes = 1;
        lpLine ++;

    }

    while (*lpLine) {

        switch(*lpLine) {
            case '"':
                if (!quotes)
                    goto def;
            case '\t':
            case ' ':

                if (!lpNext) lpNext=lpLine;
                break;
def:
            default:
                lpNext=NULL;
        }

        lpLine++;

    }

    if (lpNext)
        *lpNext = '\0';

    lpLine = Dos9_SkipBlanks(Dos9_EsToChar(lpesStr));

    if (quotes)
        lpLine ++;

    if (*lpLine && *(lpLine+1)==':') {

        if (*Dos9_SkipBlanks(lpLine+2) == '\0') {

            /* only got a drive name */
            varname[1] = *lpLine;

            if (!(lpNext = Dos9_GetEnv(lpeEnv, varname))) {

                lpNext = lpLine;

            }

            if (Dos9_SetCurrentDir(lpNext)) {

                Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR
                                        | DOS9_PRINT_C_ERROR,
                                      lpNext,
                                      FALSE
                                      );

                status = DOS9_DIRECTORY_ERROR;

            }

            goto end;

        }


        /* get current curent directory disk */
        passed = *lpLine;

    }

    if ((passed == 0)
        || (toupper(passed) == toupper(current))
        || (force == TRUE)) {

        /* change the current directory, yeah */

        if (Dos9_SetCurrentDir(lpLine)) {

            Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR | DOS9_PRINT_C_ERROR,
                                    lpLine,
                                    FALSE
                                    );

            status = DOS9_DIRECTORY_ERROR;
            goto end;

        }

    }

    varname[1] = (passed == 0) ? (current) : (passed);

    Dos9_SetEnv(lpeEnv, varname, lpLine);

end:
    Dos9_EsFree(lpesStr);

    return status;
}
