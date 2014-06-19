/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Goto.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

int Dos9_CmdGoto(char* lpLine)
{
	char lpLabelName[FILENAME_MAX];
	char lpFileName[FILENAME_MAX];
	char* lpFile=NULL;
	int bEchoError=TRUE;

	lpLine+=4;
	*lpLine=':';

	if (*(lpLine+1)==':')
		lpLine++;

	if ((lpLine=Dos9_GetNextParameter(lpLine, lpLabelName, FILENAME_MAX))) {

		if (!strcmp(lpLabelName, ":/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_GOTO);
			return 0;

		}

		if ((lpLine=Dos9_GetNextParameter(lpLine ,lpFileName, FILENAME_MAX))) {

			if (!stricmp(lpFileName, "/Q")) {

				/* on a choisi de rendre l'erreux muette */
				bEchoError=FALSE;

				if (!(Dos9_GetNextParameter(lpLine, lpFileName, FILENAME_MAX)))
					goto next;

			}

			if (bCmdlyCorrect) {

				/* if the user has set the CMDLYCORRECT flag, the capabilities of
				   calling another file's label is prohibited */

				Dos9_ShowErrorMessage(DOS9_EXTENSION_DISABLED_ERROR, NULL, FALSE);

				return -1;

			} else {

				lpFile=lpFileName;

			}

		}

	next:

		/* Now we have a valid label name, thus  we are about to find a label in the specified file */
		/* if we do have a valid file name, the search will be made in specified file */
		DOS9_DBG("Jump to Label \"%s\" in \"%s\"", lpLabelName, lpFile);

		if (!stricmp(lpLabelName,  ":EOF")) {

			/* do not even look for ``:EOF'', just
			   abort the command */

			bAbortCommand=-1;

			return 0;

		} else if (Dos9_JumpToLabel(lpLabelName, lpFile)==-1) {

			if (!bEchoError)
				Dos9_ShowErrorMessage(DOS9_LABEL_ERROR, lpLabelName, FALSE);

			return -1;

		}

	}


	/* let's set a this global variable to let the other functions
	   know that they should reload an entire line */
	bAbortCommand=TRUE;


	return 0;
}

