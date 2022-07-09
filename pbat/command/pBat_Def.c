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


#include "../core/pBat_Core.h"
#include "../errors/pBat_Errors.h"
#include "../lang/pBat_ShowHelp.h"

#include "pBat_Def.h"

int pBat_CmdDef(char* lpLine, PARSED_LINE** lpplLine)
{

	ESTR* lpEsParam=pBat_EsInit_Cached();
	COMMANDLIST* lpclNewCommands;
	COMMANDINFO  ciCommand;

	int iReplace=FALSE,
        iList=FALSE,
        status = PBAT_NO_ERROR;

	COMMANDFLAG iRet;

	void* pVoid;
	char* lpCh;

	lpLine+=3;

	/* Cancel the commands following */
    if (lpplLine)
        *lpplLine = NULL;


	while ((lpCh=pBat_GetNextParameterEs(lpLine, lpEsParam))) {

		if (!strcmp("/?", lpEsParam->str)) {

			pBat_ShowInternalHelp(PBAT_HELP_DEF);

			goto end;

		} else if (!stricmp("/f", lpEsParam->str)) {

			iReplace=TRUE;

		} else if (!stricmp("/l", lpEsParam->str)) {

            iList = TRUE;

		} else {

            /* No delayed expansion in macro definitions, in order to be
               able to use expansion without extended used of escape-characters
               it helps to enhance readability of the code */
			lpLine=pBat_SkipAllBlanks(lpLine);

			break;

		}

		lpLine = lpCh;

	}

	if (iList) {

        pBat_ListDefs(lpLine, strlen(lpLine), lpclCommands);
        goto end;

	} else if (!(lpCh=pBat_SearchChar(lpLine, '='))) {

		pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
		                      lpLine,
		                      FALSE
							 );

        status = PBAT_UNEXPECTED_ELEMENT;

		goto end;

	}

	*lpCh='\0';
	lpCh++;

	ciCommand.cfFlag=strlen(lpLine) | PBAT_DEF_FLAG;
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


	} else if (iRet != -1) {

        /* the command name is not allowed since it is already used */

        pBat_ShowErrorMessage(PBAT_TRY_REDEFINE_COMMAND,
                              lpLine,
                              FALSE
                             );

        status = PBAT_TRY_REDEFINE_COMMAND;

    } else if ((pBat_AddCommandDynamic(&ciCommand, &lpclCommands))) {

        pBat_ShowErrorMessage(PBAT_UNABLE_ADD_COMMAND,
                              lpLine,
                              FALSE
                             );

        status = PBAT_UNABLE_ADD_COMMAND;

    } else if (!(lpclNewCommands=pBat_ReMapCommandInfo(lpclCommands))) {

        pBat_ShowErrorMessage(PBAT_UNABLE_REMAP_COMMANDS,
                              __FILE__ "/pBat_CmdAlias()",
                              FALSE
                             );

        status = PBAT_UNABLE_REMAP_COMMANDS;

    } else {

        pBat_FreeCommandList(lpclCommands);

        lpclCommands=lpclNewCommands;

    }

end:
	pBat_EsFree_Cached(lpEsParam);
	return status;
}


void pBat_ListDefs(char *name, size_t len, COMMANDLIST* commands)
{
    if ((name == NULL
        || *name == '\0'
        || !(strnicmp(name, commands->ptrCommandName, len)))
        && (commands->cfFlag & PBAT_DEF_FLAG))
        fprintf(fOutput, "%s=%s" PBAT_NL,
                    commands->ptrCommandName, commands->lpCommandProc);

    /* browse the entire tree. Note that we cannot use the strincmp() function
       to guess which branch of the tree is adaquate given *name */
    if (commands->lpclLeftRoot != NULL)
        pBat_ListDefs(name, len, commands->lpclLeftRoot);

    if (commands->lpclRightRoot != NULL)
        pBat_ListDefs(name, len, commands->lpclRightRoot);

}
