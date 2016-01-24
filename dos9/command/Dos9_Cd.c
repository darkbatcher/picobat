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

 */

/* FIXME : Make it more compatible with cmd.exe
   In fact, cmd.exe supports different paths on differents drives
   through variables like %=x:% where x is the name of the drive.
   So that the /d switch is *really* usefull.
*/

int Dos9_CmdCd_nix(char* lpLine)
{
	char* lpNext;
	ESTR* lpEsDir=Dos9_EsInit();

	if (!strnicmp(lpLine,"CD", 2)){
		lpLine+=2;
	} else if (!strnicmp(lpLine, "CHDIR", 5)) {
		lpLine+=5;
	}

	if (*lpLine!=' '
		&& *lpLine!='\t'
		&& *lpLine!='\0'
		&& *lpLine!=';'

		&& *lpLine!=',') {
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, FALSE);
		goto error;
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

		lpLine=Dos9_EsToChar(lpEsDir);

		lpNext=NULL;

		while (*lpLine) {

			switch(*lpLine) {
				case '\t':
				case ' ':

					if (!lpNext) lpNext=lpLine;
					break;

				default:
					lpNext=NULL;
			}

			lpLine++;

		}

		if (lpNext) *lpNext='\0';

		errno=0;

		lpLine=Dos9_EsToChar(lpEsDir);

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
			goto error;

		}

	} else {

		puts(Dos9_GetCurrentDir());

	}

	Dos9_EsFree(lpEsDir);

	DOS9_DBG("Returning from \"cd\" on success\n");
	return 0;

error:

	Dos9_EsFree(lpEsDir);
	return -1;
}


int Dos9_CmdCd_win(char* lpLine)
{
    char varname[]="=x:",
		*lpNext,
		 current=*Dos9_GetCurrentDir(),
		 passed=0;

    ESTR* lpesStr=Dos9_EsInit();

    int force=0,
		status=0;

	if (!(lpLine=strchr(lpLine, ' '))) {
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, FALSE);
		status = -1;
		goto end;
	}


    if (!(lpNext = Dos9_GetNextParameterEs(lpLine, lpesStr))) {

        puts(Dos9_GetCurrentDir());

        status = 0;

        goto end;

    }

    if (!stricmp(Dos9_EsToChar(lpesStr), "/D")) {

        lpLine = lpNext;
        force = TRUE;

    }

    Dos9_GetEndOfLine(lpLine, lpesStr);

    lpLine = Dos9_SkipBlanks(Dos9_EsToChar(lpesStr));

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

                status = -1;

            }

            goto end;

        }


        /* get current curent directory disk */
        passed = *lpLine;

    }

    if ((passed == 0) || (passed == current) || (force == TRUE)) {

        /* change the current directory, yeah */

        if (Dos9_SetCurrentDir(lpLine)) {

            Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR | DOS9_PRINT_C_ERROR,
                                    lpLine,
                                    FALSE
                                    );

            status = -1;
            goto end;

        }

    }

    varname[1] = (passed == 0) ? (current) : (passed);

    Dos9_SetEnv(lpeEnv, varname, lpLine);

end:
    Dos9_EsFree(lpesStr);

    return status;
}
