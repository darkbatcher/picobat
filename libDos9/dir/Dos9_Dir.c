/*
 *
 *   libDos9 - The Dos9 project
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

#include <ctype.h>
#include "../libDos9-int.h"

LIBDOS9 int Dos9_RegExpMatch(char* lpRegExp, char* lpMatch)
{
	char* lpNextPos;

	if (!(lpRegExp && lpMatch)) return FALSE;

	for (; *lpRegExp && *lpMatch;
	     lpRegExp=Dos9_GetNextChar(lpRegExp),
	     lpMatch=Dos9_GetNextChar(lpMatch)
	    ) {
		switch(*lpRegExp) {
		case '?':

			break;

		case '*':

			lpRegExp=Dos9_GetNextChar(lpRegExp);

			while (*lpRegExp=='*' && *lpRegExp=='?') lpRegExp=Dos9_GetNextChar(lpRegExp);

			if (!*lpRegExp) return TRUE;


			if ((lpNextPos=_Dos9_SeekPatterns(lpMatch,lpRegExp))) lpMatch=lpNextPos; /* search if matching patterns exist*/
			else return FALSE;

			if (!*lpMatch) return TRUE; /* if there is no match string to test, the match is true */

			if (*lpRegExp) while (*lpRegExp!='*' && *lpRegExp!='?' && *lpRegExp) lpRegExp=Dos9_GetNextChar(lpRegExp);
			/* skip the next symbols, i.e. two '**' or '*?' are useless and so ignored */
			lpRegExp--;
			break;

		default:
			if (*lpRegExp != *lpMatch) return FALSE;
		}
	}

	if (*lpRegExp) lpRegExp=Dos9_GetNextChar(lpRegExp);
	if (!(!*lpMatch && !*lpRegExp)) return FALSE;
	return TRUE;
}

LIBDOS9 int Dos9_RegExpCaseMatch(char* lpRegExp, char* lpMatch)
{
	char* lpNextPos;

	if (!(lpRegExp && lpMatch)) return FALSE;

	for (; *lpRegExp && *lpMatch;
	     lpRegExp=Dos9_GetNextChar(lpRegExp),
	     lpMatch=Dos9_GetNextChar(lpMatch)
	    ) {
		switch(*lpRegExp) {
		case '?':

			break;

		case '*':

			lpRegExp=Dos9_GetNextChar(lpRegExp);

			while (*lpRegExp=='*' && *lpRegExp=='?') lpRegExp=Dos9_GetNextChar(lpRegExp);

			if (!*lpRegExp) return TRUE;

			if ((lpNextPos=_Dos9_SeekCasePatterns(lpMatch,lpRegExp))) lpMatch=lpNextPos; /* search if matching patterns exist*/
			else return FALSE;

			if (!*lpMatch) return TRUE; /* if there is no match string to test, the match is true */

			if (*lpRegExp) while (*lpRegExp!='*' && *lpRegExp!='?' && *lpRegExp) lpRegExp=Dos9_GetNextChar(lpRegExp);
			/* skip the next symbols, i.e. two '**' or '*?' are useless and so ignored */
			lpRegExp--;
			break;

		default:
			if (toupper(*lpRegExp) != toupper(*lpMatch)) return FALSE;
		}
	}

	if (*lpRegExp) lpRegExp=Dos9_GetNextChar(lpRegExp);
	if (!(!*lpMatch && !*lpRegExp)) return FALSE;
	return TRUE;
}


LIBDOS9 int Dos9_FormatFileSize(char* lpBuf, int iLength, unsigned int iFileSize)
{
	int i=0, iLastPart=0;
	if (!iFileSize) {
		return (int)strncpy(lpBuf, "", iLength);
	}
	char* lpUnit[]= {"o", "ko", "mo", "go"};
	while (iFileSize>=1000) {
		iLastPart=iFileSize % 1000;
		iFileSize/=1000;
		i++;
	}
	if (iFileSize>=100) return snprintf(lpBuf, iLength, " %d %s", iFileSize, lpUnit[i%4]);
	if (iFileSize>=10) return snprintf(lpBuf, iLength, "  %d %s", iFileSize, lpUnit[i%4]);
	else {
		iLastPart=iLastPart/10;
		return snprintf(lpBuf, iLength, "%d,%.2d %s", iFileSize, iLastPart , lpUnit[i%4]);
	}
}

LIBDOS9 int         Dos9_GetMatchFileCallback(char* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*))
{
	int iDepth; /* to store depth of  regexp */
	char lpStaticPart[FILENAME_MAX],
	     lpMatchPart[FILENAME_MAX];

	int iFileDescriptors[2];
	size_t iReturn;

	THREAD hThread;

	FILEPARAMETER fpParam= {TRUE, /* include informations on the
                                    FILEPARAMETER Structures returned */
	                        0,     /* defines the descriptor through which
                                    the callback funtion will read */

	                        pCallBack /* the callback routine in order to benefit from
                                        from the use of two thread. (this is somewhat
                                        like asynchronious version of the next function
                                     */
	                       };

	if (iFlag & DOS9_SEARCH_NO_STAT) {
		fpParam.bStat=FALSE;
	}

	_Dos9_SplitMatchPath(lpPathMatch, lpStaticPart, FILENAME_MAX, lpMatchPart, FILENAME_MAX);
	/* Split the string in two blocks :
	    - lpStaticPart : Part that does not include any
	      regexp symbol.

	    - lpMatchPart : Part that include regexp symbols

	*/

	iDepth=_Dos9_GetMatchDepth(lpMatchPart);

	/* Set pipe arguments */
	if (_Dos9_Pipe(iFileDescriptors, 1024, O_BINARY)==-1) return 0;
	fpParam.iInput=iFileDescriptors[0];

	/* start the thread that gather answers */
	Dos9_BeginThread(&hThread,
	                 (void(*)(void*))_Dos9_WaitForFileListCallBack,
	                 0,
	                 (void*)(&fpParam)
	                );

	if (!*lpStaticPart && *lpMatchPart) {

		/* add the current directory to the default
		   static path
		*/

		*lpStaticPart='.';
		lpStaticPart[1]='\0';

	}

	if (*lpStaticPart && !*lpMatchPart) {

		if (Dos9_DirExists(lpStaticPart) && (iFlag & DOS9_SEARCH_DIR_MODE)) {

			/* if the regexp is trivial but corresponds to a directory and
			   if the dir-compliant mode has been set, then the regexp will
			   browse the directory */

			*lpMatchPart='*';
			lpMatchPart[1]='\0';

		} else if (Dos9_DirExists(lpStaticPart) || Dos9_FileExists(lpStaticPart)) {

			/* if the regexp is trivial (ie. no regexp characters)
			   then perform direct test and add it if go to end */

			write(iFileDescriptors[1], lpStaticPart, FILENAME_MAX);
			goto Dos9_GetMatchFileList_End;

		}

	}


	/* Start regexp-based file research */
	_Dos9_SeekFiles(lpStaticPart,
	                lpMatchPart,
	                1,
	                iDepth,
	                iFileDescriptors[1],
	                iFlag);

Dos9_GetMatchFileList_End:


	if (write(iFileDescriptors[1], "\1", 1)==-1)
		return 0;

	Dos9_WaitForThread(&hThread, &iReturn);

	close(iFileDescriptors[0]);
	close(iFileDescriptors[1]);

	return iReturn;

}

LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag)
{
	int iDepth; /* to store depth of  regexp */
	char lpStaticPart[FILENAME_MAX],
	     lpMatchPart[FILENAME_MAX];
	int iFileDescriptors[2];
	THREAD hThread;
	LPFILELIST lpReturn;

	FILEPARAMETER fpParam= {TRUE, /* include informations on the
                                    FILEPARAMETER Structures returned */
	                        0,    /* defines the descriptor through which
                                    the callback funtion will read */
	                        NULL  /* no callback */
	                       };

    if (strlen(lpPathMatch) > FILENAME_MAX-1)
        return NULL;

	if (iFlag & DOS9_SEARCH_NO_STAT) {
		fpParam.bStat=FALSE;
	}

	_Dos9_SplitMatchPath(lpPathMatch, lpStaticPart, FILENAME_MAX, lpMatchPart, FILENAME_MAX);
	/* Split the string in two blocks :
	    - lpStaticPart : Part that does not include any
	      regexp symbol.

	    - lpMatchPart : Part that include regexp symbols

	*/

	iDepth=_Dos9_GetMatchDepth(lpMatchPart);

	/* Set pipe arguments */
	if (_Dos9_Pipe(iFileDescriptors, 1024, O_BINARY)==-1) return NULL;
	fpParam.iInput=iFileDescriptors[0];


	/* start the thread that gather answers */
	Dos9_BeginThread(&hThread,
	                 (void(*)(void*))_Dos9_WaitForFileList,
	                 0,
	                 (void*)(&fpParam)
	                );

	if (!*lpStaticPart && *lpMatchPart) {

		/* add the current directory to the default
		   static path
		*/

		*lpStaticPart='.';
		lpStaticPart[1]='\0';

	}

	if (*lpStaticPart && !*lpMatchPart) {

		if (Dos9_DirExists(lpStaticPart) && (iFlag & DOS9_SEARCH_DIR_MODE)) {

			/* if the regexp is trivial but corresponds to a directory and
			   if the dir-compliant mode has been set, then the regexp will
			   browse the directory */

			*lpMatchPart='*';
			lpMatchPart[1]='\0';

		} else if (Dos9_DirExists(lpStaticPart) || Dos9_FileExists(lpStaticPart)) {

			/* if the regexp is trivial (ie. no regexp characters)
			   then perform direct test and add it if go to end */

			write(iFileDescriptors[1], lpStaticPart, FILENAME_MAX);
			goto Dos9_GetMatchFileList_End;

		}

	}

	/* Start regexp-based file research */
	_Dos9_SeekFiles(lpStaticPart,
	                lpMatchPart,
	                1,
	                iDepth,
	                iFileDescriptors[1],
	                iFlag);

Dos9_GetMatchFileList_End:

	if (write(iFileDescriptors[1], "\1", 1)==-1) return NULL;

	Dos9_WaitForThread(&hThread, &lpReturn);

	close(iFileDescriptors[0]);
	close(iFileDescriptors[1]);

	return lpReturn;

}

LPFILELIST _Dos9_SeekFiles(char* lpDir, char* lpRegExp, int iLvl, int iMaxLvl, int iOutDescriptor, int iSearchFlag)
{
	DIR* pDir;
	/* used to browse the directory */
	struct dirent* lpDirElement;
	/* used to browse the directory elements */
	char lpRegExpLvl[FILENAME_MAX];
	/* used to get the part of the path to deal with */
	char lpFilePath[FILENAME_MAX];
	/* used to make file path */
	int iFlagRecursive=FALSE,
	    iIsPseudoDir;

	if (iSearchFlag & DOS9_SEARCH_RECURSIVE) {
		iFlagRecursive=(iLvl == iMaxLvl);
		/*  verfify wether the research on the path is ended (i.e. the top level at least been reached) */
	}

	_Dos9_GetMatchPart(lpRegExp, lpRegExpLvl, FILENAME_MAX, iLvl);
	/* returns the part of path that must the file must match */

	if ((!strcmp(lpRegExpLvl, ".")) && (iLvl == iMaxLvl)) {

		*lpRegExpLvl='\0';
		/*
		    If the regexp is trivial (ie. '.') and at top level
		    (including for recursive search), this is replaced
		    by the equivalent symbol '' (all match accepted)

		 */

	}

	if ((pDir=opendir(lpDir))) {

		while ((lpDirElement=readdir(pDir))) {


			if ((
			        !iFlagRecursive
			        && Dos9_RegExpCaseMatch(lpRegExpLvl, lpDirElement->d_name)

			        /* if the search is non-recursive (or simply not recursive yet),
			           and the file name matches up with the level of regexp */

			    ) || (

			        iFlagRecursive

			        /*
			            if the search is recursive don't care wether the element
			            matches the regular expression. Just check that the
			            element is neither '.' nor '..', since it will probably
			            circular filesystem search.
			        */
			    )) {

				_Dos9_MakePath(lpDir, lpDirElement->d_name, lpFilePath, FILENAME_MAX);

				iIsPseudoDir=!(strcmp(".", lpDirElement->d_name)
				               && strcmp("..", lpDirElement->d_name));

				switch(iFlagRecursive) {

				case TRUE:
					/* If the mode is reccursive:

					    - try to add the element, if the element matches the
					      regexp.

					    - try to browse subdirectories.

					 */

					if (Dos9_RegExpCaseMatch(lpRegExpLvl, lpDirElement->d_name)) {

						if ((iSearchFlag & DOS9_SEARCH_NO_PSEUDO_DIR)
						    && iIsPseudoDir)
							goto Dos9_DirRecursive;

						if (write(iOutDescriptor, lpFilePath, FILENAME_MAX)==-1)
							return NULL;

						if (iSearchFlag & DOS9_SEARCH_GET_FIRST_MATCH)
							return NULL;

					}

Dos9_DirRecursive:

					if (!iIsPseudoDir && Dos9_DirExists(lpFilePath))
						_Dos9_SeekFiles(lpFilePath,
						                lpRegExp,
						                iLvl,
						                iMaxLvl,
						                iOutDescriptor,
						                iSearchFlag);

					break;

				case FALSE:
					/* If the mode is not recussive */

					if (iLvl == iMaxLvl) {

						if ((iSearchFlag & DOS9_SEARCH_NO_PSEUDO_DIR)
						    && iIsPseudoDir)
							break;


						if (write(iOutDescriptor, lpFilePath, FILENAME_MAX)==-1)
							return NULL;

						if (iSearchFlag & DOS9_SEARCH_GET_FIRST_MATCH)
							return NULL;



					} else if (Dos9_DirExists(lpFilePath)) {

						_Dos9_SeekFiles(lpFilePath, lpRegExp, iLvl+1, iMaxLvl, iOutDescriptor, iSearchFlag);

					}

				}
			}
		}

		closedir(pDir);
	}
	return NULL;
}

LIBDOS9 int Dos9_GetStaticPart(const char* lpPathMatch, char* lpStaticPart, size_t size)
{
    char garbage;

    return _Dos9_SplitMatchPath(lpPathMatch, lpStaticPart, size, &garbage, sizeof(garbage));
}

LIBDOS9 size_t Dos9_GetStaticLength(const char* str)
{
    char *ptr = str,
         *orig = str;

    while (*str) {

        if (*str == '\\' || *str == '/') {

            ptr = str;

        } else if (*str == '*' || *str == '?') {

            break;

        }

        str ++;

    }

    return (size_t)(ptr-orig);
}

int _Dos9_SplitMatchPath(const char* lpPathMatch, char* lpStaticPart, size_t iStaticSize,  char* lpMatchPart, size_t iMatchSize)
{
	const char* lpLastToken=NULL; /* a pointer to the last static delimiter */
	const char* lpCh=lpPathMatch; /* a pointer to browse string */
	int iContinue=TRUE;
	size_t iSize;

	while (*lpCh && iContinue) {

		/*
		    detect the part that is static (that does not contains
		    any matching characters
		 */

		switch(*lpCh) {

		case '/':
		case '\\':
			lpLastToken=lpCh+1;
			break;

		case '?':
		case '*':
			iContinue=FALSE;
			break;

		}

		lpCh++;

	}

	if (lpLastToken && !iContinue) {

		/*
		    if match part and static part are different
		*/

		iSize=lpLastToken-lpPathMatch;


		if ((iSize==1)) {

			/* the token is just '/' which means
			   volume root */
			printf("Incrementing Size");
			iSize++;

		} else if ((iSize==3) && (*(lpPathMatch+1)==':'))  {

			/* the token is a windows drive letter */
			printf("Incrementing Size");
			iSize++;

		}

		if (iSize < iStaticSize)
			iStaticSize=iSize;

		strncpy(lpStaticPart, lpPathMatch, iStaticSize);
		lpStaticPart[iStaticSize-1]='\0';

		strncpy(lpMatchPart, lpLastToken, iMatchSize);
		lpStaticPart[iMatchSize-1]='\0';

	} else if (!iContinue) {

		/* if there is no static part */

		if (iStaticSize)
			*lpStaticPart='\0';

		strncpy(lpMatchPart, lpPathMatch, iMatchSize);
		lpStaticPart[iMatchSize-1]='\0';

	} else {

		/* if there is no match part */

		if (iMatchSize)
			*lpMatchPart='\0';

		strncpy(lpStaticPart, lpPathMatch, iStaticSize);
		lpStaticPart[iStaticSize-1]='\0';

	}

	return 0;
}

int _Dos9_GetMatchPart(const char* lpRegExp, char* lpBuffer, size_t iLength, int iLvl)
{
	const char *lpCh=lpRegExp,
	            *lpLastToken=lpRegExp;
	int iCurrentLvl=1, /* the first level is the level 1 */
	    iContinue=TRUE;

	size_t iSize;

	while (*lpCh && iContinue) {

		if (*lpCh=='\\' || *lpCh=='/') {

			if (iCurrentLvl==iLvl)
				iContinue=FALSE;

			iCurrentLvl++;
			lpLastToken=lpCh+1;

		}

		lpCh++;

	}

	if (!*lpCh) {

		/* if we are at string end */

		if (iLvl!=iCurrentLvl) {

			*lpBuffer='\0';

		} else {

			strncpy(lpBuffer, lpLastToken, iLength);
			lpBuffer[iLength-1]='\0';

		}

		return 0;

	}

	iSize=lpCh-lpLastToken+1;

	if (iSize < iLength)
		iLength=iSize;

	strncpy(lpBuffer, lpLastToken, iLength);
	lpBuffer[iSize-1]='\0';

	return 0;
}

int _Dos9_GetMatchDepth(char* lpRegExp)
{
	int iDepth;

	for (iDepth=1; *lpRegExp; lpRegExp++) {

		if (*lpRegExp=='\\' || *lpRegExp=='/') iDepth++;

	}

	return iDepth;

}

int _Dos9_MakePath(char* lpPathBase, char* lpPathEnd, char* lpBuffer, int iLength)
{
	int iLen=strlen(lpPathBase)+strlen(lpPathEnd);
	int i=0;

	if (lpPathBase[strlen(lpPathBase)-1] != '/' && lpPathBase[strlen(lpPathBase)-1] != '\\') {
		iLen++;
		i=1;
	}

	if (iLen>iLength) return -1;

	if (!strcmp(lpPathBase, ".")) {
		strcpy(lpBuffer,lpPathEnd);
		return 0;
	}

	strcpy(lpBuffer, lpPathBase);
	if (i) strcat(lpBuffer, "/");
	strcat(lpBuffer, lpPathEnd);
	return 0;
}

char* _Dos9_GetFileName(char* lpPath)
{
	char* lpLastPos=NULL;

	for (; *lpPath; lpPath++) {
		if (*lpPath=='/' || *lpPath=='\\') lpLastPos=lpPath+1;
	}

	return lpLastPos;
}

LIBDOS9 int Dos9_FreeFileList(LPFILELIST lpflFileList)
{

	_Dos9_FreeFileList(lpflFileList);

	return 0;

}

LPFILELIST _Dos9_WaitForFileList(LPFILEPARAMETER lpParam)
{
	char lpFileName[FILENAME_MAX];
	char cUseStat=lpParam->bStat;
	int iInDescriptor=lpParam->iInput;
	LPFILELIST lpflLast=NULL;
	LPFILELIST lpflCurrent=NULL;

	while (TRUE) {

		/* get data from the descriptor (take a large amount
		   of FILENAME_MAX bytes because of binary type of
		   the descriptor). */
		if (read(iInDescriptor, lpFileName, FILENAME_MAX)) {

			if (*lpFileName=='\1')
				break;

			if ((lpflCurrent=malloc(sizeof(FILELIST)))) {

				strcpy(lpflCurrent->lpFileName, lpFileName);
				lpflCurrent->lpflNext=lpflLast;

				if ((cUseStat)) {

					stat(lpFileName, &(lpflCurrent->stFileStats));

#if defined WIN32
					lpflCurrent->stFileStats.st_mode=Dos9_GetFileAttributes(lpFileName);
#endif

				}

				lpflLast=lpflCurrent;

			}
		}
	}

	Dos9_EndThread(lpflCurrent);
	return lpflCurrent;
}

int                 _Dos9_WaitForFileListCallBack(LPFILEPARAMETER lpParam)
{
	char lpFileName[FILENAME_MAX];
	char cUseStat=lpParam->bStat;
	int iInDescriptor=lpParam->iInput;
	size_t i=0;
	void(*pCallback)(FILELIST*)=lpParam->pCallBack;

	FILELIST flElement;

	while (TRUE) {

		/* get data from the descriptor (take a large amount
		   of FILENAME_MAX bytes because of binary type of
		   the descriptor). */
		if (read(iInDescriptor, lpFileName, FILENAME_MAX)) {

			if (*lpFileName=='\1')
				break;

			strcpy(flElement.lpFileName, lpFileName);

			if (cUseStat) {

				stat(lpFileName, &(flElement.stFileStats));

#ifdef WIN32

				/* Using windows, more detailled directory information
				   can be obtained */
				flElement.stFileStats.st_mode=Dos9_GetFileAttributes(lpFileName);

#endif // WIN32

			}

			/* use the callback function */
			pCallback(&flElement);

			i++;
		}

	}

	Dos9_EndThread((void*)i);
	return i;
}

int _Dos9_FreeFileList(LPFILELIST lpflFileList)
{
	LPFILELIST lpflNext;

	for (; lpflFileList; lpflFileList=lpflNext) {

		lpflNext=lpflFileList->lpflNext;
		free(lpflFileList);
	}

	return 0;
}

char* _Dos9_SeekCasePatterns(char* lpSearch, char* lpPattern)
{
	const char* lpPatternBegin=(const char*)lpPattern;
	char* lpLastBegin;
	while (TRUE) {
		for (; *lpSearch && (toupper(*lpPattern)!=toupper(*lpSearch)); lpSearch++);

		lpLastBegin=lpSearch;

		for (; *lpSearch && *lpPattern && *lpPattern!='*' && *lpPattern!='?' && (toupper(*lpPattern)==toupper(*lpSearch)); lpSearch++, lpPattern++);

		if (!*lpPattern || *lpPattern=='*' || *lpPattern=='?') {
			//printf("<FOUND> (returns: '%s')\n",lpLastBegin, lpSearch);
			return lpSearch;
		}

		if (!*lpSearch) {
			//printf("[WRONG] ('%s')\n", lpLastBegin);
			return NULL;
		}

		lpSearch=lpLastBegin+1;
		lpPattern=(char*)lpPatternBegin;
	}
}

char* _Dos9_SeekPatterns(char* lpSearch, char* lpPattern)
{
	const char* lpPatternBegin=(const char*)lpPattern;
	char* lpLastBegin;
	while (TRUE) {

		/* search the next character identical to the given character */
		for (; *lpSearch && (*lpPattern!=*lpSearch); lpSearch++) {
		}

		lpLastBegin=lpSearch;

		/* loop until the loop encounter a character '*' or '?'
		   (the next regexp symbol) */
		for (; *lpSearch && *lpPattern && *lpPattern!='*' && *lpPattern!='?' && (*lpPattern==*lpSearch); lpSearch++, lpPattern++) {
		}

		/* return if the pattern true is either finished or '*' or '?' */
		if (!*lpPattern || *lpPattern=='*' || *lpPattern=='?') {
			//printf("<FOUND> (returns: '%s')\n",lpLastBegin, lpSearch);
			return lpSearch;
		}

		/* return false if the loop browsed all the string without
		   valiable match */
		if (!*lpSearch) {
			//printf("[WRONG] ('%s')\n", lpLastBegin);
			return NULL;
		}

		lpSearch=lpLastBegin+1;
		lpPattern=(char*)lpPatternBegin;

	}
}
