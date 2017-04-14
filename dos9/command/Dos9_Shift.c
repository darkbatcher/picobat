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

#include "Dos9_Shift.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* SHIFT [/n | /s[:]n] [/d[:]k]

   Shift scripts arguments.

   /n , /s[:]n	Start of the shift between arguments.

   /d[:]k		Displacement of arguments

*/

int Dos9_CmdShift(char* lpLine)
{
	ESTR* lpEsArg=Dos9_EsInit();
	char *lpToken;
	int iBegin=0,        /* the first parameter to be displaced */
	    iDisplacement=1; /* the displacement of parameters on the left */

	lpLine+=5;

	while (lpLine=Dos9_GetNextParameterEs(lpLine, lpEsArg)) {

		lpToken=Dos9_EsToChar(lpEsArg);

		if ((*lpToken)=='/') {

			lpToken++;

			switch (*lpToken) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '8':
				case '9':
					iBegin=*lpToken-'0'; /* well, we assume that all numbers
                                            are folowing 0 */

					break;

					/* following switchs are Dos9-specific */
				case 's':
					lpToken++;

					if (*lpToken==':')
						lpToken++;

					if (!(*lpToken>='0' && *lpToken<='9')) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;

					}

					iBegin=*lpToken-'0';
					lpToken++;

					if (*lpToken) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;


					}

					break;

				case 'd':
					lpToken++;

					if (*lpToken==':')
						lpToken++;

					if (!(*lpToken>='0' && *lpToken<='9')) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;

					}

					iDisplacement=*lpToken-'0';
					lpToken++;

					if (*lpToken) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;


					}

					break;

				case '?':
					Dos9_ShowInternalHelp(DOS9_HELP_SHIFT);
					goto error;

				default:
					Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
					                      lpToken,
					                      FALSE
					                     );

					goto error;


			}

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
			                      lpToken,
			                      FALSE);

			goto error;

		}

	}

	/* Displace command-line arguments */
	while ((iBegin+iDisplacement) < 10) {

		Dos9_SetLocalVar(lpvArguments,
		                 '0'+iBegin,
		                 Dos9_GetLocalVarPointer(lpvArguments,
		                         '0'+iBegin+iDisplacement
		                                        )
		                );

		iBegin++;

	}

	/* empty the remaining arguments */
	while (iBegin < 10) {

		Dos9_SetLocalVar(lpvArguments, '0'+iBegin, "");

		iBegin++;

	}



	Dos9_EsFree(lpEsArg);

	return 0;

error:

	Dos9_EsFree(lpEsArg);
	return -1;

}

