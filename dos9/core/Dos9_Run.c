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

#ifdef _POSIX_C_SOURCE
    #include <sys/wait.h>
#endif

#include "Dos9_Core.h"

// #define DOS9_DBG_MODE

#include "Dos9_Debug.h"

#include "../command/Dos9_CmdLib.h"
#include "../errors/Dos9_Errors.h"


int Dos9_RunBatch(int isScript)
{
    PARSED_STREAM_START* lppssStreamStart;
    PARSED_STREAM* lppsStream;
    while (!Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_ISEOF, NULL, NULL))
    {

        DOS9_DBG("[*] %d : Parsing new line\n", __LINE__);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_NEWLINE, NULL, NULL);

        lppssStreamStart=(PARSED_STREAM_START*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_READ_LINE_PARSE, NULL, (void*)isScript);

        if (!lppssStreamStart) {
            DOS9_DBG("!!! Can't parse line : %s\n", strerror(errno));
            continue;
        }

        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_START_EXEC, lppssStreamStart, NULL);
        DOS9_DBG("\t[*] Global streams set\n.");


        lppsStream=lppssStreamStart->lppsStream;


        do {
            Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PIPE_OPEN, lppsStream, NULL);
            Dos9_RunCommand(lppsStream->lpCmdLine);
        } while ((lppsStream=(PARSED_STREAM*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_LINE_EXEC, lppsStream, NULL)));

        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_FREE_PARSED_LINE, lppssStreamStart, NULL);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_ENDLINE, NULL, NULL);

        DOS9_DBG("\t[*] Line run.\n");

    }
    return 0;
}

int Dos9_RunLine(ESTR* lpLine)
{
        PARSED_STREAM_START* lppssStreamStart;
        PARSED_STREAM* lppsStream;

        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_NEWLINE, NULL, NULL);
        lppssStreamStart=(PARSED_STREAM_START*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSE_ESTR, lpLine, NULL);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_START_EXEC, lppssStreamStart, NULL);
        DEBUG("Parsing succeded");
        lppsStream=lppssStreamStart->lppsStream;
        DEBUG("Starting command loop !");
        do {
            Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PIPE_OPEN, lppsStream, NULL);
            Dos9_RunCommand(lppsStream->lpCmdLine);
        } while ((lppsStream=(PARSED_STREAM*)Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_PARSED_LINE_EXEC, lppsStream, NULL)));
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_FREE_PARSED_LINE, lppssStreamStart, NULL);
        Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_ENDLINE, NULL, NULL);

        return 0;
}

int Dos9_RunCommand(ESTR* lpCommand)
{

    int (*lpProc)(char*);
    char lpErrorlevel[]="ERRORLEVEL=-3000000000";
    static int lastErrorLevel=0;
    char *lpCmdLine;
    int iFlag;

    lpCmdLine=Dos9_EsToChar(lpCommand);
    Dos9_RemoveEscapeChar(lpCmdLine);

    while (*lpCmdLine==' '
           || *lpCmdLine=='\t'
           || *lpCmdLine=='@') lpCmdLine++;

    if (*lpCmdLine==':')
        return 0;

    DOS9_DBG("*** Running  line '%s'\n", lpCmdLine);

    switch((iFlag=Dos9_GetCommandProc(lpCmdLine, lpclCommands, (void**)&lpProc)))
    {

       case -1:
            Dos9_BackTrackExternalCommand:
            Dos9_RunExternalCommand(lpCmdLine);
            break;

       default:
            if (iFlag
                && lpCmdLine[iFlag]!=' '
                && lpCmdLine[iFlag]!='\t'
                && lpCmdLine[iFlag]!='\0')
                goto Dos9_BackTrackExternalCommand;

            lpProc(lpCmdLine);

    }

    if (iErrorLevel!=lastErrorLevel) {

        snprintf(lpErrorlevel+11, sizeof(lpErrorlevel)-11, "%d", iErrorLevel);
        Dos9_PutEnv(lpErrorlevel);
        lastErrorLevel=iErrorLevel;
    }

    return 0;
}


int Dos9_RunBlock(BLOCKINFO* lpbkInfo)
{
    ESTR*  lpEsLine=Dos9_EsInit();

    char *lpBeginToken;
    char *lpToken = lpbkInfo->lpBegin;
    char *lpEnd = lpbkInfo->lpEnd;

    int iParentheseNb=0;
    int iOldState;

    size_t iSize;

    /* Save old lock state and lock the
       level, definitely */
    iOldState=Dos9_GetStreamStackLockState(lppsStreamStack);
    Dos9_SetStreamStackLockState(lppsStreamStack, TRUE);

    while (*lpToken && (lpToken < lpEnd)) {

        while (*lpToken==' ' || *lpToken=='\t') lpToken++;

        lpBeginToken=lpToken;

        for (;*lpToken && ((*lpToken!='\n' && *lpToken!=')') || iParentheseNb);lpToken++) {
            switch (*lpToken) {
                case '^':
                    lpToken++;
                case '(':
                    iParentheseNb++;
                    break;
                case  ')':
                    if (iParentheseNb>0) iParentheseNb--;
                    break;
            }
        }

        if (lpToken==lpBeginToken)  {

            if (*lpToken) lpToken++;
            continue;

        }

        if (*lpToken) {

            /* Do not modify the value of the string passed
               as argument, just take the character that interest
               you */
            iSize = lpToken - lpBeginToken;

            Dos9_EsCpyN(lpEsLine, lpBeginToken, iSize);
            Dos9_RunLine(lpEsLine);

            lpToken++;

        } else {

            Dos9_EsCpy(lpEsLine, lpBeginToken);
            Dos9_RunLine(lpEsLine);

        }

    }


    /* releases the lock */
    Dos9_SetStreamStackLockState(lppsStreamStack, iOldState);

    Dos9_EsFree(lpEsLine);

    return 0;
}

int Dos9_RunExternalCommand(char* lpCommandLine)
{

    char* lpArguments[FILENAME_MAX];
    ESTR* lpEstr[FILENAME_MAX];
    int i=0;
#ifdef _POSIX_C_SOURCE
    pid_t iPid;
#endif


    Dos9_GetParamArrayEs(lpCommandLine, lpEstr, FILENAME_MAX);

    if (lpEstr[0]) Dos9_EsReplace(lpEstr[0], "\"", "");

    for (;lpEstr[i];i++) {
        lpArguments[i]=Dos9_EsToChar(lpEstr[i]);
    }

    lpArguments[i]=0;

    errno=0;

#ifdef WIN32

    /* in windows the result is directly returned */
    iErrorLevel=spawnvp(_P_WAIT, lpArguments[0], (DOS9_SPAWN_CAST)lpArguments);

    if (errno==ENOENT)
    {

        Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpArguments[0] , 0);

    }

#elif defined _POSIX_C_SOURCE

    iPid=fork();
    if (iPid == 0 ) {
      /* if we are in the son */

      if ( execvp(lpArguments[0], lpArguments) == -1) {

        Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpArguments[0], 0);

      }

      exit(0);

    } else {
      /* if we are in the father */

      if (iPid == (pid_t)-1) {
        /* the execution failed */
        Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpArguments[0], 0);
      } else {
        wait(&iErrorLevel);
      }

    }

#endif

    for (i=0;lpEstr[i];i++) {
        Dos9_EsFree(lpEstr[i]);
    }

    DEBUG("Executable ran");
    return 0;

}
