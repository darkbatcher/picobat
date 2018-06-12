/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <limits.h>
#include <sys/types.h>

#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "../../config.h"
#ifdef DOS9_USE_LIBCU8
#include <libcu8.h>
#endif

#include "Dos9_Core.h"
#include "../init/Dos9_Init.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_Lang.h"

int Dos9_RunBatch(INPUT_FILE* pIn)
{
	ESTR* lpLine=Dos9_EsInit();

	INPUT_FILE pIndIn;

	char *lpCh,
	     *lpTmp;

	int res;

	while (!(pIn->bEof)) {

		DOS9_DBG("[*] %d : Parsing new line\n", __LINE__);


		if (*(pIn->lpFileName)=='\0'
		    && bEchoOn ) {

			Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IGREEN | DOS9_GET_BACKGROUND(colColor));
			fprintf(fOutput, DOS9_NL "DOS9 ");

			Dos9_SetConsoleTextColor(colColor);

			fprintf(fOutput, "%s>" , lpCurrentDir);

		}

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
			fprintf(fOutput, DOS9_NL "DOS9 ");
			Dos9_SetConsoleTextColor(colColor);

			fprintf(fOutput, "%s>%s" DOS9_NL, lpCurrentDir, Dos9_EsToChar(lpLine));

		}

		Dos9_ReplaceVars(lpLine);



		Dos9_RunLine(lpLine);

		if (bAbortCommand == DOS9_ABORT_EXECUTION_LEVEL)
			break;
        else
            bAbortCommand = 0;

		DOS9_DBG("\t[*] Line run.\n");

	}

	DOS9_DBG("*** Input ends here  ***\n");

	Dos9_EsFree(lpLine);

	return iErrorLevel;

}


int Dos9_ExecOperators(PARSED_LINE** lpLine)
{
    int pipedes[2];
    THREAD res;
    struct pipe_launch_data_t* infos;
    PARSED_LINE* line=*lpLine;
    char *pch;

loop:
    if (line->lppsNode) {

        pch = line->lpCmdLine->str;
        pch = Dos9_SkipAllBlanks(pch);

        if ((!strnicmp(pch, "if", 2) && Dos9_IsDelim(*(pch +2)))
            || (!strnicmp(pch, "for", 3) && Dos9_IsDelim(*(pch + 3)))) {

            /* ignore lookahead commands */

        } else if (line->lppsNode->cNodeType == PARSED_STREAM_NODE_PIPE) {

            if (_Dos9_Pipe(pipedes, 4096, O_BINARY) == -1)
                Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR,
                                        __FILE__ "/Dos9_ExecOperators()",
                                        -1);

            Dos9_SetFdInheritance(pipedes[0], 0);
            Dos9_SetFdInheritance(pipedes[1], 0);

            if ((infos = malloc(sizeof(struct pipe_launch_data_t))) == NULL)
                Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                        __FILE__ "/Dos9_ExecOperators()", -1);

            /* prepare data to launch threads */
            infos->fd = pipedes[1];
            infos->str = Dos9_EsInit();

            Dos9_EsCpyE(infos->str, line->lpCmdLine);

            res = Dos9_CloneInstance(Dos9_LaunchPipe, infos);
            Dos9_CloseThread(&res);

            /* Listen from the pipe */
            lppsStreamStack = Dos9_OpenOutputD(lppsStreamStack, pipedes[0], DOS9_STDIN);
            close(pipedes[0]);

            if (line->lppsNode) {

                line = *lpLine = line->lppsNode;
                goto loop;

            }
        }
    }

	switch (line->cNodeType) {

	case PARSED_STREAM_NODE_PIPE:
        return TRUE;

	case PARSED_STREAM_NODE_NONE :
		/* this condition is always true */
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

void Dos9_LaunchPipe(struct pipe_launch_data_t* infos)
{

    lppsStreamStack = Dos9_OpenOutputD(lppsStreamStack, infos->fd, DOS9_STDOUT);
    close(infos->fd);

    bIgnoreExit = TRUE;

    Dos9_RunCommand(infos->str, NULL);

    Dos9_EsFree(infos->str);

    /* don't forget to free unneeded memory */
    free(infos);

}

int Dos9_ExecOutput(PARSED_STREAM* lppssStart)
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
	         lppssStart->cOutputMode & ~PARSED_STREAM_MODE_TRUNCATE,
	         lppssStart->cOutputMode & PARSED_STREAM_MODE_TRUNCATE,
	         STDOUT_FILENO
	        );

	if (!lppssStart
        || (!(lppssStart->lpInputFile)
	    && !(lppssStart->lpOutputFile)
        && !(lppssStart->lpErrorFile)
        && !(lppssStart->cRedir))) {

		/* nothing to be done, just return, now */
		return 0;

	}



	/* open the redirections */
	if (lppssStart->lpInputFile)
		lppsStreamStack = Dos9_OpenOutput(lppsStreamStack,
		                lppssStart->lpInputFile,
		                DOS9_STDIN,
		                0
		               );

	if (lppssStart->lpOutputFile)
		lppsStreamStack = Dos9_OpenOutput(lppsStreamStack,
		                lppssStart->lpOutputFile,
		                DOS9_STDOUT,
		                lppssStart->cOutputMode
		               );

    if (lppssStart->lpErrorFile)
		lppsStreamStack = Dos9_OpenOutput(lppsStreamStack,
		                lppssStart->lpErrorFile,
		                DOS9_STDERR,
		                lppssStart->cErrorMode
		               );

    switch (lppssStart->cRedir) {

    case PARSED_STREAM_STDERR2STDOUT:

        lppsStreamStack = Dos9_OpenOutputD(lppsStreamStack,
		                -1,
		                -1
		               );
        fError = _fOutput;
        break;

    case PARSED_STREAM_STDOUT2STDERR:
        lppsStreamStack = Dos9_OpenOutputD(lppsStreamStack,
		                -1 ,
		                -1
		               );
        fOutput = _fError;

    }

	return 0;
}

int Dos9_RunLine(ESTR* lpLine)
{
	PARSED_LINE *orig; /* the parsed line*/

	orig = Dos9_ParseLine(lpLine);

	if (orig == NULL) {

		DOS9_DBG("!!! Can't parse line : \"%s\".\n", strerror(errno));
		return -1;

	}

	Dos9_RunParsedLine(orig);

	Dos9_FreeLine(orig);

	return iErrorLevel;
}

void Dos9_RunParsedLine(PARSED_LINE* line)
{
	int lock;

	do {

		lock = Dos9_GetStreamStackLockState(lppsStreamStack);
		Dos9_SetStreamStackLockState(lppsStreamStack, 1);

		if (Dos9_ExecOperators(&line)==FALSE || bAbortCommand) {

            lppsStreamStack = Dos9_PopStreamStackUntilLock(lppsStreamStack);
            Dos9_SetStreamStackLockState(lppsStreamStack, lock);
			break;

        }

		/* open file streams (ie. those induced by '>' or '<') */
		Dos9_ExecOutput(line->sStream);

		Dos9_RunCommand(line->lpCmdLine, &line);

		lppsStreamStack = Dos9_PopStreamStackUntilLock(lppsStreamStack);
		Dos9_SetStreamStackLockState(lppsStreamStack, lock);

	} while (line && (line=line->lppsNode));

}

int Dos9_RunCommand(ESTR* lpCommand, PARSED_LINE** lpplLine)
{
#define XSTR(x) #x
#define STR(x) XSTR(x)

	int (*lpProc)(char*);

	char lpErrorlevel[sizeof(STR(INT_MIN))],
         lpTmpLine[]="CD X:";
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

	switch((iFlag=Dos9_GetCommandProc(lpCmdLine, lpclCommands,
														(void**)&lpProc))) {

	case -1:

		iErrorLevel = Dos9_RunExternalCommand(lpCmdLine, &error);

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
                                won't be used afterwards */
            Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpCmdLine, FALSE);

        default:;
            /* This is ok */
		}

		break;

	default:

		if (iFlag & DOS9_ALIAS_FLAG) {
			/* this is an alias, expand it */

			Dos9_ExpandAlias(lpCommand,
			                 lpCmdLine + (iFlag & ~DOS9_COMMAND_FLAGS),
			                 (char*)lpProc
			                );

			goto RestartSearch;

		}

		if (iFlag & DOS9_COMMAND_LOOKAHEAD) {

            iErrorLevel=((int(*)(char*, PARSED_LINE**))lpProc)(lpCmdLine,
																	lpplLine);

        } else {

            iErrorLevel=lpProc(lpCmdLine);

        }
	}

	return iErrorLevel;
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
		if (bAbortCommand == DOS9_ABORT_EXECUTION_LEVEL
            || bAbortCommand == DOS9_ABORT_COMMAND_BLOCK)
			break;
        else
            bAbortCommand = 0;

	}

	/* releases the lock */
	Dos9_SetStreamStackLockState(lppsStreamStack, iOldState);

	Dos9_EsFree(lpEsLine);

	return iErrorLevel;
}

int Dos9_RunExternalCommand(char* lpCommandLine, int* error)
{

	char **lpArguments = NULL,
	     lpFileName[FILENAME_MAX],
	     lpExt[_MAX_EXT];

	ESTR *lpCmdLine = Dos9_EsInit();

	PARAMLIST *list=NULL, *item;

    EXECINFO info;

	int i=0,
        status=0;

    size_t nb=0;

    if ((list = Dos9_GetParamList(lpCommandLine)) == NULL)
        return 0;

    item = list;

    while (item) {

        nb ++;
        item = item->next;

    }

    Dos9_GetEndOfLine(lpCommandLine, lpCmdLine);

    if ((lpArguments = malloc((nb + 1) * sizeof(char*))) == NULL) {

        *error = *error+1;
        status=-1;
        goto error;

    }

    item = list;

    while (i < nb) {

        lpArguments[i] = item->param->str;
        item = item->next;
        i ++;
    }

	lpArguments[nb]=NULL;
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

        status = Dos9_RunExternalBatch(lpFileName, lpCmdLine->str, lpArguments);

	} else {

        info.file = lpFileName;
        info.cmdline = lpCmdLine->str;
        info.args = lpArguments;
        info.dir = lpCurrentDir;
        info.title = NULL;
        info.flags = DOS9_EXEC_WAIT;

		status = Dos9_ExecuteFile(&info);

    }

error:
    Dos9_EsFree(lpCmdLine);

    if (lpArguments)
        free(lpArguments);

    if (list)
        Dos9_FreeParamList(list);

	return status;

}

struct batch_launch_data_t {
    char* lpFileName;
    char* lpFullLine;
    char** lpArguments;
};

void Dos9_LaunchExternalBatch(struct batch_launch_data_t* arg)
{
    int i;
    char* str;
    ESTR* tmp;

    Dos9_FreeLocalBlock(lpvLocalVars);
    Dos9_FreeLocalBlock(lpvArguments);
    lpvLocalVars = Dos9_GetLocalBlock();
    lpvArguments = Dos9_GetLocalBlock();

    if (isatty(fileno(fOutput)))
        setvbuf(fOutput, NULL, _IONBF, 0);

    if (isatty(fileno(fError)))
        setvbuf(fError, NULL, _IONBF, 0);

    for (i=1;arg->lpArguments[i] && i <= 9; i++)
        Dos9_SetLocalVar(lpvArguments, '0'+i, arg->lpArguments[i]);


    Dos9_AssignCommandLine('+', arg->lpArguments + i);

    for (;i <= 9;i++)
        Dos9_SetLocalVar(lpvArguments, '0'+i , "");

    /* well skip the very first argument as it represents script
       name */

    tmp = Dos9_EsInit();

    if (str = Dos9_GetNextParameterEs(arg->lpFullLine, tmp)) {

        str = Dos9_SkipBlanks(str);
        Dos9_SetLocalVar(lpvArguments, '*', str);

    } else
        Dos9_SetLocalVar(lpvArguments, '*', arg->lpFullLine);

    Dos9_EsFree(tmp);

    Dos9_SetLocalVar(lpvArguments, '0', arg->lpFileName);

    bIgnoreExit = TRUE;
    bIsScript = 1; /* this is obviously a script */

    strncpy(ifIn.lpFileName, arg->lpFileName, sizeof(ifIn.lpFileName));
    ifIn.lpFileName[sizeof(ifIn.lpFileName)-1] = '\0';

    for (i = 0; arg->lpArguments[i]; i++)
        free(arg->lpArguments[i]);

    free(arg->lpArguments);
    free(arg->lpFileName);
    free(arg->lpFullLine);
    free(arg);

    ifIn.bEof = 0;
    ifIn.iPos = 0;

    Dos9_RunBatch(&ifIn);
}


int Dos9_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments)
{

    THREAD th;
    struct batch_launch_data_t* arg;
    int size = 0;
    void* ret;

    while (lpArguments[size])
        size ++; /* count argument items */

    if ((arg = malloc(sizeof(struct batch_launch_data_t))) == NULL
        || (arg->lpFileName = strdup(lpFileName)) == NULL
        || (arg->lpFullLine = strdup(lpFullLine)) == NULL
        || (arg->lpArguments = malloc((size + 1) * sizeof(char*))) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                              __FILE__ "/Dos9_RunExternalBatch()", -1);

    size = 0;
    while (lpArguments[size]) {

        if ((arg->lpArguments[size] = strdup(lpArguments[size])) == NULL)
            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                              __FILE__ "/Dos9_RunExternalBatch()", -1);

        size ++;

    }

    arg->lpArguments[size] = NULL;

    th = Dos9_CloneInstance(Dos9_LaunchExternalBatch, arg);

    if (Dos9_WaitForThread(&th, &ret) != 0)
        Dos9_CloseThread(&th);

    return (int)ret;
}

PARSED_LINE* Dos9_LookAHeadMakeParsedLine(BLOCKINFO* block,PARSED_LINE* lookahead)
{
	ESTR* cmd = Dos9_EsInit();

    if (!lookahead) {
        if (!(lookahead = malloc(sizeof(PARSED_LINE)))) {

            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_LookAHeadMakeParsedLine()",
                                    -1);
            return NULL;

        } else {

            lookahead->sStream = NULL;
            lookahead->lppsNode = NULL;
            lookahead->cNodeType = 0;
            lookahead->lpCmdLine = NULL;

        }
    } else {

        /* Reuse the current struct in lookahead (clean it
           carefully */
        Dos9_FreeParsedStream(lookahead->sStream);
        lookahead->sStream = NULL;

    }

    Dos9_EsCpy(cmd, "(");
	Dos9_EsCatN(cmd, block->lpBegin, block->lpEnd - block->lpBegin);
    Dos9_EsCat(cmd, ")");

    if (lookahead->lpCmdLine)
        Dos9_EsFree(lookahead->lpCmdLine);

    lookahead->lpCmdLine = cmd;

    return lookahead;
}

#ifndef WIN32
#include "../command/Dos9_Ask.h"
/* Unix is more sympathetic to us than windows. Indeed,
   Posix thread guaranties a signal handler to be ran
   by any of the existing threads. This means that
   every thread-local data is initialized and that we
   won't have any problem for the streams but we may
   have if we use allocated structures like ENVBUF* */
void Dos9_SigHandlerBreak(int sig)
{
    int choice, i;
    char lpExePath[FILENAME_MAX];
    ESTR* attr;

    /* using bIsScript is safe because it is inherited on a
       call to Dos9_CloneInstance() */
    if (bIsScript) {

        Dos9_AskConfirmation(DOS9_ASK_YN
                                | DOS9_ASK_INVALID_REASK
                                | DOS9_ASK_DEFAULT_N,
                                lpBreakConfirm);

        if (choice == DOS9_ASK_NO)
            return 0;

    }

    attr = Dos9_EsInit();

    Dos9_GetExeFilename(lpExePath, sizeof(lpExePath));

    Dos9_EsCpy(attr, lpExePath);
    Dos9_EsCpy(attr, "/a:q");

    if (!bEchoOn)
        Dos9_EsCat(attr, "e");
    if (bUseFloats)
        Dos9_EsCat(attr, "f");
    if (bDelayedExpansion)
        Dos9_EsCat(attr, "v");
    if (bCmdlyCorrect)
        Dos9_EsCat(attr, "c");

    chdir(lpCurrentDir);
    Dos9_ApplyEnv(lpeEnv);

    execl(lpExePath, lpExePath, attr->str);
}

#elif defined WIN32
#include "../command/Dos9_Ask.h"
/* This is getting increasingly complicated since the
   multi-threaded version. Before, it was kind of simple
   since we had the correspondence 1 interpreter = one process
   and when a signal was raised a new thread was launched
   on the same process.

   This golden time is somehow now over since we have
   several threads inside.

   The biggest problem we have here, is the fact we might
   start Dos9 from *scratch* due to the difficulty to
   retrieve environment variables that are now stored
   thread-local and the fact that Windows calls this
   handler within a brand-new thread (with nothing
   initialized of course...).

   Beware not to refer to any of fInput, fOutput, and
   whatever thread-local here. */
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
		    SetConsoleCtrlHandler(NULL, TRUE);

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
                fprintf(stderr, "%s %s ", lpBreakConfirm, lpAskyN);

                args = Dos9_EsInit();

                choice = DOS9_ASK_NO;

                while (Dos9_EsGet(args, stdin)) {

                    Dos9_RmTrailingNl(args->str);

                    if (!stricmp(args->str, lpAskYes)
                        || !stricmp(args->str, lpAskYesA)) {

                        choice = DOS9_ASK_YES;
                        break;

                    } else if (!stricmp(args->str, lpAskNo)
                               || !stricmp(args->str, lpAskNoA))
                        break;

                }

                if (choice == DOS9_ASK_NO) {

                    ResumeThread(thread);
                    CloseHandle(thread);
                    return TRUE;

                }
            }

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
            Dos9_EsCpy(args, "\"");
            Dos9_EsCat(args, lpExePath);
            Dos9_EsCat(args, "\"");
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
                                &pi )) {

                /* The purpose of this is not to rewrite code again and again for
                   unmaintainability's sake, but, not to refer to all this
                   thread local defined stuff */

                Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IRED);

                fprintf(stderr, lpErrorMsg[DOS9_BREAK_ERROR]);

                Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);

                fputs(DOS9_NL, stderr);
                fprintf(stderr, lpQuitMessage);
                Dos9_Getch();


                exit(-1);

            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            exit(0);

    }

	return TRUE;
}

#endif // _POSIX_C_SOURCE
