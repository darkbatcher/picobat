/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2015 DarkBatcher
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

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "../lang/Dos9_ShowHelp.h"

//#define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "Dos9_Call.h"

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
int Dos9_CmdCall(char* lpLine)
{
	ESTR *lpEsParameter=Dos9_EsInit(),
		 *lpEsLabel=Dos9_EsInit(),
		 *lpEsFile=Dos9_EsInit();

	char *lpCh,
	     *lpNxt,
	     *lpFile,
	     *lpLabel,
	     *lpFullLine,
	     lpExt[_MAX_EXT];

	int  bIsExtended=FALSE,
	     iNbParam=0;

	lpFullLine= lpLine = Dos9_SkipBlanks(lpLine+4);

	while (lpNxt=Dos9_GetNextParameterEs(lpLine, lpEsParameter)) {

		lpCh=Dos9_EsToChar(lpEsParameter);

		DOS9_DBG("[Call] Ok, the arg is : \"%s\"\n", lpCh);

		if (!stricmp(lpCh, "/E") && !iNbParam) {

			/* only accept this switch if this is the first parameter
			   given (see description above */

			if (bCmdlyCorrect == TRUE) {

				/* The user specified CMDLYCORRECT, so that what we
				   need is to output an error and to return from the
				   command. */

				Dos9_ShowErrorMessage(DOS9_EXTENSION_DISABLED_ERROR, lpCh, FALSE);
				goto error;

			}

			/* The extended mode was specified, so that the following
			   code may code may consider about let the command
			   use both ``label'' and ``file'' parameters */

			bIsExtended=TRUE;

		} else if (!stricmp(lpCh, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_CALL);
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

					if (*(Dos9_EsToChar(lpEsLabel))) {

						/* that parameter is already set, so that it migth
						   be the next first argument  */

						break;

					}

					Dos9_EsCpy(lpEsLabel, lpCh);

					break;

				default:
					/* this is the ``file'' parameter, obviously */

					if (*(Dos9_EsToChar(lpEsFile))) {

						/* that parameter is already set, so that it might
						   be the next first argument  */

						break;

					}

					Dos9_EsCpy(lpEsFile, lpCh);



			}

			iNbParam++;

		}

		lpLine=lpNxt;

	}

	/* Now, we have to decide wether the command is a
	   command line, a script name to be executed, and
	   label */

	lpLabel= (*(Dos9_EsToChar(lpEsLabel))) ?
	         (Dos9_EsToChar(lpEsLabel)) : (NULL);

	lpFile = (*(Dos9_EsToChar(lpEsFile))) ?
	         (Dos9_EsToChar(lpEsFile)) : (NULL);

	if ((!lpFile)
	    && (!lpLabel)) {

		/* neither ``file'' nor ``label'' were given */

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "CALL", FALSE);
		goto error;


	} else if (lpLabel) {

		/* both ``label'' was given. This is not problematic,
		   since the function Dos9_CmdCallFile can cope with
		   NULL as ``file'' */

		Dos9_CmdCallFile(lpFile, lpFullLine, lpLabel, lpLine);


	} else {

		/* this is the case were ``file'' is not null and ``label''
		   is NULL. This is tougher to solve, because we need to
		   determine ``file'' extension, to choose wether it should
		   be executed inside or outside Dos9 */

		Dos9_SplitPath(lpFile, NULL, NULL, NULL, lpExt);

		if (!stricmp(lpExt, ".bat")
		    || !stricmp(lpExt,".cmd")) {

			/* ``file'' is a batch file, indeed */

			Dos9_CmdCallFile(lpFile, lpFullLine, NULL, lpLine);

		} else {

			/* this is an executable */
			Dos9_GetEndOfLine(lpLine, lpEsParameter);
			Dos9_CmdCallExternal(lpFile, Dos9_EsToChar(lpEsParameter));

		}

	}

	Dos9_EsFree(lpEsFile);
	Dos9_EsFree(lpEsParameter);
	Dos9_EsFree(lpEsLabel);

	return 0;


error:
	Dos9_EsFree(lpEsFile);
	Dos9_EsFree(lpEsParameter);
	Dos9_EsFree(lpEsLabel);

	return -1;

}

int Dos9_CmdCallFile(char* lpFile, char* lpFull, char* lpLabel, char* lpCmdLine)
{
	INPUT_FILE ifOldFile;
	LOCAL_VAR_BLOCK lpvOldBlock[LOCAL_VAR_BLOCK_SIZE];
	LOCAL_VAR_BLOCK lpvTmpBlock[LOCAL_VAR_BLOCK_SIZE]={NULL};
	char lpAbsPath[FILENAME_MAX];

	ESTR *lpEsParam=Dos9_EsInit();
	int   c='1',
	      iLockState;

	/* We backup the old file informations */

	memcpy(&ifOldFile, &ifIn, sizeof(INPUT_FILE));

	/* Set the INPUT_INFO data */

	if (!lpLabel) {

		if (Dos9_GetFilePath(lpAbsPath, lpFile, sizeof(lpAbsPath))==-1) {

			Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFile, FALSE);

			goto error;

		}

		ifIn.bEof=FALSE;          /* the file is not at EOF */
		ifIn.iPos=0;              /* places the cursor at the origin */
		snprintf(ifIn.lpFileName,
		         sizeof(ifIn.lpFileName),
		         "%s",
		         lpAbsPath
		        );               /* sets input to given file */

		Dos9_SetLocalVar(lpvTmpBlock, '0', lpFile);

	} else if (Dos9_JumpToLabel(lpLabel, lpFile)== -1) {

		Dos9_ShowErrorMessage(DOS9_LABEL_ERROR, lpLabel, FALSE);
		goto error;

	} else {

		Dos9_SetLocalVar(lpvTmpBlock, '0', lpLabel);

	}

	/* Set scripts arguments */

	while ((lpCmdLine=Dos9_GetNextParameterEs(lpCmdLine, lpEsParam))
	       && (c <= '9')) {

		Dos9_SetLocalVar(lpvTmpBlock, c, Dos9_EsToChar(lpEsParam));

		c++;

	}

	while (c <= '9')
        Dos9_SetLocalVar(lpvTmpBlock, c++, "");

    /* set the %* parameter */
    Dos9_SetLocalVar(lpvTmpBlock, '*', lpFull);

	/* Backup the old variables data in order to be able to push old data
	   again on local variables. */

	memcpy(lpvOldBlock,
	       lpvLocalVars,
	       LOCAL_VAR_BLOCK_SIZE *sizeof(LOCAL_VAR_BLOCK));

	/* Use the block created as the new variables block. */

	memcpy(lpvLocalVars,
	       lpvTmpBlock,
	       LOCAL_VAR_BLOCK_SIZE *sizeof(LOCAL_VAR_BLOCK));

	/* lock the stream stack */

	iLockState=Dos9_GetStreamStackLockState(lppsStreamStack);
	Dos9_SetStreamStackLockState(lppsStreamStack, TRUE);


	/* run the  command */

	Dos9_RunBatch(&ifIn);

    bAbortCommand=FALSE;


	/* restore the stream stack */

	Dos9_SetStreamStackLockState(lppsStreamStack, iLockState);

	/* free variables that have been allocated while
	   executing the batch script (or the function) */

	for (c=0; c < LOCAL_VAR_BLOCK_SIZE; c++) {

        if (Dos9_IsLocalVarValid(c))
            Dos9_SetLocalVar(lpvLocalVars, c, NULL);
	}

	/* restore old settings before resuming to the old section */

	memcpy(&ifIn, &ifOldFile, sizeof(INPUT_FILE));
	memcpy(lpvLocalVars, lpvOldBlock, LOCAL_VAR_BLOCK_SIZE*sizeof(LOCAL_VAR_BLOCK));
	Dos9_EsFree(lpEsParam);

	return 0;

error:

	/* can't let the interpretor like that, restore old settings */
	memcpy(&ifIn, &ifOldFile, sizeof(INPUT_FILE));
	memcpy(lpvLocalVars, lpvOldBlock, LOCAL_VAR_BLOCK_SIZE*sizeof(LOCAL_VAR_BLOCK));
	Dos9_EsFree(lpEsParam);

	return -1;

}

int Dos9_CmdCallExternal(char* lpFile, char* lpCh)
{

	BLOCKINFO bkInfo;

	ESTR *lpEsLine=Dos9_EsInit();

	char *lpStr;

	Dos9_EsCpy(lpEsLine, lpFile);
	Dos9_EsCat(lpEsLine, " ");
	Dos9_EsCat(lpEsLine, lpCh);

	Dos9_ReplaceVars(lpEsLine);

	bkInfo.lpBegin=Dos9_EsToChar(lpEsLine);

	for (lpStr=Dos9_EsToChar(lpEsLine); *lpStr; lpStr++);

	bkInfo.lpEnd=lpStr;

	Dos9_RunBlock(&bkInfo);

	Dos9_EsFree(lpEsLine);

	return 0;

}
