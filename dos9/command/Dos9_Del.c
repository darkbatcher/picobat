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

#include "Dos9_Commands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#include <libDos9.h>

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"

#include "Dos9_Del.h"

#include "Dos9_Ask.h"

/* TODO : Make extended test and decide wether deleting
   files should really be enabled */

int Dos9_CmdDel(char* lpLine)
{
	char *lpNextToken,
	     *lpToken;
	ESTR* lpEstr=Dos9_EsInit();

	char  bParam=0,
	      bDel=TRUE;

	short wAttr=0;

	char  lpName[FILENAME_MAX]="\0";

	int iFlag=DOS9_SEARCH_DEFAULT,
	    iChoice;

	FILELIST* lpFileList, *lpSaveList;

	if (!(lpLine=strchr(lpLine, ' '))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "DEL / ERASE", FALSE);
		Dos9_EsFree(lpEstr);
		return -1;

	}

	while ((lpNextToken=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		lpToken=Dos9_EsToChar(lpEstr);

		if (!stricmp(lpToken, "/P")) {

			/* Demande une confirmation avant la suppression */

			bParam|=DOS9_ASK_CONFIRMATION;

		} else if (!stricmp(lpToken, "/F")) {

			/* supprime les fichiers en lecture seule */

			bParam|=DOS9_DELETE_READONLY;

		} else if (!stricmp(lpToken, "/S")) {

			/* procède récursivement */

			iFlag|=DOS9_SEARCH_RECURSIVE;

		} else if (!stricmp(lpToken, "/Q")) {

			/* pas de confirmation pour suppression avec caractères génériques */

			bParam|=DOS9_DONT_ASK_GENERIC;

		} else if (!strnicmp(lpToken, "/A", 2)) {

			/* gestion des attributs */
			lpToken+=2;
			if (*lpToken==':') lpToken++;

			wAttr=Dos9_MakeFileAttributes(lpToken);

			iFlag^=DOS9_SEARCH_NO_STAT;

		} else if (!strcmp("/?", lpToken)) {

			Dos9_ShowInternalHelp(DOS9_HELP_DEL);

			Dos9_EsFree(lpEstr);
			return -1;

		} else {

			if (*lpName != '\0') {

				/* si un nom a été donné, on affiche, l'erreur */

				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
				Dos9_EsFree(lpEstr);
				return -1;

			}

			strncpy(lpName, lpToken, FILENAME_MAX);
		}

		lpLine=lpNextToken;

	}

	if (*lpName== '\0') { /* si aucun nom n'a été donné */

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "DEL / ERASE", FALSE);
		Dos9_EsFree(lpEstr);
		return -1;

	}

	if (!(bParam & DOS9_DELETE_READONLY)) {

		/* par défaut, la recherche ne prend pas en compte la recherche des fichiers en lecture seule */
		wAttr|=DOS9_CMD_ATTR_READONLY_N | DOS9_CMD_ATTR_READONLY;

	}

	/* ne sélectionne que le fichiers */
	wAttr|=DOS9_CMD_ATTR_DIR | DOS9_CMD_ATTR_DIR_N;

	if ((Dos9_StrToken(lpName, '*') || Dos9_StrToken(lpName, '?'))
	    && !(bParam & DOS9_DONT_ASK_GENERIC)) {

		/* si la recherche est générique on met la demande de
		  confirmation sauf si `/Q` a été spécifié */
		bParam|=DOS9_ASK_CONFIRMATION;
	}

	printf("Looking for `%s'\n", lpName);

	if ((lpFileList=Dos9_GetMatchFileList(lpName, iFlag))) {
		lpSaveList=lpFileList;
		while (lpFileList) {

			/* on demande confirmation si la demande de confirmation est
			   active */
			if (bParam & DOS9_ASK_CONFIRMATION) {

				iChoice=Dos9_AskConfirmation(DOS9_ASK_YNA
				                             | DOS9_ASK_INVALID_REASK
				                             | DOS9_ASK_DEFAULT_Y,
				                             lpDelConfirm,
				                             lpFileList->lpFileName
				                            );

				if (iChoice==DOS9_ASK_NO) {
					/* si l'utilisateur refuse la suppression du fichier */
					printf("Pas suppression !\n");
					lpFileList=lpFileList->lpflNext;
					continue;

				} else if (iChoice==DOS9_ASK_ALL) {
					/* si l'utilisateur autorise tous les fichiers */
					bParam&=~DOS9_ASK_CONFIRMATION;

				}
			}

			/* on vérifie que le fichier possède les bons attributs pour la suppression */
			if ((Dos9_CheckFileAttributes(wAttr, lpFileList))) {

				printf("<DEBUG> supression de `%s'\n", lpFileList->lpFileName);

			} else {

				printf("<DEBUG> Fichier `%s' non suprimmé (attributs incorrects)\n", lpFileList->lpFileName);

			}

			/* on passe au fichier suivant */
			lpFileList=lpFileList->lpflNext;
		}

		Dos9_FreeFileList(lpSaveList);
	} else {

		Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpName, FALSE);
		Dos9_EsFree(lpEstr);

		return -1;

	}

	Dos9_EsFree(lpEstr);
	return 0;
}

