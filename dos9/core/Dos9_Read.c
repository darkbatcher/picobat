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
#include <string.h>

#include <libDos9.h>

#include "Dos9_Core.h"

#include "Dos9_Debug.h"

int Dos9_GetLine(ESTR* lpesLine, INPUT_FILE* pIn)
{

    /* If either cmdly correct is defined or if we are running interactive
       mode, use the cmdly correct function */
    if (bCmdlyCorrect || (*(pIn->lpFileName) == '\0'))
        return Dos9_GetLine_Cmdly(lpesLine, pIn);

    if (*(pIn->batch.name) == '\0') {

        /* Load the file if it is not loaded yet */
        if (Dos9_OpenBatchScript(&(pIn->batch), pIn->lpFileName)) {

            Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                  pIn->lpFileName,
                                  FALSE);
            Dos9_FreeBatchScript(&(pIn->batch));
            goto error;

        }

    } else if (strcmp(pIn->lpFileName, pIn->batch.name)) {

        /* The file has changed */
        Dos9_FreeBatchScript(&(pIn->batch));

        if (Dos9_OpenBatchScript(&(pIn->batch), pIn->lpFileName)) {

            Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                  pIn->lpFileName,
                                  FALSE);
            Dos9_FreeBatchScript(&(pIn->batch));
            goto error;

        }

    } else if (Dos9_CheckBatchScriptChanges(&(pIn->batch))) {

        /* Reload it if it has been modified */
        if (Dos9_ReloadBatchScript(&(pIn->batch))) {

            Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                  pIn->lpFileName,
                                  FALSE);
            Dos9_FreeBatchScript(&(pIn->batch));
            goto error;

        }

    }

    /* We have an appropriate batch script structure */

    if (pIn->batch.curr == NULL) {

        /* Looks like we have reached eof */
        pIn->bEof = TRUE;
        return 1; /* signal empty line */

    }

    /* Get the next line */
    Dos9_EsCpy(lpesLine, pIn->batch.curr->line);

    pIn->batch.curr = pIn->batch.curr-> next;

    return 0;

error:
    pIn->bEof = TRUE;
    return -1;
}

int Dos9_GetLine_Cmdly(ESTR* lpesLine, INPUT_FILE* pIn)
{
	FILE* pFile;
	ESTR* lpesTmp=Dos9_EsInit();

	char bCorrectBlocks=FALSE;
	char* lpCh;
	int   res;

	if (*(pIn->lpFileName) == '\0') {

		pFile=stdin;

	} else if (!(pFile=fopen(pIn->lpFileName, "r"))) {

		Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
		                      pIn->lpFileName,
		                      FALSE);
		goto error;

	} else {

    /* Libcu8 kind of perturbates the C lib because libcu8 does not
       perform byte to byte conversion, misleading file telling
       positions ... Thus discard C buffering from libcu8*/
#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
        setvbuf(pFile, NULL, _IONBF, 0);
#endif
        fseek(pFile, pIn->iPos, SEEK_SET);
	}

	*(Dos9_EsToChar(lpesLine))='\0';

	while (!(res=Dos9_EsGet(lpesTmp, pFile))) {

		lpCh=Dos9_SkipAllBlanks(Dos9_EsToChar(lpesTmp));

		/* split comments label and void lines from input */
		if (*lpCh==':' || *lpCh=='\0') {

            if (pFile != stdin)
                continue;

            bCorrectBlocks=TRUE;
            break;
		}

        /* the read line should *definitely* end with a '\n' (in
           in fact it always do, except when reaching end of file) */
        if (strchr(lpesTmp->str, '\n') == NULL)
            Dos9_EsCat(lpesTmp, "\n");

        Dos9_EsCatE(lpesLine, lpesTmp);

		if (Dos9_CheckBlocks(lpesLine) == TRUE) {

			bCorrectBlocks=TRUE;
			break;

		}
	}

	DOS9_DBG("-------------------[READ]--------------------------\n"
	         "%s\n"
	         "---------------------------------------------------\n",
	         Dos9_EsToChar(lpesLine)
	        );

	//getch();

    pIn->bEof=res;

	if (*(pIn->lpFileName)!='\0') {

		if (!res)
			pIn->iPos=ftell(pFile);

		fclose(pFile);

	}

	if (!*Dos9_EsToChar(lpesLine)) {

		Dos9_EsFree(lpesTmp);
		return 1;

	}

	if (!bCorrectBlocks) {

		Dos9_ShowErrorMessage(DOS9_NONCLOSED_BLOCKS, NULL, FALSE);
		goto error;

	}

	Dos9_EsFree(lpesTmp);

	Dos9_RmTrailingNl(lpesLine->str);

	return 0;

error:
	pIn->bEof=TRUE;
	Dos9_EsFree(lpesTmp);
	return -1;

}

/* This does some subtle stuff to determine whether :

    TRUE -> the block is complete
    FALSE -> the block is uncomplete and and a block is
             is left unclosed.
    -1 -> the block line is nearly complete, but it lacks
          a '\n' to be complete (we are at top level at
          the end of lpesLine).
 */
int Dos9_CheckBlocks(ESTR* lpesLine)
{
    char *pch = lpesLine->str;

    do {

        if ((*pch == '|') || (*pch == '&'))
            pch ++;

        if ((pch = Dos9_GetBlockLineEndEx(pch, 0)) == NULL)
            return FALSE;

    } while ((*pch != '\n') && (*pch != '\0'));

    if (*pch)
        return TRUE;

    return -1;
}

void Dos9_RmTrailingNl(char* lpLine)
{
    char* line;
	if (
#ifdef WIN32
        ((line = strrchr(lpLine, '\r'))
           && *(line+1) == '\n'
           && *(line+2) == '\0'
         ) ||
#endif
        ((line = strrchr(lpLine, '\n'))
           && *(line+1) == '\0'
         )
       )
         *line = '\0';
}
