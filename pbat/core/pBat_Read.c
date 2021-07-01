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
#include <string.h>

#include <libpBat.h>

#include "pBat_Core.h"

#include "pBat_Debug.h"

#if !defined(WIN32) && !defined(PBAT_NO_LINENOISE)
#include "../linenoise/pBat_LineNoise.h"
#endif

int pBat_UpdateScript(INPUT_FILE* pIn)
{
    if (*(pIn->lpFileName) == '\0')
        return 0;

    if (*(pIn->batch.name) == '\0') {

        /* Load the file if it is not loaded yet */
        if (pBat_OpenBatchScript(&(pIn->batch), pIn->lpFileName)) {

            pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                  pIn->lpFileName,
                                  FALSE);
            pBat_FreeBatchScript(&(pIn->batch));
            goto error;

        }

    } else if (strcmp(pIn->lpFileName, pIn->batch.name)) {

        /* The file has changed */
        pBat_FreeBatchScript(&(pIn->batch));

        if (pBat_OpenBatchScript(&(pIn->batch), pIn->lpFileName)) {

            pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                  pIn->lpFileName,
                                  FALSE);
            pBat_FreeBatchScript(&(pIn->batch));
            goto error;

        }

    } else if (pBat_CheckBatchScriptChanges(&(pIn->batch))) {

        /* Reload it if it has been modified */
        if (pBat_ReloadBatchScript(&(pIn->batch))) {

            pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
                                  pIn->lpFileName,
                                  FALSE);
            pBat_FreeBatchScript(&(pIn->batch));
            goto error;

        }

    }

    return 0;

error:
    return -1;

}

int pBat_GetLine(ESTR* lpesLine, INPUT_FILE* pIn)
{
    int  ok;
    struct cmdlines_t* cmdlines;
    ESTR* tmp;

    /* If either cmdly correct is defined or if we are running interactive
       mode, use the cmdly correct function */
    if (bCmdlyCorrect || (*(pIn->lpFileName) == '\0'))
        return pBat_GetLine_Cmdly(lpesLine, pIn);


    if (pBat_UpdateScript(pIn) == -1)
        goto error;

    /* We have an appropriate batch script structure */

    if (pIn->batch.curr == NULL) {

        /* Looks like we have reached eof */
        pIn->bEof = TRUE;
        return 1; /* signal empty line */

    }

    pBat_EsCpy(lpesLine, "");
    cmdlines = pIn->batch.curr;
    tmp = pBat_EsInit();

    /* Get the next line */
    do {

        pBat_EsCpy(tmp, cmdlines->line);
        pBat_ReplaceVars(tmp);

        pBat_EsCat(lpesLine, tmp->str);

        cmdlines = cmdlines->next;

    } while ((!(ok = pBat_CheckBlocks(lpesLine)) || ok == -1) && cmdlines);

    pBat_EsFree(tmp);
    pIn->batch.curr = cmdlines;
    pBat_RmTrailingNl(lpesLine->str);

    if (ok == -1 || !ok) {

		pBat_ShowErrorMessage(PBAT_NONCLOSED_BLOCKS, NULL, FALSE);
		goto error;

    }

    return 0;

error:
    pIn->bEof = TRUE;
    return -1;
}

int pBat_GetLine_Cmdly(ESTR* lpesLine, INPUT_FILE* pIn)
{
	FILE* pFile;
	ESTR* lpesTmp=pBat_EsInit();

	char bCorrectBlocks=FALSE;
	char* lpCh;
	int   res;

	int(*fn_getline)(ESTR*,FILE*) = pBat_EsGet;

	if (*(pIn->lpFileName) == '\0') {

		pFile=fInput;

#if !defined(WIN32) && !defined(PBAT_NO_LINENOISE)
        /* If fInput is a tty, use Linenoise ! */
        if (isatty(fileno(pFile)))
            fn_getline = pBat_LineNoise;
#endif

	} else if (!(pFile=fopen(pIn->lpFileName, "r"))) {

		pBat_ShowErrorMessage(PBAT_FILE_ERROR | PBAT_PRINT_C_ERROR,
		                      pIn->lpFileName,
		                      FALSE);
		goto error;

	} else {

    /* Libcu8 kind of perturbates the C lib because libcu8 does not
       perform byte to byte conversion, misleading file telling
       positions ... Thus discard C buffering from libcu8*/
#if defined(WIN32) && defined(PBAT_USE_LIBCU8)
        setvbuf(pFile, NULL, _IONBF, 0);
#endif
        fseek(pFile, pIn->iPos, SEEK_SET);
	}

	*(pBat_EsToChar(lpesLine))='\0';

	while (!(res=fn_getline(lpesTmp, pFile))) {

		lpCh=pBat_SkipAllBlanks(pBat_EsToChar(lpesTmp));

        pBat_ReplaceVars(lpesTmp);

		/* split comments label and void lines from input */
		if (*lpCh==':' || *lpCh=='\0' || *lpCh=='\n') {

            if (*(lpesLine->str) == '\0') {

                bCorrectBlocks=TRUE;
                break;

            }

            continue;
		}

        /* the read line should *definitely* end with a '\n' (in
           in fact it always do, except when reaching end of file) */
        if (strchr(lpesTmp->str, '\n') == NULL)
            pBat_EsCat(lpesTmp, "\n");

        pBat_EsCatE(lpesLine, lpesTmp);

		if (pBat_CheckBlocks(lpesLine) == TRUE) {

			bCorrectBlocks=TRUE;
			break;

		}

        lpAltPromptString = ">";
        pBat_OutputPromptString(lpAltPromptString);

	}

	lpAltPromptString = NULL;

    pIn->bEof=res;

	if (*(pIn->lpFileName)!='\0') {

		if (!res)
			pIn->iPos=ftell(pFile);

		fclose(pFile);

	}

	if (!*pBat_EsToChar(lpesLine)) {

		pBat_EsFree(lpesTmp);
		return 1;

	}

	if (!bCorrectBlocks) {

		pBat_ShowErrorMessage(PBAT_NONCLOSED_BLOCKS, NULL, FALSE);
		goto error;

	}

	pBat_EsFree(lpesTmp);

	pBat_RmTrailingNl(lpesLine->str);

	return 0;

error:
	pIn->bEof=TRUE;
	pBat_EsFree(lpesTmp);
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
int pBat_CheckBlocks(ESTR* lpesLine)
{
    char *pch = lpesLine->str;

    do {

        if ((*pch == '|') || (*pch == '&'))
            pch ++;

        if ((pch = pBat_GetBlockLineEndEx(pch, 0)) == NULL)
            return FALSE;

    } while ((*pch != '\n') && (*pch != '\0'));

    if (*pch)
        return TRUE;

    return -1;
}

void pBat_RmTrailingNl(char* lpLine)
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
