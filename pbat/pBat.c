/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2014 Romain GARBI
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
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <signal.h>


#if defined WIN32
#include <conio.h>
#endif

#include <libpBat.h>

#include "init/pBat_Init.h"

#include "errors/pBat_Errors.h"

#include "lang/pBat_Lang.h"
#include "lang/pBat_ShowHelp.h"

#include "core/pBat_Core.h"

#if defined(WIN32) && defined(PBAT_USE_LIBCU8)
#include <libcu8.h>
#endif

#include "command/pBat_Commands.h"
#include "command/pBat_CommandInfo.h"

//#define PBAT_DBG_MODE
#include "core/pBat_Debug.h"

#include "../config.h"

#ifdef WIN32
BOOL WINAPI pBat_BreakIgn(DWORD dwCtrlType)
{
    return TRUE;
}
#endif

int main(int argc, char *argv[])
{
    /*  a function which initializes pBat's core,
        parses the command line argumments,
        And display starting message
    */

    char *lpFileName="",
          lpFileAbs[FILENAME_MAX]="",
         *lpCmdCSwitch=NULL;


    int bQuiet=FALSE,
        bExitAfterCmd=TRUE;

    ESTR* lpesCmd;

#if defined(WIN32) && defined(PBAT_USE_LIBCU8)

    if (libcu8_init(&argv) == -1) {

        fprintf(stderr, "Unable to load libcu8...\n");
        return -1;

    }
#endif

#if !defined(WIN32)

    /* Sets the default signal handler */

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler=pBat_SigHandlerBreak;
    action.sa_flags=SA_NODEFER;
    sigaction(SIGINT, &action, NULL);

#else

    /* Retrieve the main thread id */
    iMainThreadId = GetCurrentThreadId();

    /* Stop ignoring CTRL-C Event */
    SetConsoleCtrlHandler(NULL, FALSE);

    /* Set default signal handler */
    SetConsoleCtrlHandler(pBat_SigHandler, TRUE);
#endif

    /* Ignore the SIGFPS signal */
    signal(SIGFPE, SIG_IGN);

#if defined(SIGPIPE)
    /* If the OS provide some SIGPIPE, just ignore them, theses error are not
     * relevant to us anyway as it refers to a somehow broken pipe that do not
     * have a read end. That is, basically, the end does not care about any
     * kind of input. */
    signal(SIGPIPE, SIG_IGN);
#endif /* SIGPIPE */

    /* Initialize pBat Internals*/
    pBat_Init();

    /* Set the error handler called on errors that require the process to exit */
    pErrorHandler=pBat_Exit;

    /* duplicate std streams */
    pBat_DuplicateStdStreams();
    pBat_CreateMutex(&mThreadLock);
    pBat_CreateMutex(&mSetLock);
    pBat_CreateMutex(&mRunFile);
    pBat_CreateMutex(&mEchoLock);
    pBat_CreateMutex(&mModLock);

#ifndef WIN32
    pBat_CreateMutex(&mLineNoise);
#endif

    /* Process the command line, and set the appropriate special variables for
     * special parameters. (note %0 is set when the absolute path of the script
     * is computed) */
    lpCmdCSwitch = pBat_GetParameters(argv, &lpFileName, &bExitAfterCmd, &bQuiet);

    /* Initialize stdlib random */
    srand(time(NULL));

    /* Set the console title depending on the type of actions performed (ie.
     * command prompt or batch interpretor) and set the PBAT_IS_SCRIPT var
     * appropriately */
    pBat_InitConsoleTitles(lpFileName, bQuiet);

    /* Generates the absolute path of the script and sets the special variable
     * %0 accordingly */
    if (*lpFileName!='\0') {

        /* generates real path if the path is uncomplete */
        if (pBat_GetFilePath(lpFileAbs, lpFileName, sizeof(lpFileAbs))==-1)
            pBat_ShowErrorMessage(PBAT_FILE_ERROR, lpFileName, -1);

        pBat_SetLocalVar(lpvArguments, '0', lpFileAbs);

    } else {

        pBat_SetLocalVar(lpvArguments, '0', "");

    }

    /* Map the commandinfo */
    lpclCommands = pBat_MapCommandInfo(lpCmdInfo,
                                iCmdInfoNb);

    /* running auto batch initialisation */
    pBat_RunAutoBat();

    if (lpCmdCSwitch != NULL) {

        lpesCmd=pBat_EsInit_Cached();

        pBat_EsCpy(lpesCmd, lpCmdCSwitch);
        pBat_RunLine(lpesCmd);

        if (bExitAfterCmd == TRUE)
            goto skip;
    }

    /* Set parameters for the file */
    strcpy(ifIn.lpFileName, lpFileAbs);
    ifIn.iPos=0;
    ifIn.bEof=FALSE;

    *(ifIn.batch.name) = '\0';

    /* Run either the command prompt or the batch script */
    pBat_RunBatch(&ifIn);

skip:

    pBat_Exit();

    pBat_LibClose();

    return 0;

}
