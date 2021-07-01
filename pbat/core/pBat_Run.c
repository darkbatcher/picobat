/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#ifdef PBAT_USE_LIBCU8
#include <libcu8.h>
#endif

#include "pBat_Core.h"
#include "../init/pBat_Init.h"

//#define PBAT_DBG_MODE
#include "pBat_Debug.h"

#include "../errors/pBat_Errors.h"
#include "../lang/pBat_Lang.h"

int pBat_RunBatch(INPUT_FILE* pIn)
{
	ESTR* lpLine=pBat_EsInit();

	INPUT_FILE pIndIn;

	char *lpCh,
	     *lpTmp;

	int res;

	while (!(pIn->bEof)) {

		if (*(pIn->lpFileName)=='\0'
		    && bEchoOn ) {

            fputs(PBAT_NL, fOutput);
		    pBat_OutputPrompt();

        }

		if (pBat_GetLine(lpLine, pIn))
			continue;

		lpCh=pBat_EsToChar(lpLine);

		while (*lpCh==' '
		       || *lpCh=='\t'
		       || *lpCh==';')
			lpCh++;

		if (*(pIn->lpFileName)!='\0'
		    && bEchoOn
		    && *lpCh!='@') {

            pBat_OutputPrompt();
			fprintf(fOutput, "%s" PBAT_NL, pBat_EsToChar(lpLine));

		}

		pBat_RunLine(lpLine);

		if (bAbortCommand == PBAT_ABORT_EXECUTION_LEVEL)
			break;
        else
            bAbortCommand = 0;

	}

	pBat_EsFree(lpLine);

	return iErrorLevel;

}

#define EXECOPERATORS_SKIP 1
#define EXECOPERATORS_END 2
int pBat_ExecOperators(PARSED_LINE** lpLine)
{
    int pipedes[2];
    THREAD res;
    struct pipe_launch_data_t* infos;
    PARSED_LINE* line=*lpLine;
    char *pch;
    int status = 0,
        lastin = -1;

    pch = line->lpCmdLine->str;
    pch = pBat_SkipAllBlanks(pch);

    /* if we encounter a lookahead command, end processing the line,
       indeed if and for automatically swallow every operators on the
       right hand side of the line

       so return the EXECOPERATORS_END status */
    if ((!strnicmp(pch, "if", 2) && pBat_IsDelim(*(pch +2)))
        || (!strnicmp(pch, "for", 3) && pBat_IsDelim(*(pch + 3))))
        status |= EXECOPERATORS_END;

    switch (line->cNodeType) {


    /* If we get to this, it is because the first command of the
       pipe sequence has not be executed, so skip*/
	case PARSED_STREAM_NODE_PIPE:
        status |= EXECOPERATORS_SKIP;

	case PARSED_STREAM_NODE_NONE :
		/* this condition is always true */
		break;

	case PARSED_STREAM_NODE_NOT :
		/* this condition is true when the instruction
		   before failed */
		status |= iErrorLevel ? 0 : EXECOPERATORS_SKIP;
		break;

	case PARSED_STREAM_NODE_YES:
		status |= iErrorLevel ?  EXECOPERATORS_SKIP : 0;

	}

    /* Loop through a pipe chain, spawning parallel threads using
       clone instance */
    while (line->lppsNode
        && line->lppsNode->cNodeType == PARSED_STREAM_NODE_PIPE
        && !status) {


        /* Serialize this withs calls to pBat_RunFile() */
        PBAT_RUNFILE_LOCK();

        if (_pBat_Pipe(pipedes, 4096, O_BINARY) == -1)
            pBat_ShowErrorMessage(PBAT_CREATE_PIPE | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_ExecOperators()",
                                    -1);

        PBAT_RUNFILE_RELEASE();

        if ((infos = malloc(sizeof(struct pipe_launch_data_t))) == NULL)
            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_ExecOperators()", -1);

        /* prepare data to launch threads */
        infos->fdout = pipedes[1];
        infos->fdin = lastin;

        infos->str = pBat_EsInit();
        pBat_EsCpyE(infos->str, line->lpCmdLine);

        if ((infos->stream = pBat_DuplicateParsedStream(line->sStream)) == NULL)
            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_ExecOperators()", -1);

        res = pBat_CloneInstance((void(*)(void*))pBat_LaunchPipe, infos);
        pBat_CloseThread(&res);

        lastin = pipedes[0];
        if (line->lppsNode)
            line = *lpLine = line->lppsNode;
        else
            break;

    }

    if (lastin != -1) {

        lppsStreamStack = pBat_OpenOutputD(lppsStreamStack, lastin, PBAT_STDIN);
        close(lastin);
    }

    return status;

}

void pBat_LaunchPipe(struct pipe_launch_data_t* infos)
{

    lppsStreamStack = pBat_OpenOutputD(lppsStreamStack, infos->fdout, PBAT_STDOUT);
    close(infos->fdout);

    if (infos->fdin !=-1) {
        lppsStreamStack = pBat_OpenOutputD(lppsStreamStack, infos->fdin, PBAT_STDIN);
        close(infos->fdin);
    }

    bIgnoreExit = TRUE;

    /* We can do this since lookahead command can not be processed using this
       function unless they are wrapped inside a command block due to operators
       precedence rules ie

            if 1==1 if 1==1 echo test | find test

       is equivalent to

            if 1==1 (if 1==1 (echo test | find test))

       as this function executes the left hand side of a pipe.

       */
    pBat_ExecOutput(infos->stream);

    pBat_RunCommand(infos->str, NULL);

    /* don't forget to free unneeded memory */
    pBat_FreeParsedStream(infos->stream);
    pBat_EsFree(infos->str);
    free(infos);

}

int pBat_ExecOutput(PARSED_STREAM* lppssStart)
{

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
		lppsStreamStack = pBat_OpenOutput(lppsStreamStack,
		                lppssStart->lpInputFile,
		                PBAT_STDIN,
		                0
		               );

	if (lppssStart->lpOutputFile)
		lppsStreamStack = pBat_OpenOutput(lppsStreamStack,
		                lppssStart->lpOutputFile,
		                PBAT_STDOUT,
		                lppssStart->cOutputMode
		               );

    if (lppssStart->lpErrorFile)
		lppsStreamStack = pBat_OpenOutput(lppsStreamStack,
		                lppssStart->lpErrorFile,
		                PBAT_STDERR,
		                lppssStart->cErrorMode
		               );

    switch (lppssStart->cRedir) {

    case PARSED_STREAM_STDERR2STDOUT:

        lppsStreamStack = pBat_OpenOutputD(lppsStreamStack,
		                -1,
		                -1
		               );
        fError = _fOutput;
        break;

    case PARSED_STREAM_STDOUT2STDERR:
        lppsStreamStack = pBat_OpenOutputD(lppsStreamStack,
		                -1 ,
		                -1
		               );
        fOutput = _fError;

    }

	return 0;
}

int pBat_RunLine(ESTR* lpLine)
{
	PARSED_LINE *orig; /* the parsed line*/

	/* skip the line anyway if it start with a ':'. Note that
	   such a line is by no means a label since ':' are filtered by script
       reading functions. this is thus a ':' obtained through expansion.... */
	if (*(pBat_SkipBlanks(lpLine->str)) == ':')
        return 0;

	orig = pBat_ParseLine(lpLine);

	if (orig == NULL)
		return -1;


	pBat_RunParsedLine(orig);

	pBat_FreeLine(orig);

	return iErrorLevel;
}

void pBat_RunParsedLine(PARSED_LINE* line)
{
	int lock, ok;
	char *pch;

	do {

		lock = pBat_GetStreamStackLockState(lppsStreamStack);
		pBat_SetStreamStackLockState(lppsStreamStack, 1);

        pch = line->lpCmdLine->str;
        pch = pBat_SkipAllBlanks(pch);

        ok = pBat_ExecOperators(&line);

        /* If aborting, remove the last level of redirection
           EXECOPERATORS_SKIP means that the line tokens is
           skipped when using || or && */
		if ((ok & EXECOPERATORS_SKIP) || bAbortCommand) {

            lppsStreamStack = pBat_PopStreamStackUntilLock(lppsStreamStack);
            pBat_SetStreamStackLockState(lppsStreamStack, lock);

            if (bAbortCommand)
                break;
            else
                continue;

        }

		/* open file streams (ie. those induced by '>' or '<')
		   Note that FOR and IF never trigger file stream execution
		   because they are considered lookahead commands. As such
		   any redirection applies to the leftmost element inside the
		   IF or the for:

		   Example :

                If > foo 1==1 echo test & echo test2

		   is equivalent to

                If 1==1 (echo test > foo & echo test2)

            */
		if (!((!strnicmp(pch, "if", 2) && pBat_IsDelim(*(pch +2)))
            || (!strnicmp(pch, "for", 3) && pBat_IsDelim(*(pch + 3)))))
            pBat_ExecOutput(line->sStream);

		pBat_RunCommand(line->lpCmdLine, &line);

		lppsStreamStack = pBat_PopStreamStackUntilLock(lppsStreamStack);
		pBat_SetStreamStackLockState(lppsStreamStack, lock);

	} while (line && (line=line->lppsNode)
             && !(ok & EXECOPERATORS_END));

}

int pBat_RunCommand(ESTR* lpCommand, PARSED_LINE** lpplLine)
{
#define XSTR(x) #x
#define STR(x) XSTR(x)

	int (*lpProc)(char*);

	char lpErrorlevel[sizeof(STR(INT_MIN))],
         lpTmpLine[]="CD X:";
	char *lpCmdLine, *tmp;
	int iFlag, error = 0;

RestartSearch:

	lpCmdLine=pBat_EsToChar(lpCommand);

	lpCmdLine=pBat_SkipAllBlanks(lpCmdLine);

#if defined(WIN32)

    /* handle "A:" */

	if (*lpCmdLine && *(lpCmdLine+1)==':'
	    && *pBat_SkipAllBlanks(lpCmdLine+2) == '\0') {

        lpTmpLine[3] = *lpCmdLine;
        lpCmdLine = lpTmpLine;

    }

#endif

	switch((iFlag=pBat_GetCommandProc(lpCmdLine, lpclCommands,
														(void**)&lpProc))) {

	case -1:

		iErrorLevel = pBat_RunExternalCommand(lpCmdLine, &error);

		/* There is definitely an error that prevent the file
		   from being found. Thus, try another time, but expanding
		   the whole line for this moment. This behaviour appears to
		   be a little bit fuzzy, but I suspect cmd of having quite
		   the same behaviour... */
		switch (error) {
        case 1:
            /* This is the first error, expand lpCommand */
            pBat_DelayedExpand(lpCommand);
            goto RestartSearch;

        case 2:
            /* This is clearly an error; the line failed to be parsed though
               it was given a second chance... */
            if (tmp = strpbrk(lpCmdLine, " \t\n"))
                *tmp = '\0'; /* do not fear to do trash with lpCmdLine, it
                                won't be used afterwards */
            pBat_ShowErrorMessage(PBAT_COMMAND_ERROR, lpCmdLine, FALSE);

        default:;
            /* This is ok */
		}

		break;

	default:

		if (iFlag & PBAT_DEF_FLAG) {

			/* this is a subroutine, expand it */
			pBat_ExpandDef(lpCommand,
			                 lpCmdLine + (iFlag & ~PBAT_COMMAND_FLAGS),
                             (char*)lpProc); /* Not obvious but in this case lpProc
                                                is variable to be substituted */
            tmp = (char*)lpvLocalVars;
            if (((lpvLocalVars = pBat_GetLocalBlock()) == NULL))
                pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                      __FILE__ "/pBat_RunFile",
                                      PBAT_FAILED_ALLOCATION);

            pBat_PushEnvLocals(lpeEnv);
            pBat_RunCommand(lpCommand, lpplLine);
            pBat_PopEnvLocals(lpeEnv);

            pBat_FreeLocalBlock(lpvLocalVars);
            lpvLocalVars = (LOCAL_VAR_BLOCK*)tmp;

		} else if (iFlag & PBAT_COMMAND_LOOKAHEAD) {

            iErrorLevel=((int(*)(char*, PARSED_LINE**))lpProc)(lpCmdLine,
																	lpplLine);

        } else {

            iErrorLevel=lpProc(lpCmdLine);

        }
	}

	return iErrorLevel;
}


int pBat_RunBlock(BLOCKINFO* lpbkInfo)
{

	ESTR *lpEsLine = pBat_EsInit(),
		 *lpEsBlock = pBat_EsInit();

	char *lpToken,
         *lpEnd,
         *lpBlockBegin,
         *lpBlockEnd,
         *lpNl;

	size_t iSize;

	int iOldState;

	/* Save old lock state and lock the
	   level, definitely */
	iOldState=pBat_GetStreamStackLockState(lppsStreamStack);
	pBat_SetStreamStackLockState(lppsStreamStack, TRUE);



	if (lpbkInfo->lpEnd == NULL) {

		pBat_EsCpy(lpEsBlock, lpbkInfo->lpBegin);
		lpEnd = lpEsBlock->str + strlen(lpEsBlock->str);

	} else {

		pBat_EsCpyN(lpEsBlock,  lpbkInfo->lpBegin,
				(size_t)(lpbkInfo->lpEnd - lpbkInfo->lpBegin));
		lpEnd = lpEsBlock->str + (size_t)(lpbkInfo->lpEnd -
					lpbkInfo->lpBegin);
	}

	lpToken = lpEsBlock->str;

	while ((*lpToken) && (lpToken < lpEnd)) {
		/* get the block that are contained in the line */
        //printf("Block = %s\n", lpBlockBegin);

        lpBlockEnd = lpToken;

        do {

            lpBlockEnd = pBat_SkipBlanks(lpBlockEnd);

            lpBlockEnd = pBat_GetBlockLineEnd(lpBlockEnd);
            assert(lpBlockEnd);

            if (*lpBlockEnd == '&' || *lpBlockEnd == '|')
                lpBlockEnd ++;
            if (*lpBlockEnd == '&' || *lpBlockEnd == '|')
                lpBlockEnd ++;

        } while (*lpBlockEnd != '\0' && *lpBlockEnd != '\n'
                    && *lpBlockEnd != ')');


		iSize=lpBlockEnd-lpToken;

		pBat_EsCpyN(lpEsLine, lpToken, iSize);

        if (*lpBlockEnd != '\0')
            lpBlockEnd++;

		lpToken=pBat_SkipAllBlanks(lpToken);

		if (*lpToken=='\0'
		    || *lpToken=='\n') {

			/* don't run void lines, it is time wasting */
			lpToken=lpBlockEnd;

			continue;

		}

		lpToken=lpBlockEnd;

		pBat_RunLine(lpEsLine);

		/* if we are asked to abort the command */
		if (bAbortCommand == PBAT_ABORT_EXECUTION_LEVEL
            || bAbortCommand == PBAT_ABORT_COMMAND_BLOCK)
			break;
        else
            bAbortCommand = 0;

	}

	/* releases the lock */
	pBat_SetStreamStackLockState(lppsStreamStack, iOldState);

	pBat_EsFree(lpEsLine);
	pBat_EsFree(lpEsBlock);

	return iErrorLevel;
}

int pBat_RunExternalCommand(char* lpCommandLine, int* error)
{

	char **lpArguments = NULL,
	     lpFileName[FILENAME_MAX],
	     lpExt[_MAX_EXT];

	ESTR *lpCmdLine = pBat_EsInit();

	PARAMLIST *list=NULL, *item;

    EXECINFO info;

	int i=0,
        status=0;

    size_t nb=0;

    if ((list = pBat_GetParamList(lpCommandLine)) == NULL)
        return 0;

    item = list;

    while (item) {

        nb ++;
        item = item->next;

    }

    pBat_GetEndOfLine(lpCommandLine, lpCmdLine);

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

	if (pBat_GetFilePath(lpFileName, lpArguments[0], sizeof(lpFileName))==-1) {

        *error = *error+1;
        status=-1;
        goto error;

	}

	/* check if "command" is a batch file */
	pBat_SplitPath(lpFileName, NULL, NULL, NULL, lpExt);

	if (!stricmp(".bat", lpExt)
	    || !stricmp(".cmd", lpExt)) {

        status = pBat_RunExternalBatch(lpFileName, lpCmdLine->str, lpArguments);

	} else {

        info.file = lpFileName;
        info.cmdline = lpCmdLine->str;
        info.args = (const char* const*)lpArguments;
        info.dir = lpCurrentDir;
        info.title = NULL;
        info.flags = PBAT_EXEC_WAIT;

		status = pBat_ExecuteFile(&info);

    }

error:
    pBat_EsFree(lpCmdLine);

    if (lpArguments)
        free(lpArguments);

    if (list)
        pBat_FreeParamList(list);

	return status;

}

struct batch_launch_data_t {
    char* lpFileName;
    char* lpFullLine;
    char** lpArguments;
};

void pBat_LaunchExternalBatch(struct batch_launch_data_t* arg)
{
    int i;
    char* str;
    ESTR* tmp;

    pBat_FreeLocalBlock(lpvLocalVars);
    pBat_FreeLocalBlock(lpvArguments);
    lpvLocalVars = pBat_GetLocalBlock();
    lpvArguments = pBat_GetLocalBlock();

    if (isatty(fileno(fOutput)))
        setvbuf(fOutput, NULL, _IONBF, 0);

    if (isatty(fileno(fError)))
        setvbuf(fError, NULL, _IONBF, 0);

    for (i=1;arg->lpArguments[i] && i <= 9; i++)
        pBat_SetLocalVar(lpvArguments, '0'+i, arg->lpArguments[i]);


    pBat_AssignCommandLine('+', arg->lpArguments + i);

    for (;i <= 9;i++)
        pBat_SetLocalVar(lpvArguments, '0'+i , "");

    /* well skip the very first argument as it represents script
       name */

    tmp = pBat_EsInit();

    if (str = pBat_GetNextParameterEs(arg->lpFullLine, tmp)) {

        str = pBat_SkipBlanks(str);
        pBat_SetLocalVar(lpvArguments, '*', str);

    } else
        pBat_SetLocalVar(lpvArguments, '*', arg->lpFullLine);

    pBat_EsFree(tmp);

    pBat_SetLocalVar(lpvArguments, '0', arg->lpFileName);

    bIgnoreExit = TRUE;
    bIsScript = 1; /* this is obviously a script */
    pBat_SetEnv(lpeEnv, "PBAT_IS_SCRIPT", "true");

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

    pBat_RunBatch(&ifIn);
}


int pBat_RunExternalBatch(char* lpFileName, char* lpFullLine, char** lpArguments)
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
        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                              __FILE__ "/pBat_RunExternalBatch()", -1);

    size = 0;
    while (lpArguments[size]) {

        if ((arg->lpArguments[size] = strdup(lpArguments[size])) == NULL)
            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                              __FILE__ "/pBat_RunExternalBatch()", -1);

        size ++;

    }

    arg->lpArguments[size] = NULL;

    th = pBat_CloneInstance((void(*)(void*))pBat_LaunchExternalBatch, arg);

    if (pBat_WaitForThread(&th, &ret) != 0)
        pBat_CloseThread(&th);

    return (int)ret;
}

/* Refactors a PARSED_LINE that contains a lookAHead command.

   PARSED lines are originally parsed not taking accound of
   lookAheads, that is you can have something like this :

   if 1 == 1 smthing & echo something else

   Parsed into:
                        &
   "if 1 == 1 smthing" ==> "echo something else"

   However, both the if and for commands (the only current lookAHeads)
   swallow every command bits to the left of them. So when the command
   is executed, the PARSED_LINE* must be refactored to look like this :

              &
   "smthing" ==> "echo something else"

   that will either be executed later or not depending on the
   command.

   if lookahead is NULL a new PARSED_LINE will be malloc'd */
PARSED_LINE* pBat_LookAHeadMakeParsedLine(BLOCKINFO* block, PARSED_LINE* lookahead)
{
	ESTR* cmd = pBat_EsInit();

    if (!lookahead) {
        if (!(lookahead = malloc(sizeof(PARSED_LINE)))) {

            pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
                                    __FILE__ "/pBat_LookAHeadMakeParsedLine()",
                                    -1);
            return NULL;

        } else {

            lookahead->sStream = NULL;
            lookahead->lppsNode = NULL;
            lookahead->cNodeType = 0;
            lookahead->lpCmdLine = NULL;

        }
    }

    pBat_EsCpy(cmd, "(");
	pBat_EsCatN(cmd, block->lpBegin, block->lpEnd - block->lpBegin);
    pBat_EsCat(cmd, ")");

    if (lookahead->lpCmdLine)
        pBat_EsFree(lookahead->lpCmdLine);

    lookahead->lpCmdLine = cmd;

    return lookahead;
}

#ifndef WIN32
#include "../command/pBat_Ask.h"
/* Unix is more sympathetic to us than windows. Indeed,
   Posix thread guaranties a signal handler to be ran
   by any of the existing threads. This means that
   every thread-local data is initialized and that we
   won't have any problem for the streams but we may
   have if we use allocated structures like ENVBUF* */
void pBat_SigHandlerBreak(int sig)
{
    int choice, i;
    ESTR* attr;

    /* using bIsScript is safe because it is inherited on a
       call to pBat_CloneInstance() */
    if (bIsScript) {

        choice = pBat_AskConfirmation(PBAT_ASK_YN
                                | PBAT_ASK_INVALID_REASK
                                | PBAT_ASK_DEFAULT_N, NULL,
                                lpBreakConfirm);

        if (choice == PBAT_ASK_NO)
            return;

    }

    attr = pBat_EsInit();

    pBat_EsCpy(attr, lppBatExec);
    pBat_EsCpy(attr, "/a:q");

    if (!bEchoOn)
        pBat_EsCat(attr, "e");
    if (bDelayedExpansion)
        pBat_EsCat(attr, "v");
    if (bCmdlyCorrect)
        pBat_EsCat(attr, "c");

    chdir(lpCurrentDir);
    pBat_ApplyEnv(lpeEnv);

    execl(lppBatExec, lppBatExec, attr->str);
}

#elif defined WIN32
#include "../command/pBat_Ask.h"
/* This is getting increasingly complicated since the
   multi-threaded version. Before, it was kind of simple
   since we had the correspondence 1 interpreter = one process
   and when a signal was raised a new thread was launched
   on the same process.

   This golden time is somehow now over since we have
   several threads inside.

   The biggest problem we have here, is the fact we might
   start pBat from *scratch* due to the difficulty to
   retrieve environment variables that are now stored
   thread-local and the fact that Windows calls this
   handler within a brand-new thread (with nothing
   initialized of course...).

   Beware not to refer to any of fInput, fOutput, and
   whatever thread-local here. */
BOOL WINAPI pBat_SigHandler(DWORD dwCtrlType)
{
    int choice, i;
    HANDLE thread;
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
                pBat_ShowErrorMessage(PBAT_BREAK_ERROR, NULL, -1);

            /* suspend the main thread */
            SuspendThread(thread);

            if (bIsScript) {
                /* we are running a script, so give two options : either
                   continuing or killing pBat */

                fputs(PBAT_NL, stderr);
                fprintf(stderr, "%s %s ", lpBreakConfirm, lpAskyN);

                args = pBat_EsInit();

                choice = PBAT_ASK_NO;

                while (pBat_EsGet(args, stdin)) {

                    pBat_RmTrailingNl(args->str);

                    if (!stricmp(args->str, lpAskYes)
                        || !stricmp(args->str, lpAskYesA)) {

                        choice = PBAT_ASK_YES;
                        break;

                    } else if (!stricmp(args->str, lpAskNo)
                               || !stricmp(args->str, lpAskNoA))
                        break;

                }

                if (choice == PBAT_ASK_NO) {

                    ResumeThread(thread);
                    CloseHandle(thread);
                    return TRUE;

                }
            }

            /* Kill the main thread right now */
            TerminateThread(thread, -1);
            CloseHandle(thread);

            /* Odds are that some pbat internal structures may
               have been corrupted by the somehow brutal kill of the
               main process. As the user *is* likely running interactive
               command, (he is indeed to trigger CTRL-C), do not bother
               that much and simply restart a new pBat command prompt. This
               implies a little performance penalty, but ... */

            args = pBat_EsInit();

            pBat_EsCpy(args, "\"");
            pBat_EsCat(args, lppBatExec);
            pBat_EsCat(args, "\"");
            pBat_EsCat(args, " /a:q");

            if (!bEchoOn)
                pBat_EsCat(args, "e");
            if (bDelayedExpansion)
                pBat_EsCat(args, "v");
            if (bCmdlyCorrect)
                pBat_EsCat(args, "c");

            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            ZeroMemory( &pi, sizeof(pi) );

            /* Use create process rather than spawn in order to break
               inheritance of probably broken stuff like fds */
            if( !CreateProcess( lppBatExec,
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
                   unmaintainability's sake, but, not to refer to all the
                   thread local defined stuff */

                pBat_SetConsoleTextColor(stderr, PBAT_BACKGROUND_DEFAULT | PBAT_FOREGROUND_IRED);

                fprintf(stderr, lpErrorMsg[PBAT_BREAK_ERROR]);

                pBat_SetConsoleTextColor(stderr, PBAT_COLOR_DEFAULT);

                fputs(PBAT_NL, stderr);
                fprintf(stderr, lpQuitMessage);
                pBat_Getch(stderr);


                exit(-1);

            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            exit(0);

    }

	return TRUE;
}

#endif // _POSIX_C_SOURCE
