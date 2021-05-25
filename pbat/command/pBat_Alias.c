/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2014 DarkBatcher
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

#include "pBat_Alias.h"
#include "../core/pBat_Core.h"

#include "../errors/pBat_Errors.h"
#include "../lang/pBat_ShowHelp.h"

int pBat_CmdAlias(char* lpLine)
{

	ESTR* lpEsParam=pBat_EsInit();
	COMMANDLIST* lpclNewCommands;
	COMMANDINFO  ciCommand;

	int iReplace=FALSE,
        status = PBAT_NO_ERROR;

	COMMANDFLAG iRet;

	void* pVoid;

	char* lpCh;

	lpLine+=5;

	while ((lpCh=pBat_GetNextParameterEs(lpLine, lpEsParam))) {

		if (!strcmp("/?", pBat_EsToChar(lpEsParam))) {

			pBat_ShowInternalHelp(PBAT_HELP_ALIAS);

			goto error;

		} else if (!stricmp("/f", pBat_EsToChar(lpEsParam))) {

			iReplace=TRUE;

		} else {

			pBat_GetEndOfLine(lpLine, lpEsParam);
			lpLine=pBat_EsToChar(lpEsParam);

			lpLine=pBat_SkipAllBlanks(lpLine);

			break;

		}

		lpLine=lpCh;

	}

	if (!(lpCh=pBat_SearchChar(lpLine, '='))) {

		pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
		                      lpLine,
		                      FALSE
							 );

        status = PBAT_UNEXPECTED_ELEMENT;

		goto error;

	}

	*lpCh='\0';
	lpCh++;

	ciCommand.cfFlag=strlen(lpLine) | PBAT_ALIAS_FLAG;
	ciCommand.ptrCommandName=lpLine;

	ciCommand.lpCommandProc=lpCh;

	iRet=pBat_GetCommandProc(lpLine, lpclCommands, &pVoid);

	if (iReplace && (iRet!=-1)) {

		/* it is possible to reassign pBat internal commands. I decided
		   to allow this because it may be a funny trick to hack around for
		   example, if some batch requires some incompatible features it
		   allows to redefine those commands in order to get compatibility */

		if ((pBat_ReplaceCommand(&ciCommand, lpclCommands))) {

			/* if we fail to reassign command, print an error message */

			pBat_ShowErrorMessage(PBAT_UNABLE_REPLACE_COMMAND,
								  lpLine,
								  FALSE
								 );

            status = PBAT_UNABLE_REPLACE_COMMAND;

		}

		pBat_EsFree(lpEsParam);

		return status;

	}

	if (iRet!=-1) {

		/* the command name is not allowed since it is already used */

		pBat_ShowErrorMessage(PBAT_TRY_REDEFINE_COMMAND,
							  lpLine,
							  FALSE
							 );

        status = PBAT_TRY_REDEFINE_COMMAND;

		goto error;

	}

	if ((pBat_AddCommandDynamic(&ciCommand, &lpclCommands))) {

		pBat_ShowErrorMessage(PBAT_UNABLE_ADD_COMMAND,
		                      lpLine,
		                      FALSE
		                     );

        status = PBAT_UNABLE_ADD_COMMAND;

		goto error;

	}

	if (!(lpclNewCommands=pBat_ReMapCommandInfo(lpclCommands))) {

		pBat_ShowErrorMessage(PBAT_UNABLE_REMAP_COMMANDS,
		                      __FILE__ "/pBat_CmdAlias()",
		                      FALSE
		                     );

		status = PBAT_UNABLE_REMAP_COMMANDS;

		goto error;

	}

	pBat_FreeCommandList(lpclCommands);

	lpclCommands=lpclNewCommands;

error:
	pBat_EsFree(lpEsParam);
	return status;
}
