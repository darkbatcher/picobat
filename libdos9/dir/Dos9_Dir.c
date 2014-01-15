#include <ctype.h>
#include "../LibDos9.h"

LIBDOS9 int Dos9_RegExpMatch(char* lpRegExp, char* lpMatch)
{
        if (!(lpRegExp && lpMatch)) return 0;

        for (;*lpRegExp && *lpMatch;
                                    lpRegExp=Dos9_GetNextChar(lpRegExp),
                                    lpMatch=Dos9_GetNextChar(lpMatch)
        ) {
            switch(*lpRegExp)
            {
                case '?':
                    break;
                case '*':
                    lpRegExp=Dos9_GetNextChar(lpRegExp);
                    if (!*lpRegExp) return TRUE;

                    for (;(*lpMatch) && (*lpRegExp != *lpMatch);lpRegExp=Dos9_GetNextChar(lpRegExp));

                    break;

                default:
                    if (*lpRegExp != *lpMatch) return FALSE;
            }
        }
        return TRUE;
}

LIBDOS9 int Dos9_RegExpCaseMatch(char* lpRegExp, char* lpMatch)
{
        char* lpNextPos;

        if (!strcmp(lpRegExp, ".")) return TRUE;
        if (!(lpRegExp && lpMatch)) return FALSE;

        for (;*lpRegExp && *lpMatch;
                                    lpRegExp=Dos9_GetNextChar(lpRegExp),
                                    lpMatch=Dos9_GetNextChar(lpMatch)
        ) {
            switch(*lpRegExp)
            {
                case '?':

                    break;

                case '*':

                    lpRegExp=Dos9_GetNextChar(lpRegExp);

                    if (!*lpRegExp) return TRUE;

                    if ((lpNextPos=_Dos9_SeekPatterns(lpMatch,lpRegExp))) lpMatch=lpNextPos; /* search if matching patterns exist*/
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
    char* lpUnit[]={"O", "Ko", "Mo", "Go"};
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

LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag)
{
    int iDepth, iBaseLvl; /* to store depth an the first level including regexp */
    int iThreadNb=1;
    char lpStaticPart[FILENAME_MAX];
    int iFileDescriptors[2];
    THREAD hThread;
    LPFILELIST lpReturn;

    FILEPARAMETER fpParam={TRUE, /* include informations on the FILEPARAMETER Structures returned */
                           0 /* defines the descriptor through which the callback funtion will read */
                           };

    if (iFlag & DOS9_SEARCH_NO_STAT) {
        fpParam.bStat=FALSE;
    }

    _Dos9_GetMatchInfo(lpPathMatch, lpStaticPart, FILENAME_MAX, &iDepth, &iBaseLvl);
        /* Get information about path matching */

    if (_Dos9_Pipe(iFileDescriptors, 1024, O_BINARY)==-1) return NULL;
    fpParam.iInput=iFileDescriptors[0];

    hThread=Dos9_BeginThread((void(*)(void*))_Dos9_WaitForFileList, 0, (void*)(&fpParam));
	/* start the thread that gather answers */

    _Dos9_SeekFiles(lpStaticPart, lpPathMatch, iBaseLvl, iDepth+iBaseLvl, &iThreadNb, iFileDescriptors[1], iFlag);
	/* start file research */

    if (write(iFileDescriptors[1], "\1", 1)==-1) return NULL;

    Dos9_WaitForThread(hThread);
    Dos9_GetThreadExitCode(hThread, &lpReturn);

    close(iFileDescriptors[0]);
    close(iFileDescriptors[1]);

    return lpReturn;
}

LPFILELIST _Dos9_SeekFiles(char* lpDir, char* lpRegExp, int iLvl, int iMaxLvl, int* iThreadNb, int iOutDescriptor, int iSearchFlag)
{
    DIR* pDir;
        /* used to browse the directory */
    struct dirent* lpDirElement;
        /* used to browse the directory elements */
    char lpRegExpLvl[FILENAME_MAX];
        /* used to get the part of the path to deal with */
    char lpFilePath[FILENAME_MAX];
        /* used to make file path */
    int iFlag=FALSE;

    if (iSearchFlag & DOS9_SEARCH_RECURSIVE) {
        iFlag=(iLvl == iMaxLvl);
            /*  verfify wether the research on the path is ended (i.e. the top level at least been reached) */
    }

    _Dos9_GetMatchPart(lpRegExp, lpRegExpLvl, FILENAME_MAX, iLvl);
        /* returns the part of path that must the file must match */

    if ((pDir=opendir(lpDir))) {
        while ((lpDirElement=readdir(pDir))) {

			if (iFlag) {

				_Dos9_MakePath(lpDir, lpDirElement->d_name, lpFilePath, FILENAME_MAX);

				/* if the DOS9_FILE_RECURSIVE has been checked and if we reached the top level */
		        if (strcmp(lpDirElement->d_name, ".") && strcmp(lpDirElement->d_name, "..")) {

					/* if the current dir element is neither '.' nor '..' then try to browse element's
					children */
					_Dos9_SeekFiles(lpFilePath, lpRegExp, iLvl, iMaxLvl, iThreadNb, iOutDescriptor, iSearchFlag);

				}

			}

            if (Dos9_RegExpCaseMatch(lpRegExpLvl, lpDirElement->d_name)) {

					_Dos9_MakePath(lpDir, lpDirElement->d_name, lpFilePath, FILENAME_MAX);

					if ((iLvl < iMaxLvl) && strcmp(lpDirElement->d_name, ".") && strcmp(lpDirElement->d_name, "..")) {

						/* if the top of the regexp has not been reached, then we have to browse
						the subfolder */
						_Dos9_SeekFiles(lpFilePath, lpRegExp, iLvl+1, iMaxLvl, iThreadNb, iOutDescriptor, iSearchFlag);

					} else if (iLvl == iMaxLvl) {

						/* else we just write its name in the pipe... */
						if ((!(iSearchFlag & DOS9_SEARCH_NO_CURRENT_DIR))
                              || (strcmp(lpDirElement->d_name, ".")
                                  && strcmp(lpDirElement->d_name, "..")) ) {

                            if (write(iOutDescriptor, lpFilePath, FILENAME_MAX)==-1) return NULL;

                            if (iSearchFlag & DOS9_SEARCH_GET_FIRST_MATCH) {
                                return NULL;
                            }
						}
					}
            }
        }

        closedir(pDir);
    }
    return NULL;
}


int _Dos9_GetMatchPart(char* lpRegExp, char* lpBuffer, int iLength, int iLvl)
{
    char* lpRegBegin=lpRegExp;
    int i=0;
    if ((*lpRegExp=='/' || *lpRegExp=='\\') && iLvl==1 ) {
        i=1;
        iLvl=0;
    }
    for (;*lpRegExp && iLvl;lpRegExp++ , i++) {
        switch(*lpRegExp) {
            case '/':
            case '\\':
                iLvl--;
                if (iLvl) {
                    i=-1;
                    lpRegBegin=lpRegExp+1;
                } else i--;
            default:;
        }
    }
    if (i) {
        if (i>=iLength) i=iLength-1;
        strncpy(lpBuffer, lpRegBegin, i);
        lpBuffer[i]='\0';
    } else {
        strncpy(lpBuffer, ".", iLength);
        lpBuffer[iLength-1]='\0';
    }
    return 0;
}

int _Dos9_GetMatchDepth(char* lpRegExp)
{
        int iDepth;
        for (iDepth=1;*lpRegExp;lpRegExp++) {
            if (*lpRegExp=='\\' || *lpRegExp=='/') iDepth++;
        }
        return iDepth;
}

int _Dos9_GetMatchInfo(char* lpRegExp, char* lpBuffer, int iLenght, int* lpDepth, int* lpBaseLvl)
{
    int iDepth=0, iContinue=TRUE, iLast=0, i=1, iBaseLvl=1;
    const char* lpRegBegin=lpRegExp;
    if (*lpRegExp=='\\' || *lpRegBegin=='/') {
        iLast=1;
        iDepth=1;
        lpRegExp++;
    }
    for (;*lpRegExp; lpRegExp++, i++) {
        switch(*lpRegExp) {
            case '/':
            case '\\':
                iDepth++;
                if (iContinue) {
                    iBaseLvl++;
                    iLast=i;
                }
                break;
            case '?':
            case '*':
                iContinue=FALSE;
            default:;
        }
    }
    if (iLast) {
        if (iLast>=iLenght) iLast=iLenght-1;
        strncpy(lpBuffer, lpRegBegin, iLast);
        lpBuffer[iLast]='\0';
        *lpDepth=iDepth;
        iDepth++;
        iBaseLvl++;
        *lpBaseLvl=iBaseLvl;
    } else {
        strncpy(lpBuffer, ".", iLenght);
        lpBuffer[iLenght]='\0';
        *lpDepth=iDepth;
        *lpBaseLvl=iBaseLvl;
    }

    return 0;
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
    for (;*lpPath;lpPath++) {
        if (*lpPath=='/' || *lpPath=='\\') lpLastPos=lpPath+1;
    }
    return lpLastPos;
}

char* _Dos9_SeekPatterns(char* lpSearch, char* lpPattern)
{
    const char* lpPatternBegin=(const char*)lpPattern;
    char* lpLastBegin;
    while (TRUE) {
        for (;*lpSearch && (toupper(*lpPattern)!=toupper(*lpSearch));lpSearch++) {
        }
        lpLastBegin=lpSearch;
        for (;*lpSearch && *lpPattern && *lpPattern!='*' && *lpPattern!='?' && (toupper(*lpPattern)==toupper(*lpSearch)); lpSearch++, lpPattern++) {
        }
        if (!*lpPattern || *lpPattern=='*' || *lpPattern=='?') {
            //printf("<FOUND> (returns: '%s')\n",lpLastBegin, lpSearch);
            return lpSearch;
        }
        if (!*lpSearch){
            //printf("[WRONG] ('%s')\n", lpLastBegin);
            return NULL;
        }
        lpSearch=lpLastBegin+1;
        lpPattern=(char*)lpPatternBegin;
    }
}

LIBDOS9 THREAD Dos9_FreeFileList(LPFILELIST lpflFileList)
{
	return Dos9_BeginThread((void(*)(void*))_Dos9_FreeFileList, 0, (void*)(lpflFileList));
}

LPFILELIST _Dos9_WaitForFileList(LPFILEPARAMETER lpParam)
{
    char lpFileName[FILENAME_MAX];
    char cUseStat=lpParam->bStat;
    int iInDescriptor=lpParam->iInput;
    LPFILELIST lpflLast=NULL;
    LPFILELIST lpflCurrent=NULL;
    while (TRUE) {
        if (read(iInDescriptor, lpFileName, FILENAME_MAX)) {
            if (*lpFileName=='\1') break;
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
    Dos9_EndThreadEx(lpflCurrent);
    return lpflCurrent;
}

int _Dos9_FreeFileList(LPFILELIST lpflFileList)
{
    LPFILELIST lpflNext;
    for (;lpflFileList;lpflFileList=lpflNext) {
        lpflNext=lpflFileList->lpflNext;
        free(lpflFileList);
    }
    Dos9_EndThread();
    return 0;
}
