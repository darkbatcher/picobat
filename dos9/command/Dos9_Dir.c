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

int Dos9_CmdDir(char* lpLine)
{
	char *lpNext,
	     *lpToken,
	     lpFileName[FILENAME_MAX]= {0};

    char *buf = NULL;

	int iFlag=DOS9_SEARCH_DEFAULT | DOS9_SEARCH_DIR_MODE,
        iDirNb, iFileNb,
        status = 0;

	ESTR* lpParam=Dos9_EsInit();
    FILELIST *files, *item, *others;

    short wAttr;
    char bSimple;
    size_t nSize = 0;

    char lpType[]="D RHSA ",
	              lpSize[16];

	struct tm* lTime;

	lpNext=lpLine+3;

	wAttr=DOS9_CMD_ATTR_ALL;
	bSimple=FALSE;

    if (isatty(fileno(fOutput))) {

        if (!(buf = malloc(8192))) {

            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_CmdType()",
                                    0);
            status = DOS9_FAILED_ALLOCATION;
            goto end;

        }

        setvbuf(fOutput, buf, _IOFBF, 8192);
    }

	while ((lpNext=Dos9_GetNextParameterEs(lpNext, lpParam))) {

		lpToken=Dos9_EsToChar(lpParam);
		if (!strcmp(lpToken, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_DIR);
			goto end;

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
				status = DOS9_UNEXPECTED_ELEMENT;
				goto end;

			}

			if (!TEST_ABSOLUTE_PATH(lpToken))
                nSize = strlen(lpCurrentDir) + 1;
            else
                nSize = 0;

			Dos9_MakeFullPath(lpFileName, lpToken, FILENAME_MAX);

		}
	}

	if (!*lpFileName) {
		/* if no file or directory name have been specified
		   the put a correct value on it */

		snprintf(lpFileName, FILENAME_MAX, "%s" DEF_DELIMITER "*", lpCurrentDir);
		nSize = strlen(lpCurrentDir) + 1;

	}


	/* do a little global variable setup before
	   starting file research */
	iDirNb=0;
	iFileNb=0;

	if (!bSimple) {

        fputs(DOS9_NL, fOutput);
        fputs(lpDirListTitle, fOutput);
        fputs(DOS9_NL, fOutput);

	}

	if (!(files = Dos9_GetMatchFileList(lpFileName, iFlag))
        && !bSimple) {

        fputs(lpDirNoFileFound, fOutput);
        fputs(DOS9_NL, fOutput);

	} else  {

        Dos9_AttributesSplitFileList(wAttr, files, &files, &others);

        if (others)
            Dos9_FreeFileList(others);

        item = files;

        while (item) {

            if (!bSimple) {

                strcpy(lpType, "       ");
                if (Dos9_GetFileMode(item) & DOS9_FILE_DIR) {
                    lpType[0]='D';
                    iDirNb++;

                    strcpy(lpSize, "<REP>\t");

                } else {

                    strcpy(lpSize, "       ");
                    Dos9_FormatFileSize(lpSize+7, 8, Dos9_GetFileSize(item));

                    iFileNb++;

                }

                if (Dos9_GetFileMode(item) & DOS9_FILE_READONLY) lpType[2]='R';

                if (Dos9_GetFileMode(item) & DOS9_FILE_HIDDEN) lpType[3]='H';

                if (Dos9_GetFileMode(item) & DOS9_FILE_SYSTEM) lpType[4]='S';

                if (Dos9_GetFileMode(item) & DOS9_FILE_ARCHIVE) lpType[5]='A';

                /* !! Recyclage de la variable lpFilename pour afficher la taille du fichier */


                lTime=localtime(&Dos9_GetModifTime(item));

                fprintf(fOutput, "%02d/%02d/%02d %02d:%02d %s\t%s\t%s" DOS9_NL, lTime->tm_mday,
                                                                lTime->tm_mon+1,
                                                                1900+lTime->tm_year,
                                                                lTime->tm_hour,
                                                                lTime->tm_min,
                                                                lpSize,
                                                                lpType,
                                                                item->lpFileName + nSize
                                                                );

            } else {

                if (bCmdlyCorrect && (iFlag & DOS9_SEARCH_RECURSIVE))
                    fprintf(fOutput, "%s" DOS9_NL, item->lpFileName);
                else
                    fprintf(fOutput, "%s" DOS9_NL, item->lpFileName + nSize);

            }

            item = item->lpflNext;

        }

        Dos9_FreeFileList(files);

        if (!bSimple)
            fprintf(fOutput, "\t\t\t\t%d %s" DOS9_NL "\t\t\t\t%d %s" DOS9_NL,
                                iFileNb, lpDirFile, iDirNb, lpDirDir);

	}


end:
	Dos9_EsFree(lpParam);


    if (isatty(fileno(fOutput))) {

        fflush(fOutput);
        setvbuf(fOutput, NULL, _IONBF, 0);

        if (buf)
            free(buf);

    }

	return status;
}
