/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2017 Romain GARBI
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
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include <libpBat.h>

#include "pBat_Ask.h"
#include "../core/pBat_Core.h"
#include "../lang/pBat_Lang.h"
#include "../../config.h"

#if defined(WIN32) && defined(PBAT_USE_LIBCU8)

/* A function to override the hook that libcu8 adds on read()
   that automatically enable autocompletion, which can get
   quite problematic when asking question to the user */
void xread(int fd, void* p, size_t sz)
{
    size_t cnt;

    do {

        ReadFile(_get_osfhandle(fd), p, sz, &cnt, NULL);

    } while (!cnt);

}
#else
#define xread(fd, p, n) read(fd, p, n)
#endif // defined

void pBat_AskConfirmationRead(char* buffer, size_t size)
{

    int i = 0;
    char c;

    while (1) {

        xread(fileno(fInput), &c, 1);

        if (c == '\r')
            continue;

        if (c == '\n')
            break;

        if (i < size)
            buffer[i] = c;

        i ++;
    }


    if (i < size)
        buffer[i] = '\0';
    else
        buffer[size - 1] = '\0';
}

int pBat_AskConfirmation(int iFlags, void(*lpFn)(char*,size_t),  const char* lpMsg, ...)
{
	va_list vaArgs;
	const char *lpChoices=NULL;
	/* char *lpLf; */

	int iRet;
	char lpInput[30];

	if (iFlags & PBAT_ASK_YNA) {

		/* if the user choosed to display YES NO ALL
		   options */

		switch(iFlags & (PBAT_ASK_DEFAULT_Y | PBAT_ASK_DEFAULT_N |
		                 PBAT_ASK_DEFAULT_A)) {


			case PBAT_ASK_DEFAULT_Y:
				lpChoices=lpAskYna;
				break;

			case PBAT_ASK_DEFAULT_N:
				lpChoices=lpAskyNa;
				break;

			case PBAT_ASK_DEFAULT_A:
				lpChoices=lpAskynA;
				break;

			default:
				lpChoices=lpAskyna;
				break;


		}

	} else {

		/* if the user chose to display only YES and NO */

		if (iFlags & PBAT_ASK_DEFAULT_A)
			return PBAT_ASK_INVALID;

		switch (iFlags & (PBAT_ASK_DEFAULT_Y | PBAT_ASK_DEFAULT_N)) {

			case PBAT_ASK_DEFAULT_Y:
				lpChoices=lpAskYn;
				break;

			case PBAT_ASK_DEFAULT_N:
				lpChoices=lpAskyN;
				break;

			default:
				lpChoices=lpAskyn;

		}

	}

	va_start(vaArgs, lpMsg);
	vfprintf(fError, lpMsg, vaArgs);
	va_end(vaArgs);

	do {

		fputs(lpChoices, fError);

        /* if no read function provided, fallback to default */
        if (lpFn == NULL)
            lpFn = pBat_AskConfirmationRead;

        lpFn(lpInput, sizeof(lpInput));

		if (!stricmp(lpInput, lpAskYes)
		    || !stricmp(lpInput, lpAskYesA)) {

			iRet=PBAT_ASK_YES;

		} else if (!stricmp(lpInput, lpAskNo)
		           || !stricmp(lpInput, lpAskNoA)) {

			iRet=PBAT_ASK_NO;

		} else {

			if ((iFlags & PBAT_ASK_YNA) && (
			        !stricmp(lpInput, lpAskAll)
			        || !stricmp(lpInput, lpAskAllA))) {


				iRet=PBAT_ASK_ALL;

			} else if ((iFlags & (PBAT_ASK_DEFAULT_Y | PBAT_ASK_DEFAULT_N
			                      | PBAT_ASK_DEFAULT_A))
			           && *lpInput=='\0') {

				iRet=iFlags & (PBAT_ASK_DEFAULT_Y | PBAT_ASK_DEFAULT_N
				               | PBAT_ASK_DEFAULT_A);

			} else if (!(iFlags & PBAT_ASK_INVALID_REASK)) {

				iRet=PBAT_ASK_INVALID;

			} else {

				fputs(lpAskInvalid, stderr);

			}

		}

	} while (iRet==0);



	return iRet;
}
