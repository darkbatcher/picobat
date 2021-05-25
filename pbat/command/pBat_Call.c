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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "../lang/pBat_ShowHelp.h"

//#define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "pBat_Call.h"

/* CALL [/e] [file] [:label] [parameters ...]

   /e 				: extended mode (specify file and label)
   file 			: file to be executed (or in which look for label)
   :label 			: label to be executed
   parameters ... 	: parameters to be given to the script.

   As a special exception to our guidelines (independent order of switches),
   the /e switch may only be located right after the call command, this is
   intended for compatibility purposes. Hence we could easily imagine a
   command that takes a /e switch that would confuse call command.
*/
int pBat_CmdCall(char* lpLine)
{
	ESTR *lpEsParameter=pBat_EsInit(),
		 *lpEsLabel=pBat_EsInit(),
		 *lpEsFile=pBat_EsInit();

	char *lpCh,
	     *lpNxt,
	     *lpFile,
	     *lpLabel,
	     *lpFullLine,
	     lpExt[_MAX_EXT],
	     lpFullName[FILENAME_MAX];

	int  bIsExtended=FALSE,
	     iNbParam=0,
	     status = PBAT_NO_ERROR;

	lpFullLine= lpLine = pBat_SkipBlanks(lpLine+4);

	while ((lpNxt=pBat_GetNextParameterEs(lpLine, lpEsParameter))) {

		lpCh=pBat_EsToChar(lpEsParameter);

		PBAT_DBG("[Call] Ok, the arg is : \"%s\"\n", lpCh);

		if (!stricmp(lpCh, "/E") && !iNbParam) {

			/* only accept this switch if this is the first parameter
			   given (see description above */

			if (bCmdlyCorrect == TRUE) {

				/* The user specified CMDLYCORRECT, so that what we
				   need is to output an error and to return from the
				   command. */

				pBat_ShowErrorMessage(PBAT_EXTENSION_DISABLED_ERROR, lpCh, FALSE);
				status = PBAT_EXTENSION_DISABLED_ERROR;

				goto error;

			}

			/* The extended mode was specified, so that the following
			   code may code may consider about let the command
			   use both ``label'' and ``file'' parameters */

			bIsExtended=TRUE;

		} else if (!stricmp(lpCh, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_CALL);
			goto error;

		} else {

			/* If it is not a switch, then it must be either the label
			   parameter or the file parameter */

			if ((iNbParam && (bIsExtended == FALSE))
			    || (iNbParam==2)) {

				break;

			}

			switch(*lpCh) {

				case ':' :
					/* this is a label, obviously, so that we must
					   have parameter ``label'' */

					if (*(pBat_EsToChar(lpEsLabel))) {

						/* that parameter is already set, so that it might
						   be the next first argument  */

						break;

					}

					pBat_EsCpy(lpEsLabel, lpCh);

					break;

				default:
					/* this is the ``file'' parameter, obviously */

					if (*(pBat_EsToChar(lpEsFile))) {

						/* that parameter is already set, so that it might
						   be the next first argument  */

						break;

					}

					pBat_EsCpy(lpEsFile, lpCh);



			}

			iNbParam++;

		}

		lpLine=lpNxt;

	}

	/* Now, we have to decide wether the command is a
	   command line, a script name to be executed, and
	   label */

	lpLabel= (*(pBat_EsToChar(lpEsLabel))) ?
	         (pBat_EsToChar(lpEsLabel)) : (NULL);

	lpFile = (*(pBat_EsToChar(lpEsFile))) ?
	         (pBat_EsToChar(lpEsFile)) : (NULL);

	if ((!lpFile)
	    && (!lpLabel)) {

		/* neither ``file'' nor ``label'' were given */

		pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "CALL", FALSE);

		status = PBAT_EXPECTED_MORE;
		goto error;


	} else if (lpLabel) {

		/* ``label'' was given. This is not problematic,
		   since the function pBat_CmdCallFile can cope with
		   NULL as ``file'' */

		status = pBat_CmdCallFile(lpFile, lpFullLine, lpLabel, lpLine);


	} else {

		/* this is the case were ``file'' is not null and ``label''
		   is NULL. This is tougher to solve, because we need to
		   determine ``file'' extension, to choose wether it should
		   be executed inside or outside pBat */

        if (pBat_GetFilePath(lpFullName, lpFile, sizeof(lpFullName)) == -1) {

            pBat_ShowErrorMessage(PBAT_COMMAND_ERROR, lpFile, 0);

            status = PBAT_COMMAND_ERROR;
            goto error;
        }

		pBat_SplitPath(lpFullName, NULL, NULL, NULL, lpExt);


		if (!stricmp(lpExt, ".bat")
		    || !stricmp(lpExt,".cmd")) {

			/* ``file'' is a batch file, indeed */

			status = pBat_CmdCallFile(lpFullName, lpFullLine, NULL, lpLine);

		} else {

			/* this is an executable */
			pBat_GetEndOfLine(lpLine, lpEsParameter);
			status = pBat_CmdCallExternal(lpFullName, pBat_EsToChar(lpEsParameter));

		}

	}

error:
	pBat_EsFree(lpEsFile);
	pBat_EsFree(lpEsParameter);
	pBat_EsFree(lpEsLabel);

	return status ? status : iErrorLevel;
}

int pBat_CmdCallFile(char* lpFile, char* lpFull, char* lpLabel, char* lpCmdLine)
{
	INPUT_FILE ifOldFile;
	LOCAL_VAR_BLOCK* lpvOldBlock;
	LOCAL_VAR_BLOCK lpvTmpBlock[LOCAL_VAR_BLOCK_SIZE]={NULL};
	LOCAL_VAR_BLOCK* lpvOldArgs;
	LOCAL_VAR_BLOCK lpvTmpArgs[LOCAL_VAR_BLOCK_SIZE]={NULL};
	char lpAbsPath[FILENAME_MAX];
	int status = PBAT_NO_ERROR;

	ESTR *lpEsParam=pBat_EsInit(),
         *lpEsCmd
         /*, *lpEsTmp */;
	int   c='1',
	      iLockState;

    if (lpFile != NULL
        && pBat_GetFileFullPath(lpAbsPath, lpFile, sizeof(lpAbsPath))==-1) {

			pBat_ShowErrorMessage(PBAT_FILE_ERROR, lpFile, FALSE);

            status = PBAT_FILE_ERROR;
			goto error;

    }

    	/* We backup the old file informations */
	memcpy(&ifOldFile, &ifIn, sizeof(INPUT_FILE));

	/* This does nothing but just marking &(ifIn.batch) as
	   copy of &(iOldFile.batch) and so not be freed, all
	   the content has already be copied in the previous line */
	ifIn.batch.cpy = 1;

	/* Set the INPUT_INFO FILE */
	if (!lpLabel) {

		ifIn.bEof=FALSE;          /* the file is not at EOF */
		ifIn.iPos=0;              /* places the cursor at the origin */
		snprintf(ifIn.lpFileName,
		         sizeof(ifIn.lpFileName),
		         "%s",
		         lpAbsPath
		        );               /* sets input to given file */

		pBat_SetLocalVar(lpvTmpArgs, '0', lpAbsPath);

	} else if (pBat_JumpToLabel(lpLabel, lpFile ? lpAbsPath : NULL)== -1) {

		pBat_ShowErrorMessage(PBAT_LABEL_ERROR, lpLabel, FALSE);

		status = PBAT_LABEL_ERROR;
		goto error;

	} else {

		pBat_SetLocalVar(lpvTmpArgs, '0', lpLabel);

	}

    /* set the %* parameter */
    pBat_GetEndOfLine(lpCmdLine, lpEsParam);
    pBat_SetLocalVar(lpvTmpArgs, '*', lpEsParam->str);

    /* Set scripts arguments */
	while ((c <= '9')
            && (lpCmdLine=pBat_GetNextParameterEs(lpCmdLine, lpEsParam))) {

		pBat_SetLocalVar(lpvTmpArgs, c, pBat_EsToChar(lpEsParam));

		c++;

	}

	if (c <= '9') {

        /* empty remaining vars */
        while (c <= '9')
            pBat_SetLocalVar(lpvTmpArgs, c++, "");

        pBat_SetLocalVar(lpvTmpArgs, '+', "");

	} else if (lpCmdLine) {

        lpEsCmd = pBat_EsInit();

        while ((lpCmdLine = pBat_GetNextParameterEs(lpCmdLine, lpEsParam))) {
            pBat_EsReplace(lpEsParam, " ", "^ ");
            pBat_EsReplace(lpEsParam, ";", "^;");
            pBat_EsReplace(lpEsParam, ",", "^,");
            pBat_EsReplace(lpEsParam, "\t", "^\t");
            pBat_EsReplace(lpEsParam, "\"", "^\"");

            pBat_EsCatE(lpEsCmd, lpEsParam);
            pBat_EsCat(lpEsCmd, " ");

        }


        /* set the %+ parameter */
        pBat_SetLocalVar(lpvTmpArgs, '+', lpEsCmd->str);

        pBat_EsFree(lpEsCmd);

	} else {

	    pBat_SetLocalVar(lpvTmpArgs, '+', "");

	}

	/* Backup the old variables data in order to be able to push old data
	   again on local variables. */

    lpvOldBlock = lpvLocalVars;
    lpvOldArgs = lpvArguments;

	/* Use the block created as the new variables block. */

    lpvLocalVars = lpvTmpBlock;
    lpvArguments = lpvTmpArgs;

	/* lock the stream stack */

	iLockState=pBat_GetStreamStackLockState(lppsStreamStack);
	pBat_SetStreamStackLockState(lppsStreamStack, TRUE);

    /* Save echo state of the caller */

	/* run the  command */

	pBat_RunBatch(&ifIn);

    bAbortCommand=FALSE;

	/* restore the stream stack */

	pBat_SetStreamStackLockState(lppsStreamStack, iLockState);

	/* free variables that have been allocated while
	   executing the batch script (or the function) */

	for (c=0; c < LOCAL_VAR_BLOCK_SIZE; c++) {

        if (pBat_IsLocalVarValid(c)) {
            /* This is really important to prevent any kind of memory
               leakage */

            pBat_SetLocalVar(lpvLocalVars, c, NULL);
            pBat_SetLocalVar(lpvArguments, c, NULL);
        }
	}

	/* free the newly allocated batch script */
    pBat_FreeBatchScript(&(ifIn.batch));

	/* restore old settings before resuming to the old section */
	memcpy(&ifIn, &ifOldFile, sizeof(INPUT_FILE));

	lpvLocalVars = lpvOldBlock;
	lpvArguments = lpvOldArgs;

error:
	pBat_EsFree(lpEsParam);
	return status ? status : iErrorLevel; /* do not affect errorlevel */
}

int pBat_CmdCallExternal(char* lpFile, char* lpCh)
{

	BLOCKINFO bkInfo;

	ESTR *lpEsLine=pBat_EsInit();

	char *lpStr;

	pBat_EsCpy(lpEsLine, lpFile);
	pBat_EsCat(lpEsLine, " ");
	pBat_EsCat(lpEsLine, lpCh);

    /* Offer a free new expansion turn  */
	pBat_ReplaceVars(lpEsLine);

	bkInfo.lpBegin=pBat_EsToChar(lpEsLine);

	for (lpStr=pBat_EsToChar(lpEsLine); *lpStr; lpStr++);

	bkInfo.lpEnd=lpStr;

	pBat_RunBlock(&bkInfo);

	pBat_EsFree(lpEsLine);

	return iErrorLevel; /* do not affect errorlevel */

}
