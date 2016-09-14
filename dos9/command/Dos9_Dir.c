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
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "Dos9_Dir.h"

static int iDirNb,
    iFileNb;
static short wAttr;
static char bSimple;

void Dos9_CmdDirShow(FILELIST* lpElement)
{
	char lpType[]="D RHSA ",
	              lpSize[16];

	struct tm* lTime;

	/* This structures get only one argument at a time,
	   thus ``lpElement->lpNext'' is inconsistent */

	if (Dos9_CheckFileAttributes(wAttr, lpElement)) {

		/* if the file has right attributes */

		if (!bSimple) {

			strcpy(lpType, "       ");
			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_DIR) {
				lpType[0]='D';
				iDirNb++;

				strcpy(lpSize, "<REP>\t");

			} else {

                strcpy(lpSize, "       ");
                Dos9_FormatFileSize(lpSize+7, 8, Dos9_GetFileSize(lpElement));

				iFileNb++;

			}

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_READONLY) lpType[2]='R';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_HIDDEN) lpType[3]='H';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_SYSTEM) lpType[4]='S';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_ARCHIVE) lpType[5]='A';

			/* !! Recyclage de la variable lpFilename pour afficher la taille du fichier */


			lTime=localtime(&Dos9_GetModifTime(lpElement));

			printf("%02d/%02d/%02d %02d:%02d %s\t%s\t%s\n", lTime->tm_mday,
                                                            lTime->tm_mon+1,
                                                            1900+lTime->tm_year,
                                                            lTime->tm_hour,
                                                            lTime->tm_min,
                                                            lpSize,
                                                            lpType,
                                                            lpElement->lpFileName
                                                            );

		} else {

			printf("%s\n", lpElement->lpFileName);

		}
	}
}

int Dos9_CmdDir(char* lpLine)
{
	char *lpNext,
	     *lpToken,
	     lpFileName[FILENAME_MAX]= {0};

	int iFlag=DOS9_SEARCH_DEFAULT | DOS9_SEARCH_DIR_MODE;

	ESTR* lpParam=Dos9_EsInit();

	//if (lpNext=strchr(lpLine, ' ')) *lpNext='\0';
	lpNext=lpLine+3;

	wAttr=DOS9_CMD_ATTR_ALL;
	bSimple=FALSE;

	while ((lpNext=Dos9_GetNextParameterEs(lpNext, lpParam))) {

		lpToken=Dos9_EsToChar(lpParam);
		if (!strcmp(lpToken, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_DIR);
			return 0;

		} else if (!stricmp("/b", lpToken)) {

			/* use the simple dir output */
			bSimple=TRUE;
			if (!wAttr) iFlag|=DOS9_SEARCH_NO_STAT;
			iFlag|=DOS9_SEARCH_NO_PSEUDO_DIR;

		} else if (!stricmp("/s", lpToken)) {

			/* set the command to recusive */
			iFlag|=DOS9_SEARCH_RECURSIVE;

		} else if (!strnicmp("/a", lpToken,2)) {

			/* uses the attributes command */
			lpToken+=2;
			if (*lpToken==':') lpToken++;
			wAttr=Dos9_MakeFileAttributes(lpToken);
			iFlag&=~DOS9_SEARCH_NO_STAT;

		} else {

			if (*lpFileName) {
				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
				Dos9_EsFree(lpParam);
				return -1;
			}
			strncpy(lpFileName, lpToken, FILENAME_MAX);

		}
	}

	if (!*lpFileName) {
		/* if no file or directory name have been specified
		   the put a correct value on it */

		*lpFileName='*';
		lpFileName[1]='\0';

	}

	/* do a little global variable setup before
	   starting file research */
	iDirNb=0;
	iFileNb=0;

	if (!bSimple)
        puts(lpDirListTitle);

	/* Get a list of file and directories matching to the
	   current filename and options set */
	if (!(Dos9_GetMatchFileCallback(lpFileName, iFlag, Dos9_CmdDirShow))
	    && !bSimple)
		puts(lpDirNoFileFound);

	if (!bSimple) printf(lpDirFileDirNb, iFileNb, iDirNb);

	Dos9_EsFree(lpParam);

	return 0;
}
