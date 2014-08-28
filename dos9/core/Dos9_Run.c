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

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef _POSIX_C_SOURCE
#include <sys/wait.h>
#endif

#include "Dos9_Core.h"

//#define DOS9_DBG_MODE

#include "Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"


int Dos9_RunBatch(INPUT_FILE* pIn)
{
	ESTR* lpLine=Dos9_EsInit();

	INPUT_FILE pIndIn;

	char* const lpCurrentDir=Dos9_GetCurrentDir();

	char *lpCh,
	     *lpTmp;

	int res;

	while (!(pIn->bEof)) {

		DOS9_DBG("[*] %d : Parsing new line\n", __LINE__);


		if (*(pIn->lpFileName)=='\0'
		    && bEchoOn ) {

			/* this is a direct input */

			Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IGREEN | DOS9_GET_BACKGROUND(colColor));
			printf("\nDOS9 ");

			Dos9_SetConsoleTextColor(colColor);

			printf("%s>", lpCurrentDir);

		}

		/* the line we read was a void line */
		if (Dos9_GetLine(lpLine, pIn))
			continue;

		lpCh=Dos9_EsToChar(lpLine);

		while (*lpCh==' '
		       || *lpCh=='\t'
		       || *lpCh==';')
			lpCh++;

		if (*(pIn->lpFileName)!='\0'
		    && bEchoOn
		    && *lpCh!='@') {

			Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IGREEN | DOS9_GET_BACKGROUND(colColor));
			printf("\nDOS9 ");
			Dos9_SetConsoleTextColor(colColor);

			printf("%s>%s", lpCurrentDir, Dos9_EsToChar(lpLine));

		}

		Dos9_ReplaceVars(lpLine);

		bAbortCommand=FALSE;

		Dos9_RunLine(lpLine);

		if (bAbortCommand == -1)
			break;

		DOS9_DBG("\t[*] Line run.\n");

	}

	DOS9_DBG("*** Input ends here  ***\n");

	return 0;

}

int Dos9_ExecOperators(PARSED_STREAM* lppsStream)
{

	lppsStreamStack=Dos9_Pipe(lppsStreamStack);

	if (lppsStream==NULL)
		return 0;

	//fprintf(stderr, "Stream Status: cNodeType=%d\n", lppsStream->cNodeType);
	//Dos9_DumpStreamStack(lppsStreamStack);

	if (lppsStream->lppsNode != NULL) {

		if (lppsStream->lppsNode->cNodeType
		    == PARSED_STREAM_NODE_PIPE)
			Dos9_OpenPipe(lppsStreamStack);
	}

	//Dos9_DumpStreamStack(lppsStreamStack);

	//while (getchar()!='\n');

	switch (lppsStream->cNodeType) {

	case PARSED_STREAM_NODE_PIPE:
	case PARSED_STREAM_NODE_NONE :
		/* this condition is alwais true */
		return TRUE;

	case PARSED_STREAM_NODE_NOT :
		/* this condition is true when the instruction
		   before failed */
		return iErrorLevel;

	case PARSED_STREAM_NODE_YES:
		return !iErrorLevel;

	}

	return FALSE;

}

int Dos9_ExecOutput(PARSED_STREAM_START* lppssStart)
{

	DOS9_DBG("lppssStart->lpInputFile=%s\n"
	         "          ->lpOutputFile=%s\n"
	         "          ->cOutputMode=%d\n"
	         "lppssStart->cOutputMode & ~PARSED_STREAM_START_MODE_TRUNCATE=%d\n"
	         "lppssStart->cOutputMode & PARSED_STREAM_START_MODE_TRUNCATE=%d\n"
	         "STDOUT_FILENO=%d\n",
	         lppssStart->lpInputFile,
	         lppssStart->lpOutputFile,
	         lppssStart->cOutputMode,
	         lppssStart->cOutputMode & ~PARSED_STREAM_START_MODE_TRUNCATE,
	         lppssStart->cOutputMode & PARSED_STREAM_START_MODE_TRUNCATE,
	         STDOUT_FILENO
	        );

	if (!(lppssStart->lpInputFile)
	    && !(lppssStart->lpOutputFile)) {

		/* nothing to be done, just return, now */
		return 0;

	}



	/* open the redirections */

	lppsStreamStack=Dos9_PushStreamStack(lppsStreamStack);

	if (lppssStart->cOutputMode
	    && lppssStart->lpOutputFile )
		Dos9_OpenOutput(lppsStreamStack,
		                lppssStart->lpOutputFile,
		                lppssStart->cOutputMode & ~PARSED_STREAM_START_MODE_TRUNCATE,
		                lppssStart->cOutputMode & PARSED_STREAM_START_MODE_TRUNCATE
		               );


	if (lppssStart->lpInputFile)
		Dos9_OpenOutput(lppsStreamStack,
		                lppssStart->lpInputFile,
		                DOS9_STDIN,
		                0
		               );

	return 0;
}

int Dos9_RunLine(ESTR* lpLine)
{
	PARSED_STREAM_START* lppssStreamStart;
	PARSED_STREAM* lppsStream;

#ifdef DOS9_DBG_MODE
	STREAMSTACK* lpStack;
#endif

	Dos9_RmTrailingNl(Dos9_EsToChar(lpLine));

	lppssStreamStart=Dos9_ParseLine(lpLine);

	if (!lppssStreamStart) {
		DOS9_DBG("!!! Can't parse line : \"%s\".\n", strerror(errno));
		return -1;
	}

	Dos9_ExecOutput(lppssStreamStart);

	DOS9_DBG("\t[*] Global streams set.\n");

	lppsStream=lppssStreamStart->lppsStream;

	do {

		if (Dos9_ExecOperators(lppsStream)==FALSE || bAbortCommand)
			break;

		Dos9_RunCommand(lppsStream->lpCmdLine);

	} while ((lppsStream=lppsStream->lppsNode));

	/* deal with the remaining pipe operator */
	Dos9_ExecOperators(NULL);

	lppsStreamStack=Dos9_PopStreamStack(lppsStreamStack);

	Dos9_FreeLine(lppssStreamStart);

	DOS9_DBG("\t[*] Line run.\n");

	DOS9_DBG("*** Input ends here  ***\n");

	return 0;
}

int Dos9_RunCommand(ESTR* lpCommand)
{

	int (*lpProc)(char*);
	char lpErrorlevel[sizeof("-3000000000")],
         lpTmpLine[]="CD X:";
	static int lastErrorLevel=0;
	char *lpCmdLine;
	int iFlag;

RestartSearch:

	lpCmdLine=Dos9_EsToChar(lpCommand);

	lpCmdLine=Dos9_SkipAllBlanks(lpCmdLine);

#if defined(WIN32)

    /* handle "A:" */

	if (*lpCmdLine && *(lpCmdLine+1)==':'
	    && *Dos9_SkipAllBlanks(lpCmdLine+2) == '\0') {

        lpTmpLine[3] = *lpCmdLine;
        lpCmdLine = lpTmpLine;

    }

#endif

	switch((iFlag=Dos9_GetCommandProc(lpCmdLine, lpclCommands, (void**)&lpProc))) {

	case -1:
BackTrackExternalCommand:

		iErrorLevel=Dos9_RunExternalCommand(lpCmdLine);
		break;

	default:
		if (iFlag
		    && lpCmdLine[iFlag & ~DOS9_ALIAS_FLAG]!=' '
		    && lpCmdLine[iFlag & ~DOS9_ALIAS_FLAG]!='\t'
		    && lpCmdLine[iFlag & ~DOS9_ALIAS_FLAG]!='\0')
			goto BackTrackExternalCommand;

		if (iFlag & DOS9_ALIAS_FLAG) {
			/* this is an alias */

			Dos9_ExpandAlias(lpCommand,
			                 lpCmdLine + (iFlag & ~DOS9_ALIAS_FLAG),
			                 (char*)lpProc
			                );

			goto RestartSearch;

		}

		iErrorLevel=lpProc(lpCmdLine);

	}

	if (iErrorLevel!=lastErrorLevel) {

		snprintf(lpErrorlevel, sizeof(lpErrorlevel), "%d", iErrorLevel);
		Dos9_SetEnv(lpeEnv, "ERRORLEVEL",lpErrorlevel);
		lastErrorLevel=iErrorLevel;
	}

	return 0;
}


int Dos9_RunBlock(BLOCKINFO* lpbkInfo)
{

	ESTR *lpEsLine=Dos9_EsInit();

	char *lpToken = lpbkInfo->lpBegin,
	      *lpEnd = lpbkInfo->lpEnd,
	       *lpBlockBegin,
	       *lpBlockEnd,
	       *lpNl;

	size_t iSize;

	int iOldState;

	/* Save old lock state and lock the
	   level, definitely */
	iOldState=Dos9_GetStreamStackLockState(lppsStreamStack);
	Dos9_SetStreamStackLockState(lppsStreamStack, TRUE);

	DOS9_DBG("Block_b=\"%s\"\n"
	         "Block_e=\"%s\"\n",
	         lpToken,
	         lpEnd
	        );

	while (*lpToken && (lpToken < lpEnd)) {

		lpBlockBegin=Dos9_GetNextBlockBeginEx(lpToken, TRUE);

		/* get the block that are contained in the line */

		if (lpBlockBegin) {

			lpBlockEnd=Dos9_GetBlockLineEnd(lpBlockBegin);

			assert(lpBlockEnd != NULL);

			lpBlockBegin=lpBlockEnd;

		} else {

			lpBlockBegin=lpToken;

		}

		/* search the end of the line */
		if (!(lpBlockEnd=Dos9_SearchChar(lpBlockBegin, '\n'))) {

			lpBlockEnd=lpEnd;

		}

		lpBlockEnd++;

		if (lpBlockEnd > lpEnd)
			lpBlockEnd=lpEnd;

		iSize=lpBlockEnd-lpToken;

		Dos9_EsCpyN(lpEsLine, lpToken, iSize);

		//printf("Running=\"%s\"\n", Dos9_EsToChar(lpEsLine));

		//getch();

		lpToken=Dos9_SkipAllBlanks(lpToken);

		if (*lpToken=='\0'
		    || *lpToken=='\n') {

			/* don't run void lines, it is time wasting */
			lpToken=lpBlockEnd;

			continue;

		}

		lpToken=lpBlockEnd;

		Dos9_RunLine(lpEsLine);

		/* if we are asked to abort the command */
		if (bAbortCommand)
			break;


	}

	/* releases the lock */
	Dos9_SetStreamStackLockState(lppsStreamStack, iOldState);

	Dos9_EsFree(lpEsLine);

	return 0;
}

int Dos9_RunExternalCommand(char* lpCommandLine)
{

	char *lpArguments[FILENAME_MAX],
	     lpFileName[FILENAME_MAX],
	     lpExt[_MAX_EXT],
	     lpTmp[FILENAME_MAX],
	     lpExePath[FILENAME_MAX];

	ESTR* lpEstr[FILENAME_MAX];

	int i=0,
        status=0;


	Dos9_GetParamArrayEs(lpCommandLine, lpEstr, FILENAME_MAX);

	if (!lpEstr[0])
		return 0;

	Dos9_EsReplace(lpEstr[0], "\"", "");

	for (; lpEstr[i] && (i < FILENAME_MAX); i++) {
		lpArguments[i]=Dos9_EsToChar(lpEstr[i]);
	}

	lpArguments[i]=NULL;

	/* check if the program exist */

	if (Dos9_GetFilePath(lpFileName, lpArguments[0], sizeof(lpFileName))==-1) {

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
		                      lpArguments[0],
		                      FALSE);

        status=-1;
		goto error;

	}

	/* check if "command" is a batch file */
	Dos9_SplitPath(lpFileName, NULL, NULL, NULL, lpExt);

	if (!stricmp(".bat", lpExt)
	    || !stricmp(".cmd", lpExt)) {

        status=Dos9_RunExternalBatch(lpFileName, lpArguments);

	} else {

		status=Dos9_RunExternalFile(lpFileName, lpArguments);

    }

error:
	for (i=0; lpEstr[i] && (i < FILENAME_MAX); i++)
		Dos9_EsFree(lpEstr[i]);

	return status;

}


#ifdef WIN32

int Dos9_RunExternalFile(char* lpFileName, char** lpArguments)
{
	int res;

	errno=0;

    Dos9_ApplyEnv(lpeEnv);

	/* in windows the result is directly returned */
	res=spawnv(_P_WAIT, lpFileName, (char * const*)lpArguments);

	if (errno==ENOENT) {

		res=-1;

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
		                      lpArguments[0],
		                      FALSE
		                     );

	}


	return res;

}

#elif defined _POSIX_C_SOURCE

int Dos9_RunExternalFile(char* lpFileName, char** lpArguments)
{
	pid_t iPid;

	int iResult;

    Dos9_ApplyEnv(lpeEnv);

	iPid=fork();

	if (iPid == 0 ) {
		/* if we are in the son */

		if ( execv(lpFileName, lpArguments) == -1) {

			/* if we got here, we can't set ERRORLEVEL
			   variable anymore, but print an error message anyway.

			   This is problematic because if fork do not fail (that
			   is the usual behaviour) command line such as

			        batbox || goto error

			   will not work as expected. However, during search in the
			   path, command found exist, so the risk of such a
			   dysfunction is limited.

			   For more safety, we return -1, so that the given value will be
			   reported anyway*/

			Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
			                      lpArguments[0],
			                      FALSE
			                     );

			exit(-1);


		}

	} else {
		/* if we are in the father */

		if (iPid == (pid_t)-1) {
			/* the execution failed */

			Dos9_ShowErrorMessage(DOS9_FAILED_FORK | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_RunExternalFile()",
                                    -1
                                    );

			return -1;

		} else {

			wait(&iResult);

		}

	}

	return iResult;

}

#endif // WIN32 || _POSIX_C_SOURCE


#if defined(WIN32)

int Dos9_RunExternalBatch(char* lpFileName, char** lpArguments)
{

        int i;

        char lpTmp[FILENAME_MAX],
             lpExePath[FILENAME_MAX];
             *lpArgs[FILENAME_MAX+2];

        /* these are batch */

		for (; (i > 0) && (i < FILENAME_MAX); i--)
			lpArgs[i+2]=lpArguments[i];

		lpArguments[i+2]=NULL;

		lpArguments[2]=lpFilename;
		lpArguments[1]="//"; /* use this switch to prevent
                                other switches from being executed */

		Dos9_GetExePath(lpExePath, sizeof(lpExePath));

		snprintf(lpTmp, sizeof(lpFileName) ,"%s/dos9.exe", lpExePath);

		return Dos9_RunExternalFile(lpTmp, lpArgs);
}

#elif defined(_POSIX_C_SOURCE)

int Dos9_RunExternalBatch(char* lpFileName, char** lpArguments)
{

    pid_t pid;
    int status;
    int i;

    pid = fork();

    if (pid == 0) {

        /* if we are in the son process */

        Dos9_FreeLocalBlock(lpvLocalVars);
        lpvLocalVars = Dos9_GetLocalBlock();

        printf ("Setting local var before interpreting batch file..\n");

        for (i=1;lpArguments[i] && i <= 9; i++) {

            Dos9_SetLocalVar(lpvLocalVars, '0'+i, lpArguments[i]);

        }

        Dos9_SetLocalVar(lpvLocalVars, '0', lpFileName);

        strncpy(ifIn.lpFileName, lpFileName, sizeof(ifIn.lpFileName));
        ifIn.lpFileName[sizeof(ifIn.lpFileName)-1] = '\0';

        ifIn.bEof = 0;
        ifIn.iPos = 0;

        printf("Running batch file : \"%s\", starting at \"%d\"\n",
                ifIn.lpFileName, ifIn.iPos);

        Dos9_RunBatch(&ifIn);

        exit(iErrorLevel);

    } else if (pid == -1) {

        /* error */
        status = -1;

        Dos9_ShowErrorMessage(DOS9_FAILED_FORK | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_RunExternalFile()",
                                -1
                                );

    } else {

        wait(&status);

    }

    return status;

}

#endif /* WIN32 */
