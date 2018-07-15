/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2017 Romain Garbi
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "libDos9.h"

#include "Dos9_Core.h"
#include "../../config.h"

#include "../errors/Dos9_Errors.h"

int Dos9_InitVar(char* lpArray[])
{
	int i;
	for (i=0; lpArray[i] && lpArray[i+1]; i+=2) {
		Dos9_SetEnv(lpeEnv, lpArray[i], lpArray[i+1]);
	}
	return 0;
}

int Dos9_TestLocalVarName(char cVar)
{
	if ((cVar & 0x80) || (cVar <= 0x20)) {

		/* if the name is not strict ASCII, it
		   is not conformant */

		return -1;
	}

	return 0;
}

#ifdef DOS9_USE_LIBCU8
/* count the number of u8 chars in the string, this is
   useful to prevent cutting var inside a u8 char */
size_t __inline__ Dos9_CountU8Chars(const char* ch)
{
    size_t ret=0;

    while(*ch) {
        ret += ((*ch & 0xC0) != 0x80) ? 1 : 0;
        ch++;
    }

    return ret;
}

 __inline__ char* Dos9_SkipU8Chars(char* ch, size_t cnt)
{
    while (cnt) {
        cnt --;
        ch ++;

        while ((*ch & 0xC0) == 0x80)
            ch++;
    }

    return ch;
}
#endif // DOS9_USE_LIBCU8

void Dos9_GetTimeBasedVar(char type, char* buf, size_t size)
{
    struct tm lt;
    time_t t;
    size_t s;

#ifdef WIN32
#define localtime_r(t, lt) memcpy(lt, localtime(t), sizeof(struct tm));

    SYSTEMTIME st;
    GetSystemTime(&st);
#endif // WIN32

    t = time(NULL);
    localtime_r(&t, &lt);

    strftime(buf, size, (type == 'T' || type == 't') ? "%X" : "%x", &lt);

    if (type == 'T' || type == 't') {
        /* still cs to write */

        s = strlen(buf);

        buf += s;
        size -= s;

#ifdef WIN32
        snprintf(buf, size, ",%d", st.wMilliseconds / 10);
#else
        snprintf(buf, size, "%d", 0);
#endif // WIN32
    }
}

int Dos9_GetVar(char* lpName, ESTR* lpRecieve)
{
	char        *lpVarContent, /* a pointer to the environment var string */
	            *lpToken, /* a pointer used to tokenize vars like %var:a=b% */
	            *lpNextToken=NULL, /* a pointer used to tokenize '=' or ',' in vars like %var:a=b% */
	             *lpNameCpy, /* a pointer used to duplicate lpName (because function should avoid bordering effect)*/
	             *lpZeroPos=NULL;; /* a pointer to the zero put in the environment string */

	char        lpBuf[30];
	int         iVarState=0, /* the status of the var interpreter 1 means replace, 2 means cut */
	            iTotalLen,
	            iBegin=0, /* the start position */
	            iLen=0; /* the lenght to be cut */

	char        cCharSave=0; /* the backup of the character replaced by '\0' */;

	/* empty the line */
	*Dos9_EsToChar(lpRecieve) = '\0';

	if (!(lpNameCpy=strdup(lpName)))
		return FALSE;

	if ((lpToken=strchr(lpNameCpy, ':'))) {
		if ((lpNextToken=strchr(lpToken, '='))) {

			/* char are about to be replaced */
			*lpToken='\0';
			lpToken++;
			*lpNextToken='\0';
			lpNextToken++;
			iVarState=1;

		} else if (*(lpToken+1)=='~') {
			/* string is about to be truncated */
			*lpToken='\0';
			lpToken+=2;

			if ((lpNextToken=strchr(lpToken, ','))) {
				*lpNextToken='\0';
				lpNextToken++;
				iLen=atol(lpNextToken);
			}

			iBegin=atol(lpToken);
			iVarState=2;

		} else {

            *lpToken='\0';
            lpToken++;
            iVarState=3;

		}

	}

	if (!stricmp(lpNameCpy, "CD")) {

        /* requested current dir */
        lpVarContent=lpCurrentDir;

	} else if (!(stricmp(lpNameCpy, "ERRORLEVEL"))) {

        lpVarContent=lpBuf;
		sprintf(lpBuf, "%d", iErrorLevel);

	}  else if (!(stricmp(lpNameCpy, "=EXITCODEASCII"))) {

		lpVarContent=lpBuf;
#if defined(DOS9_USE_LIBCU8)
        snprintf(lpBuf, sizeof(iErrorLevel)+1, "%s", &iErrorLevel);
#else
        snprintf(lpBuf, 2, "%s", &iErrorLevel);
#endif /* DOS9_USE_LIBCU8 */


	} else if (!(stricmp(lpNameCpy, "RANDOM"))) {

		/* requested RANDOM */
		lpVarContent=lpBuf;
		sprintf(lpBuf, "%d", rand());

	} else if (!(stricmp(lpNameCpy, "DATE"))
                || !(stricmp(lpNameCpy, "TIME"))) {

		lpVarContent=lpBuf;
		Dos9_GetTimeBasedVar(*lpNameCpy, lpBuf, sizeof(lpBuf));

	} else if (!(lpVarContent=Dos9_GetEnv(lpeEnv, lpNameCpy))) {

        if (iVarState == 3) {
            Dos9_EsCpy(lpRecieve, lpToken);
            free(lpNameCpy);
            return TRUE;
        }

		free(lpNameCpy);
		return FALSE;

	}

#ifdef DOS9_USE_LIBCU8
    /* prevent strings from being cut in the middle of an u8 character */
    iTotalLen = Dos9_CountU8Chars(lpVarContent);
#else
    iTotalLen=strlen(lpVarContent);
#endif // DOS9_USE_LIBCU8

	if (iVarState==2) {

		if (iBegin < 0) {

            if (iLen > 0) {

                if ( (iTotalLen+iBegin) >= iLen)
                    iBegin = iTotalLen + iBegin;

            } else {

                if ( iBegin <= iLen )
                    iBegin = iTotalLen + iBegin;

            }

        }

        if (iBegin < 0 || iBegin > iTotalLen) {


			/* skip because these values are not valid
			    indeed iBegin must not be negative and
			    must not overflow the buffer */

			    lpVarContent = "";

		} else if (iLen > 0) {

			if ((iBegin+iLen)<= iTotalLen) {
				/* if the strings is right */

#ifdef DOS9_USE_LIBCU8
                lpZeroPos = Dos9_SkipU8Chars(lpVarContent, iBegin+iLen);
                lpVarContent = Dos9_SkipU8Chars(lpVarContent, iBegin);
#else
                lpZeroPos=lpVarContent+iBegin+iLen;
                lpVarContent+=iBegin;
#endif /* DOS9_USE_LIBCU8 */
				cCharSave=*lpZeroPos;
				*lpZeroPos='\0';

			} else
                lpVarContent = "";


		} else if (iLen <= 0) {

			if (abs(iLen) <= iTotalLen-iBegin) {

				/* if the string is right too
				   but the lenght given is negative, such as -3
				   (ie. truncate 3 characters before the end of
				   the string)
				*/

#ifdef DOS9_USE_LIBCU8
                lpZeroPos=Dos9_SkipU8Chars(lpVarContent, iTotalLen+iLen);
                lpVarContent =Dos9_SkipU8Chars(lpVarContent, iBegin);
#else
				lpZeroPos = lpVarContent+iTotalLen+iLen;
				lpVarContent+=iBegin;
#endif /* DOS9_USE_LIBCU8 */
				cCharSave=*lpZeroPos;
				*lpZeroPos='\0';


			} else
                lpVarContent = "";

		}
	}

	Dos9_EsCpy(lpRecieve, lpVarContent);

	if (iVarState==1) {

		/* FIXME : This should be case insensitive */
		Dos9_EsReplace(lpRecieve, lpToken, lpNextToken);

	}

	if (iVarState==2 && lpZeroPos != NULL) {
		*lpZeroPos=cCharSave;
	}

	free(lpNameCpy);

	return TRUE;
}

char* Dos9_GetLocalVarPointer(LOCAL_VAR_BLOCK* lpvBlock, char cVarName)
{
	if (Dos9_TestLocalVarName(cVarName))
		return NULL;

	return lpvBlock[(int)cVarName];

}

void Dos9_SetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char cVarName, char* cVarContent)
{
    char* old;

	/* Perform test on value cName, to test its
	   specification conformance, i.e. the character must be
	   a strict ASCII character, excluding control characters
	   and space (code range from 0x00 to 0x20 included) */

	if (Dos9_TestLocalVarName(cVarName))
		return;

	/* Free the current content of the variable if it is
	   already allocated */

	old = lpvBlock[(int)cVarName];

	if (cVarContent) {

		if (!(lpvBlock[(int)cVarName]=strdup(cVarContent)))
            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                    __FILE__ "Dos9_SetLocalVar()",
                                    -1);

	} else {

		lpvBlock[(int)cVarName]=NULL;

	}

    /* Makes this more resilient by allowing cVarContent to point to the actual
       buffer that holds the variable already*/
	if (old)
        free(old);

	return;
}


char* Dos9_GetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char* lpName, ESTR* lpRecieve)
{
	char *lpPos, *lpNext;
	char lpDrive[_MAX_DRIVE], lpDir[_MAX_DIR], lpFileName[_MAX_FNAME], lpExt[_MAX_EXT],
         lpFullPath[FILENAME_MAX];
	char cFlag[DOS9_VAR_MAX_OPTION+1]= {DOS9_ALL_PATH};
	char lpBuffer[FILENAME_MAX];
	char bSeekFile=FALSE, bSplitPath=FALSE;
	char cVarName,
	     cValidName=TRUE;

	struct tm lTime;
	struct stat stFileInfo;

	int i=0;

    *Dos9_EsToChar(lpRecieve) = '\0';

	if (*lpName!='~') {

		/* this is a conventionnal special variable */

		if (Dos9_TestLocalVarName(*lpName))
			return NULL;

		if (!lpvBlock[(int)*lpName])
			return NULL;

		Dos9_EsCpy(lpRecieve, lpvBlock[(int)*lpName]);

		return lpName+1;
	}

	lpName++;

	/* this is an extended special variable */

	if (!*lpName) return NULL;

	for (; *(lpName) && strchr("dnpxztaf", *(lpName)) && i<DOS9_VAR_MAX_OPTION; lpName++) {

		switch(*lpName) {

		case 'f':
            cFlag[i]='f';
            i++;
            bSplitPath=TRUE;
            break;

		case 'd':

			cFlag[i]='d';
			i++;
			bSplitPath=TRUE;
			break;

		case 'n':

			cFlag[i]='n';
			i++;
			bSplitPath=TRUE;
			break;

		case 'p':

			cFlag[i]='p';
			i++;
			bSplitPath=TRUE;
			break;

		case 'x':

			cFlag[i]='x';
			i++;
			bSplitPath=TRUE;
			break;

		case 'z':

			cFlag[i]='z';
			i++;
			bSeekFile=TRUE;
			break;

		case 't':

			cFlag[i]='t';
			i++;
			bSeekFile=TRUE;
			break;

		case 'a':

			cFlag[i]='a';
			i++;
			bSeekFile=TRUE;
			break;

		}
	}


	if ((*lpName & 0x80) || (*lpName <= 0x20)) {

		/* the varname is not valid */
		cValidName=FALSE;

	} else if (!lpvBlock[(int)*lpName]) {

		cValidName=FALSE;

	}

	if (!cValidName) {

		cVarName=0;

		/* if not, make a descending test */
		while (i>0) {

			i--;
			lpName--;

			if (lpvBlock[(int)cFlag[i]]) {

				/* the flag are all valid varnames so backtrack
				   we always take the longest match */
				cVarName=cFlag[i];

                if (i == 0)
                    cFlag[0] = DOS9_ALL_PATH;
                else
                    cFlag[i] = 0; /* end the flag list */

				break;

			}

		}

		if (cVarName==0)
			return NULL;

	} else {

		/* if the var is defined */
		cVarName=*lpName;

	}

    /* Copy the content of the varialble */
	Dos9_EsCpy(lpRecieve, lpvBlock[(int)cVarName]);
	lpPos=Dos9_EsToChar(lpRecieve);

    /* Remove any quote surrounding the argument*/
	if (*lpPos=='"' || *lpPos=='\'') {

		if ((lpNext=strrchr(lpPos, *lpPos)))
			*lpNext='\0';

		while (*(lpPos+1)) {

			*(lpPos)=*(lpPos+1);
			lpPos++;

		}

		*lpPos='\0';

		lpPos=Dos9_EsToChar(lpRecieve);
	}

	if (bSeekFile) {

	    if (cVarName == '0')
            Dos9_EsCpy(lpRecieve, ifIn.lpFileName);

        lpPos=Dos9_EsToFullPath(lpRecieve);
		stat(lpPos, &stFileInfo);

#if defined WIN32
		stFileInfo.st_mode=GetFileAttributes(lpPos);
#endif
	}

	if (bSplitPath) {

        if (cVarName == '0')
            Dos9_EsCpy(lpRecieve, ifIn.lpFileName);

        lpPos=Dos9_EsToFullPath(lpRecieve);

		Dos9_SplitPath(lpPos, lpDrive, lpDir, lpFileName, lpExt);
		snprintf(lpFullPath, sizeof(lpFullPath), "%s", lpPos);

	}

	if (cFlag[0]!=DOS9_ALL_PATH) {

		*Dos9_EsToChar(lpRecieve) = '\0';

		for (i=0; cFlag[i]!=0; i++) {
			switch (cFlag[i]) {

			case 'f':
                Dos9_EsCat(lpRecieve, lpFullPath);
				if (cFlag[i+1]!=0) Dos9_EsCat(lpRecieve, "\t");
				break;


			case 'd':
				Dos9_EsCat(lpRecieve, lpDrive);

				if (cFlag[i+1] == 'p')
                    Dos9_EsCat(lpRecieve, "/");
				else if (cFlag[i+1]!=0)
					Dos9_EsCat(lpRecieve, "\t");

				break;

			case 'p':
				Dos9_EsCat(lpRecieve, lpDir);

				if (cFlag[i+1]!=0 && cFlag[i+1]!='n')
					Dos9_EsCat(lpRecieve, "\t");

				break;

			case 'n':
				Dos9_EsCat(lpRecieve, lpFileName);

				if (cFlag[i+1]!=0 && cFlag[i+1]!='x')
					Dos9_EsCat(lpRecieve, "\t");

				break;

			case 'x':
				Dos9_EsCat(lpRecieve, lpExt);
				if (cFlag[i+1]!=0) Dos9_EsCat(lpRecieve, "\t");
				break;

			case 'z':
				sprintf(lpBuffer, "%d%c", (int)stFileInfo.st_size, (cFlag[i+1]!=0 ? '\t' : '\0'));
				Dos9_EsCat(lpRecieve, lpBuffer);
				break;

			case 't':
				localtime_r(&stFileInfo.st_atime, &lTime);

				strftime(lpBuffer, sizeof(lpBuffer), "%x %X", &lTime);

				Dos9_EsCat(lpRecieve, lpBuffer);
				if (cFlag[i+1]!=0) Dos9_EsCat(lpRecieve, "\t");
				break;

			case 'a':
				sprintf(lpBuffer, "       %c", (cFlag[i+1]!=0 ? '\t': '\0'));
				if (stFileInfo.st_mode & DOS9_FILE_DIR) lpBuffer[0]='D';
				if (stFileInfo.st_mode & DOS9_FILE_READONLY) lpBuffer[2]='R';
				if (stFileInfo.st_mode & DOS9_FILE_HIDDEN) lpBuffer[3]='H';
				if (stFileInfo.st_mode & DOS9_FILE_SYSTEM) lpBuffer[4]='S';
				if (stFileInfo.st_mode & DOS9_FILE_ARCHIVE) lpBuffer[5]='A';
				Dos9_EsCat(lpRecieve, lpBuffer);
				break;
			}
		}
	}

	return lpName+1;
}

LOCAL_VAR_BLOCK* Dos9_DuplicateLocalVar(LOCAL_VAR_BLOCK* block)
{
    LOCAL_VAR_BLOCK* new;
    int i = 0;

    if ((new = Dos9_GetLocalBlock()) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                              | DOS9_PRINT_C_ERROR,
                              __FILE__ "/Dos9_DuplicateLocalVar", -1);


    while (i < LOCAL_VAR_BLOCK_SIZE) {

        if (block[i]
            && (new[i] = strdup(block[i])) == NULL)
            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                              | DOS9_PRINT_C_ERROR,
                              __FILE__ "/Dos9_DuplicateLocalVar", -1);


        i++;
    }

    return new;
}

void Dos9_FreeLocalBlock(LOCAL_VAR_BLOCK* local)
{
    int i;

    for (i = 0x21; i < 0x80; i++)
        if (local[i])
            free(local[i]);

    free(local);
}
