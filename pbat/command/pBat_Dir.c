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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Commands.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

#include "pBat_Dir.h"

int pBat_CmdDir(char* lpLine)
{
	char *lpNext,
	     *lpToken,
	     lpFileName[FILENAME_MAX]= {0};

    char *buf = NULL;

	int iFlag=PBAT_SEARCH_DEFAULT | PBAT_SEARCH_DIR_MODE,
        iDirNb, iFileNb,
        status = 0;

	ESTR* lpParam=pBat_EsInit_Cached();
    FILELIST *files, *item, *others;

    short wAttr;
    char bSimple;
    size_t nSize = 0;

    char lpType[]="D RHSA ",
	              lpSize[16],
	              lpTime[30];

	struct tm lTime, *t;

	lpNext=lpLine+3;

	wAttr=PBAT_CMD_ATTR_ALL;
	bSimple=FALSE;

    if (isatty(fileno(fOutput))) {

        if (!(buf = malloc(8192))) {

            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_CmdType()",
                                    0);
            status = PBAT_FAILED_ALLOCATION;
            goto end;

        }

        setvbuf(fOutput, buf, _IOFBF, 8192);
    }

	while ((lpNext=pBat_GetNextParameterEs(lpNext, lpParam))) {

		lpToken=pBat_EsToChar(lpParam);
		if (!strcmp(lpToken, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_DIR);
			goto end;

		} else if (!stricmp("/b", lpToken)) {

			/* use the simple dir output */
			bSimple=TRUE;
			if (!wAttr) iFlag|=PBAT_SEARCH_NO_STAT;
			iFlag|=PBAT_SEARCH_NO_PSEUDO_DIR;

		} else if (!stricmp("/s", lpToken)) {

			/* set the command to recusive */
			iFlag|=PBAT_SEARCH_RECURSIVE;

		} else if (!strnicmp("/a", lpToken,2)) {

			/* uses the attributes command */
			lpToken+=2;
			if (*lpToken==':') lpToken++;
			wAttr=pBat_MakeFileAttributes(lpToken);
			iFlag&=~PBAT_SEARCH_NO_STAT;

		} else {

			if (*lpFileName) {

				pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, lpToken, FALSE);
				status = PBAT_UNEXPECTED_ELEMENT;
				goto end;

			}

			if (!PBAT_TEST_ABSOLUTE_PATH(lpToken))
                nSize = strlen(lpCurrentDir) + 1;
            else
                nSize = 0;

			pBat_MakeFullPath(lpFileName, lpToken, FILENAME_MAX);

		}
	}

	if (!*lpFileName) {
		/* if no file or directory name have been specified
		   the put a correct value on it */

		snprintf(lpFileName, FILENAME_MAX, "%s" PBAT_DEF_DELIMITER "*", lpCurrentDir);
		nSize = strlen(lpCurrentDir) + 1;

	}


	/* do a little global variable setup before
	   starting file research */
	iDirNb=0;
	iFileNb=0;

	if (!bSimple) {

        fputs(PBAT_NL, fOutput);
        fputs(lpDirListTitle, fOutput);
        fputs(PBAT_NL, fOutput);

	}

	if (!(files = pBat_GetMatchFileList(lpFileName, iFlag))
        && !bSimple) {

        fputs(lpDirNoFileFound, fOutput);
        fputs(PBAT_NL, fOutput);

	} else  {

        pBat_AttributesSplitFileList(wAttr, files, &files, &others);

        if (others)
            pBat_FreeFileList(others);

        item = files;

        while (item) {

            if (!bSimple) {

                strcpy(lpType, "       ");
                if (pBat_GetFileMode(item) & PBAT_FILE_DIR) {
                    lpType[0]='D';
                    iDirNb++;

                    strcpy(lpSize, "<REP>\t");

                } else {

                    strcpy(lpSize, "       ");
                    pBat_FormatFileSize(lpSize+7, 8, pBat_GetFileSize(item));

                    iFileNb++;

                }

                if (pBat_GetFileMode(item) & PBAT_FILE_READONLY) lpType[2]='R';

                if (pBat_GetFileMode(item) & PBAT_FILE_HIDDEN) lpType[3]='H';

                if (pBat_GetFileMode(item) & PBAT_FILE_SYSTEM) lpType[4]='S';

                if (pBat_GetFileMode(item) & PBAT_FILE_ARCHIVE) lpType[5]='A';

                /* !! Recyclage de la variable lpFilename pour afficher la taille du fichier */

#ifdef WIN32
#define localtime_r(t, lt) memcpy(lt, localtime(t), sizeof(struct tm))
#endif // WIN32

                localtime_r(&(pBat_GetModifTime(item)), &lTime);
                strftime(lpTime, sizeof(lpTime), "%x %X", &lTime);
                fprintf(fOutput, "%s %s\t%s\t%s" PBAT_NL, lpTime,
                                                                lpSize,
                                                                lpType,
                                                                item->lpFileName + nSize
                                                                );

            } else {

                if (bCmdlyCorrect && (iFlag & PBAT_SEARCH_RECURSIVE))
                    fprintf(fOutput, "%s" PBAT_NL, item->lpFileName);
                else
                    fprintf(fOutput, "%s" PBAT_NL, item->lpFileName + nSize);

            }

            item = item->lpflNext;

        }

        pBat_FreeFileList(files);

        if (!bSimple)
            fprintf(fOutput, "\t\t\t\t%d %s" PBAT_NL "\t\t\t\t%d %s" PBAT_NL,
                                iFileNb, lpDirFile, iDirNb, lpDirDir);

	}


end:
	pBat_EsFree_Cached(lpParam);


    if (isatty(fileno(fOutput))) {

        fflush(fOutput);
        setvbuf(fOutput, NULL, _IONBF, 0);

        if (buf)
            free(buf);

    }

	return status;
}
