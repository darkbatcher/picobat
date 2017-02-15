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
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include <libDos9.h>

#include "Dos9_Ask.h"
#include "../lang/Dos9_Lang.h"

int Dos9_AskConfirmation(int iFlags, const char* lpMsg, ...)
{
	va_list vaArgs;
	const char *lpChoices=NULL;
	char *lpLf;

	int iRet;

	ESTR* lpInput=Dos9_EsInit();

	if (iFlags & DOS9_ASK_YNA) {

		/* if the user choosed to display YES NO ALL
		   options */

		switch(iFlags & (DOS9_ASK_DEFAULT_Y | DOS9_ASK_DEFAULT_N |
		                 DOS9_ASK_DEFAULT_A)) {


			case DOS9_ASK_DEFAULT_Y:
				lpChoices=lpAskYna;
				break;

			case DOS9_ASK_DEFAULT_N:
				lpChoices=lpAskyNa;
				break;

			case DOS9_ASK_DEFAULT_A:
				lpChoices=lpAskynA;
				break;

			default:
				lpChoices=lpAskyna;
				break;


		}

	} else {

		/* if the user choosed to display only YES and NO */

		if (iFlags & DOS9_ASK_DEFAULT_A) {

			Dos9_EsFree(lpInput);
			return DOS9_ASK_INVALID;

		}

		switch (iFlags & (DOS9_ASK_DEFAULT_Y || DOS9_ASK_DEFAULT_N)) {

			case DOS9_ASK_DEFAULT_Y:
				lpChoices=lpAskYn;
				break;

			case DOS9_ASK_DEFAULT_N:
				lpChoices=lpAskyN;
				break;

			default:
				lpChoices=lpAskyn;

		}

	}

	va_start(vaArgs, lpMsg);

	do {

		vfprintf(stderr, lpMsg, vaArgs);

		fputs(lpChoices, stderr);

		Dos9_EsGet(lpInput, stdin);

		if ((lpLf=strchr(Dos9_EsToChar(lpInput), '\n')))
			*lpLf='\0';

		if (!stricmp(Dos9_EsToChar(lpInput), lpAskYes)
		    || !stricmp(Dos9_EsToChar(lpInput), lpAskYesA)) {

			iRet=DOS9_ASK_YES;

		} else if (!stricmp(Dos9_EsToChar(lpInput), lpAskNo)
		           || !stricmp(Dos9_EsToChar(lpInput), lpAskNoA)) {

			iRet=DOS9_ASK_NO;

		} else {

			if ((iFlags & DOS9_ASK_YNA) && (
			        !stricmp(Dos9_EsToChar(lpInput), lpAskAll)
			        || !stricmp(Dos9_EsToChar(lpInput), lpAskAllA))) {


				iRet=DOS9_ASK_ALL;

			} else if ((iFlags & (DOS9_ASK_DEFAULT_Y | DOS9_ASK_DEFAULT_N
			                      | DOS9_ASK_DEFAULT_A))
			           && *Dos9_EsToChar(lpInput)=='\0') {

				iRet=iFlags & (DOS9_ASK_DEFAULT_Y | DOS9_ASK_DEFAULT_N
				               | DOS9_ASK_DEFAULT_A);

			} else if (!(iFlags & DOS9_ASK_INVALID_REASK)) {

				iRet=DOS9_ASK_INVALID;

			} else {

				fputs(lpAskInvalid, stderr);

			}

		}

	} while (iRet==0);

	va_end(vaArgs);

	Dos9_EsFree(lpInput);
	return iRet;
}
