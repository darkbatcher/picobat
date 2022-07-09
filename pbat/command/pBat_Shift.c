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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Shift.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

/* SHIFT [/n | /s[:]n] [/d[:]k]

   Shift scripts arguments.

   /n , /s[:]n	Start of the shift between arguments.

   /d[:]k		Displacement of arguments

*/

int pBat_CmdShift(char* lpLine)
{
	ESTR* lpEsArg=pBat_EsInit_Cached();
	char *lpToken;
	int iBegin=0,        /* the first parameter to be displaced */
	    iDisplacement=1, /* the displacement of parameters on the left */
	    status = PBAT_NO_ERROR;

	lpLine+=5;

	while ((lpLine=pBat_GetNextParameterEs(lpLine, lpEsArg))) {

		lpToken=pBat_EsToChar(lpEsArg);

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

                /* following switchs are pBat-specific */
				case 's':
					lpToken++;

					if (*lpToken==':')
						lpToken++;

					if (!(*lpToken>='0' && *lpToken<='9')) {

						pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

                        status = PBAT_UNEXPECTED_ELEMENT;

						goto error;

					}

					iBegin=*lpToken-'0';
					lpToken++;

					if (*lpToken) {

						pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

                        status = PBAT_UNEXPECTED_ELEMENT;

						goto error;


					}

					break;

				case 'd':
					lpToken++;

					if (*lpToken==':')
						lpToken++;

					if (!(*lpToken>='0' && *lpToken<='9')) {

						pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

                        status = PBAT_UNEXPECTED_ELEMENT;

						goto error;

					}

					iDisplacement=*lpToken-'0';
					lpToken++;

					if (*lpToken) {

						pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

                        status = PBAT_UNEXPECTED_ELEMENT;

						goto error;


					}

					break;

				case '?':
					pBat_ShowInternalHelp(PBAT_HELP_SHIFT);
					goto error;

				default:
					pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
					                      lpToken,
					                      FALSE
					                     );

                    status = PBAT_UNEXPECTED_ELEMENT;

					goto error;


			}

		} else {

			pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
			                      lpToken,
			                      FALSE);

            status = PBAT_UNEXPECTED_ELEMENT;

			goto error;

		}

	}

	/* Displace command-line arguments */
	while ((iBegin+iDisplacement) < 10) {

		pBat_SetLocalVar(lpvArguments,
		                 '0'+iBegin,
		                 pBat_GetLocalVarPointer(lpvArguments,
		                         '0'+iBegin+iDisplacement
		                                        )
		                );

		iBegin++;

	}

	/* try to get arguments from %+ */
	if (iBegin < 10) {

        /* Fast access */
        lpToken = lpvArguments[(int)'+'];

        /* if there is some remaining arguments */
        if (lpToken && *lpToken != '\0') {

            /* fill arguments with arguments remaining in "%+" */
            while (iBegin < 10
                   && (lpToken = pBat_GetNextParameterEs(lpToken, lpEsArg))) {

                pBat_SetLocalVar(lpvArguments, '0'+iBegin, lpEsArg->str);

                iBegin ++;
            }

            if (lpToken == NULL)
                pBat_SetLocalVar(lpvArguments, '+', "");
            else
                pBat_SetLocalVar(lpvArguments, '+', lpToken);
        }

	}

	/* empty the remaining arguments */
	while (iBegin < 10) {

		pBat_SetLocalVar(lpvArguments, '0'+iBegin, "");

		iBegin++;

	}

error:
	pBat_EsFree_Cached(lpEsArg);
	return status;

}
