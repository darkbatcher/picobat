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
#if !defined(WIN32) && !defined(_X_OPEN_SOURCE)
#define _X_OPEN_SOURCE
#endif

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>

#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "Dos9_Core.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_Lang.h"


int Dos9_RunBatch(INPUT_FILE* pIn)
{
	ESTR* lpLine=Dos9_EsInit();

	INPUT_FILE pIndIn;

	char* const lpCurrentDir=Dos9_GetCurrentDir();

	char *lpCh,
	     *lpTmp;

	int res;

	#ifndef WIN32
    	struct sigaction action;
    	memset(&action, 0, sizeof(action));
	#endif

	/* Create a non-local jump to get back to here if the user presses CTRL-C (ie. break)
       Note that by using break, the user admits some part of memory leakage...
       */
    if (setjmp(jbBreak));

	#ifndef WIN32
    	action.sa_handler=Dos9_SigHandlerBreak;
    	action.sa_flags=SA_NODEFER;
		sigaction(SIGINT, &action, NULL); /* Sets the default signal handler */
	#elif defined(WIN32)
		SetConsoleCtrlHandler(Dos9_SigHandler, TRUE); /* set default signal handler */
	#endif // WINDOWS

	while (!(pIn->bEof)) {

		DOS9_DBG("[*] %d : Parsing new line\n", __LINE__);


		if (*(pIn->lpFileName)=='\0'
		    && bEchoOn ) {

			Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IGREEN | DOS9_GET_BACKGROUND(colColor));
			printf(DOS9_NL "DOS9 ");

			Dos9_SetConsoleTextColor(colColor);

			printf("%s>" , lpCurrentDir);

		}

		/* the line we red was a void line */
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
			printf(DOS9_NL "DOS9 ");
			Dos9_SetConsoleTextColor(colColor);

			printf("%s>%s" DOS9_NL, lpCurrentDir, Dos9_EsToChar(lpLine));

		}

		Dos9_ReplaceVars(lpLine);

		bAbortCommand=FALSE;

		Dos9_RunLine(lpLine);

		if (bAbortCommand == -1)
			break;

		DOS9_DBG("\t[*] Line run.\n");

	}

	DOS9_DBG("*** Input ends here  ***\n");

	Dos9_EsFree(lpLine);

	return 0;

}

#ifdef WIN32

int Dos9_ExecOperators(PARSED_STREAM** lpppsStream)
{
	PARSED_STREAM* lppsStream = *lpppsStream;
	ESTR* lpCommand, *lpExpanded;

	char lpProgName[FILENAME_MAX],
         lpQuoteProgName[FILENAME_MAX+2],
		 lpAttrArgs[16]="/A:QE",
		 input[16],
		 output[16];
	char* lpArgs[FILENAME_MAX];

	int i,
		j=5,
		pipedes[2];

	int olddes = 0;

	lppsStreamStack=Dos9_Pipe(lppsStreamStack);

loop:

    if (lppsStream->lppsNode
        && lppsStream->lppsNode->cNodeType == PARSED_STREAM_NODE_PIPE) {

		/* OK, so we are running on Micro$oft Windows... This is pretty annoying
		   because the lack of fork equivalent call messes everything up,
		   resulting in harder job */

		/* Get pipe file descriptors */
		if (pipe(pipedes, 0, O_BINARY) == -1)
            Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_ExecOperators()",
                                    -1);

        Dos9_SetFdInheritance(pipedes[0], 0);

		/* prepare the command-line arguments before launching the program */
		Dos9_GetExeFilename(lpProgName, sizeof(lpProgName));
        snprintf(lpQuoteProgName, sizeof(lpQuoteProgName),
                                            "\"%s\"", lpProgName);

		lpArgs[(i=0)] = lpQuoteProgName;
		lpArgs[++i] = lpAttrArgs;

        if (bUseFloats)
            lpAttrArgs[j++] = 'F';

		if (bDelayedExpansion)
			lpAttrArgs[j++] = 'V';

		if (bCmdlyCorrect)
			lpAttrArgs[j++] = 'C';

		lpAttrArgs[j] = '\0';

		/* if the input descriptor is from a previous pipe */
		if (olddes != 0) {

            Dos9_SetFdInheritance(pipedes[0], 1);

			snprintf(input, sizeof(input), "%d", olddes);
			lpArgs[++i] =  "/I";
			lpArgs[++i] =  input;

		}

		snprintf(output, sizeof(output), "%d", pipedes[1]);

		lpArgs[++i] = "/O";
		lpArgs[++i] = output;

		/* Start the line */
		lpArgs[++i] = "/C";

		lpCommand = Dos9_EsInit();
		lpExpanded = Dos9_EsInit();
        Dos9_GetEndOfLine(lppsStream->lpCmdLine->str, lpExpanded);

		Dos9_EsCpy(lpCommand, "\"");
		Dos9_EsCatE(lpCommand, lpExpanded);
		Dos9_EsCat(lpCommand, "\"");

		lpArgs[++i] = Dos9_EsToChar(lpCommand);

		/* The la parameter must be NULL */
		lpArgs[++i] = NULL;

        Dos9_ApplyEnv(lpeEnv);
        Dos9_SetStdInheritance(1);

        /* Launches a sub Dos9 command prompt */
		_spawnv(_P_NOWAIT, lpProgName, (char * const*)lpArgs);

		if (errno == ENOENT) {

			Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR | DOS9_PRINT_C_ERROR,
		        	              lpArgs[0],
		    	    			  FALSE);
            Dos9_EsFree(lpCommand);
            Dos9_EsFree(lpExpanded);

			return FALSE;

		}

		/* Save the output of the pipe */
		close(pipedes[1]);

		if (olddes != 0)
			close(olddes);

		olddes = pipedes[0];

		Dos9_EsFree(lpCommand);
		Dos9_EsFree(lpExpanded);

    	if (lppsStream->lppsNode) {

    		lppsStream = *lpppsStream = lppsStream->lppsNode;
    		goto loop;

    	}

    } else if (olddes != 0) {

    	/* This is logically the last pipe of a series of pipes */
    	lppsStreamStack = Dos9_PushStreamStackIfNotPipe(lppsStreamStack);
		Dos9_OpenOutputD(lppsStreamStack, olddes, DOS9_STDIN);

    } else if (!lppsStream->lppsNode
               && lppsStream->cNodeType != PARSED_STREAM_NODE_PIPE) {

        lppsStreamStack = Dos9_Pipe(lppsStreamStack);

    }

	switch (lppsStream->cNodeType) {

	case PARSED_STREAM_NODE_PIPE:
	case PARSED_STREAM_NODE_NONE :
		/* this condition is always true */
		return TRUE;

	case PARSED_STREAM_NODE_NOT :
		/* this condition is true when the instruction
		   before failed */
		   Dos9_Pipe(lppsStreamStack);
		return iErrorLevel;

	case PARSED_STREAM_NODE_YES:
		   Dos9_Pipe(lppsStreamStack);
		return !iErrorLevel;

	}

	return FALSE;

}

#elif !defined(WIN32)

int Dos9_ExecOperators(PARSED_STREAM** lpppsStream)
{
    int pipedes[2];
    pid_t  pid;
    PARSED_STREAM* lppsStream=*lpppsStream;

loop:

    if (lppsStream->lppsNode
        && lppsStream->lppsNode->cNodeType == PARSED_STREAM_NODE_PIPE) {

        /* We're under Unix-like OS, fork, fork, fork, fork */

        if (pipe(pipedes) == -1)
            Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_ExecOperators()",
                                    -1);

        Dos9_SetFdInheritance(pipedes[0], 0);
        Dos9_SetFdInheritance(pipedes[1], 0);

        pid = fork();

        if (pid == 0) {

            /* don't care about redirections, this process will *eventually* die */
            Dos9_OpenOutputD(lppsStreamStack, pipedes[1], DOS9_STDOUT);

            Dos9_RunCommand(lppsStream->lpCmdLine);

            close (pipedes[1]);

            exit(iErrorLevel);

        } else if (pid == -1) {


            Dos9_ShowErrorMessage(DOS9_FAILED_FORK | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_ExecOperators()",
                                    -1);

        } else {

            close(pipedes[1]); /* do not need it yeah */

            /* do not create enormous stack in case of following pipes */
            lppsStreamStack = Dos9_PushStreamStackIfNotPipe(lppsStreamStack);

            /* Child process recieve's parent output */
            Dos9_OpenOutputD(lppsStreamStack, pipedes[0], DOS9_STDIN);

            if (lppsStream->lppsNode) {

                lppsStream = *lpppsStream = lppsStream->lppsNode;
                goto loop;

            }

        }

    } else if (!lppsStream->lppsNode
               && lppsStream->cNodeType != PARSED_STREAM_NODE_PIPE) {

        lppsStreamStack = Dos9_Pipe(lppsStreamStack);

    }

	switch (lppsStream->cNodeType) {

	case PARSED_STREAM_NODE_PIPE:
        return TRUE;

	case PARSED_STREAM_NODE_NONE :
		/* this condition is alwais true */
		lppsStreamStack = Dos9_Pipe(lppsStreamStack);
		return TRUE;

	case PARSED_STREAM_NODE_NOT :
		/* this condition is true when the instruction
		   before failed */
        lppsStreamStack = Dos9_Pipe(lppsStreamStack);
		return iErrorLevel;

	case PARSED_STREAM_NODE_YES:
        lppsStreamStack = Dos9_Pipe(lppsStreamStack);
		return !iErrorLevel;

	}

	return FALSE;

}

#endif

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

	//Dos9_RmTrailingNl(Dos9_EsToChar(lpLine));

	//fprintf(stderr, "line=%s\n", lpLine->str);

    DOS9_DBG("\t[*] Parsing line \"%s\"\n", lpLine->str);

	lppssStreamStart=Dos9_ParseLine(lpLine);

	if (!lppssStreamStart) {
		DOS9_DBG("!!! Can't parse line : \"%s\".\n", strerror(errno));
		return -1;
	}

	Dos9_ExecOutput(lppssStreamStart);

	DOS9_DBG("\t[*] Global streams set.\n");

	lppsStream=lppssStreamStart->lppsStream;

	do {

		if (Dos9_ExecOperators(&lppsStream)==FALSE || bAbortCommand)
			break;

		Dos9_RunCommand(lppsStream->lpCmdLine);

	} while ((lppsStream=lppsStream->lppsNode));

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
	char *lpCmdLine, *tmp;
	int iFlag, error = 0;

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

		iErrorLevel=Dos9_RunExternalCommand(lpCmdLine, &error);

		/* There is definitely an error that prevent the file
		   from being found. Thus, try another time, but expanding
		   the whole line for this moment. This behaviour appears to
		   be a little bit fuzzy, but I suspect cmd of having quite
		   the same behaviour... */
		switch (error) {
        case 1:
            /* This is the first error, expand lpCommand */
            Dos9_DelayedExpand(lpCommand, bDelayedExpansion);
            goto RestartSearch;

        case 2:
            /* This is clearly an error; the line failed to be parsed though
               it was given a second chance... */
            if (tmp = strpbrk(lpCmdLine, " \t\n"))
                *tmp = '\0'; /* do not fear to do trash with lpCmdLine, it
                                won't be used after */
            Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpCmdLine, FALSE);

        default:;
            /* This is ok */
		}

		break;

	default:

		if (iFlag & DOS9_ALIAS_FLAG) {
			/* this is an alias, expand it */

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

	while ((*lpToken) && (lpToken < lpEnd)) {
		/* get the block that are contained in the line */
        //printf("Block = %s\n", lpBlockBegin);

        lpBlockEnd = lpToken;

        do {

            lpBlockEnd = Dos9_SkipBlanks(lpBlockEnd);

            lpBlockEnd = Dos9_GetBlockLineEnd(lpBlockEnd);
            assert(lpBlockEnd);

            if (*lpBlockEnd == '&' || *lpBlockEnd == '|')
                lpBlockEnd ++;
            if (*lpBlockEnd == '&' || *lpBlockEnd == '|')
                lpBlockEnd ++;

        } while (*lpBlockEnd != '\0' && *lpBlockEnd != '\n'
                    && *lpBlockEnd != ')');


		iSize=lpBlockEnd-lpToken;

		Dos9_EsCpyN(lpEsLine, lpToken, iSize);

        if (*lpBlockEnd != '\0')
            lpBlockEnd++;

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

int Dos9_RunExternalCommand(char* lpCommandLine, int* error)
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

	for (; lpEstr[i] && (i < FILENAME_MAX); i++)
		lpArguments[i]=Dos9_EsToChar(lpEstr[i]);



	lpArguments[i]=NULL;
	/* check if the program exist */

	if (Dos9_GetFilePath(lpFileName, lpArguments[0], sizeof(lpFileName))==-1) {

        *error = *error+1;
        status=-1;
        goto error;

	}

	/* check if "command" is a batch file */
	Dos9_SplitPath(lpFileName, NULL, NULL, NULL, lpExt);

	if (!stricmp(".bat", lpExt)
	    || !stricmp(".cmd", lpExt)) {

        status=Dos9_RunExternalBatch(lpFileName, lpCommandLine, lpArguments);

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
	char str[FILENAME_MAX+2],
		 *tmp;
	errno=0;

	Dos9_ApplyEnv(lpeEnv);
	Dos9_SetStdInheritance(1);

	snprintf(str, sizeof(str), "\"%s\"", lpArguments[0]);
	tmp = lpArguments[0];
	lpArguments[0] = str;

	/* in windows the result is directly returned */
	res=spawnv(_P_WAIT, lpFileName, (char * const*)lpArguments);

	lpArguments[0] = tmp;

	if (errno==ENOENT) {

		res=-1;

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
		                      lpArguments[0],
		                      FALSE
		                     );

	}


	return res;

}

#elif !defined(WIN32)

int Dos9_RunExternalFile(char* lpFileName, char** lpArguments)
{
	pid_t iPid;

	int iResult = 0;

	iPid=fork();

	if (iPid == 0 ) {
		/* if we are in the son */

        Dos9_ApplyEnv(lpeEnv); /* set internal variable */
        Dos9_SetStdInheritance(1); /* make std* inheritable */

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

			waitpid(iPid, &iResult, 0);

		}

	}

	return WEXITSTATUS(iResult);

}

#endif // WIN32 || _POSIX_C_SOURCE


#if defined(WIN32)

int Dos9_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments)
{

        int i=FILENAME_MAX-1;
        int ret;

        char lpFile[FILENAME_MAX+3],
             lpExePath[FILENAME_MAX],
             lpModes[FILENAME_MAX] = {'/', 'a', ':' ,'q'},
             *lpArgs[FILENAME_MAX+3];

        /* these are batch */

		for (i; (i > 0) && (i < FILENAME_MAX); i--)
			lpArgs[i+3]=lpArguments[i];


		Dos9_GetExeFilename(lpExePath, sizeof(lpExePath));

        snprintf(lpFile, sizeof(lpFile), "\"%s\"", lpFileName);

		lpArgs[0]=lpExePath;
		lpArgs[3]=lpFile;
		lpArgs[2]="//"; /* use this switch to prevent
                                other switches from being executed */

        lpArgs[1]=lpModes;

        i = 4;

        if (bUseFloats)
            *(lpModes + (i++)) = 'f';
        if (bDelayedExpansion)
            *(lpModes + (i++)) = 'v';
        if (!bEchoOn)
            *(lpModes + (i++)) = 'e';
        if (bCmdlyCorrect)
            *(lpModes + (i++)) = 'c';

        *(lpModes + i) = '\0';

		ret = Dos9_RunExternalFile(lpExePath, lpArgs);
}

#elif !defined(WIN32)

int Dos9_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments)
{

    pid_t pid;
    int status;
    int i;

    pid = fork();

    if (pid == 0) {

        /* if we are in the son process */

        //Dos9_SetStreamStackLockState(lppsStreamStack, TRUE);

        Dos9_ClearStack(lppsStreamStack, (void(*)(void*))free);
        lppsStreamStack = Dos9_InitStreamStack();

        Dos9_FreeLocalBlock(lpvLocalVars);
        lpvLocalVars = Dos9_GetLocalBlock();

        for (i=1;lpArguments[i] && i <= 9; i++) {

            Dos9_SetLocalVar(lpvArguments, '0'+i, lpArguments[i]);

        }

        for (;i <= 9;i++)
            Dos9_SetLocalVar(lpvArguments, '0'+i , "");

        Dos9_SetLocalVar(lpvArguments, '*', lpFullLine);

        Dos9_SetLocalVar(lpvArguments, '0', lpFileName);

        bIsScript = 1; /* this is obviously a script */

        strncpy(ifIn.lpFileName, lpFileName, sizeof(ifIn.lpFileName));
        ifIn.lpFileName[sizeof(ifIn.lpFileName)-1] = '\0';

        ifIn.bEof = 0;
        ifIn.iPos = 0;

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

        waitpid(pid, &status, 0);

    }

    return WEXITSTATUS(status);

}

#endif /* WIN32 */

#ifndef WIN32

void Dos9_SigHandlerBreak(int sig)
{
    if (bIsScript == 0) {
        longjmp(jbBreak, 1);
    } else {
        exit(1);
    }
}

#elif defined WIN32
#include "../command/Dos9_Ask.h"

BOOL WINAPI Dos9_SigHandler(DWORD dwCtrlType)
{
    int choice, i;
    HANDLE thread;
    char lpExePath[FILENAME_MAX];
    ESTR* args;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;



	switch(dwCtrlType) {
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
            /* Request a handle to the main thread and try to freeze it */
            thread = OpenThread(THREAD_ALL_ACCESS, FALSE, iMainThreadId);

            if (thread == NULL)
                Dos9_ShowErrorMessage(DOS9_BREAK_ERROR, NULL, -1);

            /* suspend the main thread */
            SuspendThread(thread);

            if (bIsScript) {
                /* we are running a script, so give two options : either
                   continuing or killing Dos9 */

                fputs(DOS9_NL, stderr);
                choice = Dos9_AskConfirmation(DOS9_ASK_YN
                                              | DOS9_ASK_DEFAULT_N
                                              | DOS9_ASK_INVALID_REASK,
                    lpBreakConfirm);

                if (choice == DOS9_ASK_YES)
                    exit(-1);

                ResumeThread(thread);
                CloseHandle(thread);

            } else {
                /* Kill the main thread right now */
                TerminateThread(thread, -1);
                CloseHandle(thread);

                /* Odds are that some dos9 internal structures may
                   have been corrupted by the somehow brutal kill of the
                   main process. As the user *is* likely running interactive
                   command, (he is indeed to trigger CTRL-C), do not bother
                   that much and simply restart a new Dos9 command prompt. This
                   implies a little performance penalty, but ... */

                args = Dos9_EsInit();

                Dos9_GetExeFilename(lpExePath, sizeof(lpExePath));
                Dos9_EsCpy(args, lpExePath);
                Dos9_EsCat(args, " /a:q");

                if (!bEchoOn)
                    Dos9_EsCat(args, "e");
                if (bUseFloats)
                    Dos9_EsCat(args, "f");
                if (bDelayedExpansion)
                    Dos9_EsCat(args, "v");
                if (bCmdlyCorrect)
                    Dos9_EsCat(args, "c");

                ZeroMemory( &si, sizeof(si) );
                si.cb = sizeof(si);
                ZeroMemory( &pi, sizeof(pi) );

                /* By default, any file opened by Dos9 is set not to be
                   inherited by any subprocess. As this policy a bit
                   strict, just make the standard streams inheritable */
                Dos9_SetStdInheritance(1);

                /* Use create process rather than spawn in order to break
                   inheritance of probably broken stuff like fds */
                if( !CreateProcess( lpExePath,
                                    args->str,
                                    NULL,
                                    NULL,
                                    FALSE,
                                    0,
                                    NULL,
                                    NULL,
                                    &si,
                                    &pi ))
                    Dos9_ShowErrorMessage(DOS9_BREAK_ERROR, NULL, -1);

                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                exit(0);

            }

	}

	return TRUE;
}

#endif // _POSIX_C_SOURCE
