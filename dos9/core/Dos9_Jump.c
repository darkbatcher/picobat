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

/* Jump to a special position in file

    returns -1 in case of error
    return 0 if the jump is achieved

 */
int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName)
{
    struct labels_t* lbl;
    struct batch_script_t script;
    /* size_t size = strlen(lpLabelName); */
    int new;

    /* If neither label nor filename are given or if interactive mode */
    if ((lpLabelName == NULL && lpFileName == NULL)
        || (*(ifIn.lpFileName) == '\0' && lpFileName == NULL))
        return -1;

    if (bCmdlyCorrect)
        return Dos9_JumpToLabel_Cmdly(lpLabelName, lpFileName);

	if (lpFileName && stricmp(ifIn.batch.name, lpFileName)) {

        if (Dos9_OpenBatchScript(&script, lpFileName)) {

            Dos9_FreeBatchScript(&script);
            return -1;
        }

        lbl = script.lbls;
        new = 1;

        if (lpLabelName == NULL)
            return 0;


	} else {

        /* Update the file first */
        if (Dos9_UpdateScript(&ifIn) == -1)
            return -1;

        lbl = ifIn.batch.lbls;
        new = 0;

	}

    while (lbl && (stricmp(lbl->label, lpLabelName)))
        lbl = lbl->next;

    if (lbl == NULL) {

        Dos9_FreeBatchScript(&script);
        return -1;

    }

    /* a label has been found */
    if (new) {

        /* Set the new file as the new file */
        Dos9_FreeBatchScript(&(ifIn.batch));
        memcpy(&(ifIn.batch), &script, sizeof(struct batch_script_t));
        memcpy(ifIn.lpFileName, ifIn.batch.name, sizeof(ifIn.lpFileName));

    }

    /* Check if lbl follow a line or is the first line */
    ifIn.batch.curr = lbl->following ? lbl->following->next : ifIn.batch.cmds;
	return 0;
}

int Dos9_JumpToLabel_Cmdly(char* lpLabelName, char* lpFileName)
{
	size_t iSize=strlen(lpLabelName);
	char* lpName=lpFileName;
	FILE* pFile;
	ESTR* lpLine=Dos9_EsInit();


	if (lpFileName==NULL) {
		lpName=ifIn.lpFileName;
	}

    if (iSize == 1) {
        return -1;
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

                strncpy(ifIn.lpFileName, lpFileName, sizeof(ifIn.lpFileName));
                ifIn.lpFileName[FILENAME_MAX-1]='\0';

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
