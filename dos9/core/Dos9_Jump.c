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
#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

#include <errno.h>
#include <string.h>

int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName)
{
	size_t iSize=strlen(lpLabelName);
	char* lpName=lpFileName;
	FILE* pFile;
	ESTR* lpLine=Dos9_EsInit();


	if ((lpFileName==NULL)) {
		lpName=ifIn.lpFileName;
	}


	if (!(pFile=fopen(lpName, "r"))) {
		Dos9_EsFree(lpLine);

		DEBUG("unable to open file : %s");
		DEBUG(strerror(errno));

		return -1;
	}

    /* Libcu8 kind of perturbate the C lib because libcu8 does not
       perform byte to byte conversion, misleading file telling
       positions ... */
#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
        setvbuf(pFile, NULL, _IONBF, 0);
#endif

	while (!Dos9_EsGet(lpLine, pFile)) {

		if (!strnicmp(Dos9_SkipBlanks(lpLine->str), lpLabelName, iSize)) {

			if (lpFileName) {

				/* at that time, we can assume that lpFileName is not
				   the void string, because the void string is not usually
				   a valid file name */
				if (*lpFileName=='/'
				    || !strncmp(":/", lpFileName+1, 2)
				    || !strncmp(":\\", lpFileName+1, 2)) {

					/* the path is absolute */
					strncpy(ifIn.lpFileName, lpFileName, sizeof(ifIn.lpFileName));
					ifIn.lpFileName[FILENAME_MAX-1]='\0';

				} else {

					/* the path is relative */
					snprintf(ifIn.lpFileName,
					         sizeof(ifIn.lpFileName),
					         "%s/%s",
					         Dos9_GetCurrentDir(),
					         lpFileName
					        );

				}

			}

			ifIn.iPos=ftell(pFile);
			ifIn.bEof=feof(pFile);


			DEBUG("Freeing data");

			fclose(pFile);
			Dos9_EsFree(lpLine);

			DEBUG("Jump created with success");

			return 0;
		}
	}

	fclose(pFile);
	Dos9_EsFree(lpLine);

	DEBUG("Unable to find label");

	return -1;
}

