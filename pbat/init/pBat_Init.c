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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <signal.h>

#include "../../config.h"

#if defined WIN32
#include <conio.h>
#endif

#include <libpBat.h>

#include "../errors/pBat_Errors.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

//#define PBAT_DBG_MODE

#include "../core/pBat_Core.h"
#include "../core/pBat_Debug.h"
#include "../command/pBat_Commands.h"

#include <gettext.h>

void pBat_AssignCommandLine(int c, char** argv)
{
    ESTR *lpEsStr=pBat_EsInit_Cached(),
          *lpEsParam=pBat_EsInit_Cached();

    /* int i; */

    char*  delims=" ,;=\t&|";

    while (*argv) {

        pBat_EsCpy(lpEsParam, *argv);
        pBat_EsReplace(lpEsParam, "\"", "^\""); /* escape parenthesis */

        if (strpbrk(*argv, delims)) {

            pBat_EsCat(lpEsStr, "\"");
            pBat_EsCatE(lpEsStr, lpEsParam);
            pBat_EsCat(lpEsStr, "\" ");

        } else {

            pBat_EsCatE(lpEsStr, lpEsParam);
            pBat_EsCat(lpEsStr, " ");

        }

        argv ++;

    }

    pBat_SetLocalVar(lpvArguments, c, pBat_EsToChar(lpEsStr));

    pBat_EsFree_Cached(lpEsStr);
    pBat_EsFree_Cached(lpEsParam);
}

void pBat_DuplicateStdStreams(void)
{
    int newInput, newOutput, newErr; /* descriptors to duplicate std streams */

    if ((newInput = dup(PBAT_STDIN)) == -1
        || (newOutput = dup(PBAT_STDOUT)) == -1
        || (newErr = dup(PBAT_STDERR)) == -1) {

        /* cowardly refuse to do anything if we failed to duplicate stream */
        fputs("Error: Unable to duplicate current standard streams.\b", stderr);
        exit(-1);
    }

    fInput = fdopen(newInput, "r");
    _fOutput = fdopen(newOutput, "w");
    _fError = fdopen(newErr, "w");

    fOutput = _fOutput;
    fError = _fError;

    if (!fInput || !fOutput || !fError) {

        fputs("Error: Unable to load standard streams duplicates", stderr);
        exit(-1);

    }

    /* Re-set appropriate flags for the standard streams if they
       actually refer to a tty (no reason not to buffer files that
       are not referring to tty) */

    if (isatty(newOutput))
        setvbuf(fOutput, NULL, _IONBF, 0);

    if (isatty(newErr))
        setvbuf(fError, NULL, _IONBF, 0);
}

void pBat_Init(void)
{
    char lpSharePath[FILENAME_MAX];

    /* Initialize ESTR cache */
    pBat_EsCacheBuild(&ecEstrCache);

    /* Initialize pBat specific environment */
    lpeEnv = pBat_InitEnv(environ);

    /* Start libpBat */
    if (pBat_LibInit() == -1) {

        puts("Error : Unable to load LibpBat. Exiting ...");
        exit(-1);

    }

    /* Fixme : This is probably useless now */
    pBat_SetNewLineMode(PBAT_NEWLINE_UNIX);

    /* Allocate locale blocks for special vars & arguments */
    lpvLocalVars=pBat_GetLocalBlock();
    lpvArguments=pBat_GetLocalBlock();

    /* Initialize the console features of pBat */
    pBat_InitConsole();

    /* Initialize the completion features */
    pBat_InitCompletion();

    /* Set the current directory of the current thread */
    getcwd(lpCurrentDir, FILENAME_MAX);

    /* Fetch path of the current directory */
    pBat_GetExePath(lppBatPath, FILENAME_MAX);
    pBat_GetExeFilename(lppBatExec, FILENAME_MAX);

    /* Set the current locale for gettext to be able what language
       should be used */
    setlocale(LC_ALL, "");

    snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lppBatPath);
    pBat_LoadLocaleMessages(lpSharePath);

    /* Only use default LC_NUMERIC after loading messages
       not to mess with gettext messages loading

       Setting LC_NUMERIC to C enables to have strings numbers
       written like 1.1, regardless of the chosen language. For
       Example if LC_NUMERIC is french, then 1.1 becomes 1,1 */
    setlocale(LC_NUMERIC, "C");

    /* Set the value of %PBAT_PATH% */
    lpInitVar[4]="PBAT_PATH";
    lpInitVar[5]=lppBatPath;

#ifdef WIN32
    char drive_var[] = "=x:";

    /* initialize the current %=x:% drive variable with current dir */
    drive_var[1] = *lpCurrentDir;
    lpInitVar[6] = drive_var;
    lpInitVar[7] = lpCurrentDir;

#endif // WIN32

    /* Set the default pBat vars */
    pBat_InitVar(lpInitVar);

    lppsStreamStack=pBat_InitStreamStack();

}

int pBat_InitSetModes(char* str)
{
    int bQuiet = 0;

    if (*(str + 1) == ':')
        str ++;

    while (*(str ++)) {
        switch (toupper(*str)) {

            case 'V':
                /* enables delayed expansion */
                bDelayedExpansion=TRUE;
                break;

            case 'E':
                bEchoOn=FALSE;
                break;

            case 'C':
                /* enable cmd-compatible mode */
                bCmdlyCorrect=TRUE;
                break;

            case 'Q':
                bQuiet=TRUE; // run silently
                break;

        }

    }

    return bQuiet;
}

#ifdef ENABLE_NLS
#define NLS_CONFIG "use-nls"
#else
#define NLS_CONFIG "no-nls"
#endif // ENABLE_NLS

#ifdef PBAT_USE_LIBCU8
#define LIBCU8_CONFIG "use-libcu8"
#else
#define LIBCU8_CONFIG "no-libcu8"
#endif // PBAT_USE_LIBCU8

#ifdef PBAT_USE_MODULES
#define MODULE_CONFIG "use-modules"
#else
#define MODULE_CONFIG "no-modules"
#endif // PBAT_USE_MODULES

#ifdef LIBPBAT_NO_CONSOLE
#define CONSOLE_CONFIG "no-console"
#else
#define CONSOLE_CONFIG "use-console"
#endif // LIBPBAT_NO_CONSOLE

#ifdef PBAT_NO_LINENOISE
#define LINENOISE_CONFIG "no-linenoise"
#else
#define LINENOISE_CONFIG "use-linenoise"
#endif // PBAT_NO_LINENOISE

#ifdef PBAT_STATIC_CMDLYCORRECT
#define CMDLYCORRECT_CONFIG "use-cmdlycorrect"
#else
#define CMDLYCORRECT_CONFIG "no-cmdlycorrect"
#endif // PBAT_STATIC_CMDLYCORRECT

void pBat_InitHelp(void)
{
    /* Here it not mandatory to use the PBAT_NL macro to produce OS-independent
       line terminators, since the stdout file descriptor is text
       oriented when this portion of code is to be executed */

    fputs("PBAT [" PBAT_VERSION "] (" PBAT_HOST ") - " PBAT_BUILDDATE "\n"
         "Copyright (c) 2010-" PBAT_BUILDYEAR " " PBAT_AUTHORS "\n", fError);
    fputs("config : " NLS_CONFIG " " MODULE_CONFIG " " LIBCU8_CONFIG " " LINENOISE_CONFIG " "
          CONSOLE_CONFIG " " CMDLYCORRECT_CONFIG "\n\n", fError);

    fputs(lpHlpMain, fError);

    exit(0);
}

char* pBat_GetParameters(char** argv, char** lpFileName, int* bExitAfterCmd, int*
                            bQuiet)
{
    int i, c = '0', j, bGetSwitch = 1;
    //int iInputD, iOutputD;
    char* lpCmdCSwitch = NULL;

    ESTR* lpCmdC;

    if (!argv[0])
        pBat_ShowErrorMessage(PBAT_BAD_COMMAND_LINE, "pBat", -1);

    /* get command line arguments */
    for (i=1; argv[i]; i++) {

        if (*argv[i]=='/' && bGetSwitch) {

            argv[i]++;

            switch(toupper(*argv[i])) {

                case 'A':
                    *bQuiet = pBat_InitSetModes(argv[i]);
                    break;

                case 'K':
                    *bExitAfterCmd = FALSE;
                case 'C':
                    if (lpCmdCSwitch != NULL || argv[++i] == NULL)
                        pBat_ShowErrorMessage(PBAT_BAD_COMMAND_LINE, "", -1);

                    /* introduce new semantics with /C or /K command, if only
                       @ is passed as argument, then /C takes the command line
                       from the variable %__PBAT_COMMAND__% */
                    if (*(argv[i]) == '@' && *(argv[i]+1)=='\0') {

                        lpCmdCSwitch = getenv("__PBAT_COMMAND__");

                        lpCmdCSwitch = (lpCmdCSwitch != NULL)
                                                    ? lpCmdCSwitch : "";

                    } else {

                        /* allow a tiny memory leak, since i cannot see how
                           to handle it otherwise */
                        lpCmdC = pBat_EsInit_Cached();

                        do {

                            if (strpbrk(argv[i], " \t\n")) {
                                pBat_EsCat(lpCmdC, "\"");
                                pBat_EsCat(lpCmdC,  argv[i]);
                                pBat_EsCat(lpCmdC, "\"");
                            } else {
                                pBat_EsCat(lpCmdC,  argv[i]);
                            }

                            pBat_EsCat(lpCmdC, " ");

                        } while ((argv[i+1] != NULL) && i++);

                        lpCmdCSwitch = lpCmdC->str;
                    }

                    break;

                case '?':
                    pBat_InitHelp();
                    return 0;

                case '/' :
                    /* there is no more switch on the command line.
                       '' */
                    bGetSwitch=FALSE;
                    break;

                default:
                    pBat_ShowErrorMessage(PBAT_BAD_COMMAND_LINE, NULL, -1);

            }

        } else {

            if (**lpFileName != '\0') {

                /* set parameters for the file currently ran */
                for (j=i ,  c='1'; argv[j] && c<='9'; i++, c++ , j++ ) {

                    pBat_SetLocalVar(lpvArguments, c, argv[j]);

                }

                pBat_AssignCommandLine('+', argv + j);

                break;
            }

            *lpFileName=argv[i];
            c='1';

            pBat_AssignCommandLine('*', argv + i + 1);
            pBat_SetLocalVar(lpvArguments, '+', "");

            bGetSwitch = FALSE;
        }

    }

    /* empty remaining special vars */
    for (; c<='9'; c++)
        pBat_SetLocalVar(lpvArguments, c , "");

    return lpCmdCSwitch;
}


void pBat_InitConsoleTitles(char *lpFileName, int bQuiet)
{
    char lpTitle[FILENAME_MAX]="pBat [" PBAT_VERSION "] - ";

    if (*lpFileName=='\0') {

        if (!bQuiet)
            pBat_SetEnv(lpeEnv, "PBAT_IS_SCRIPT", "false");
        else
            pBat_SetEnv(lpeEnv, "PBAT_IS_SCRIPT", "true");

        strcat(lpTitle, "Command prompt");
        bIsScript = 0;

    } else {

        strncat(lpTitle, lpFileName, sizeof(lpTitle)-sizeof("pBat [" PBAT_VERSION "] - "));
        pBat_SetEnv(lpeEnv, "PBAT_IS_SCRIPT", "true");
        bIsScript = 1;

        PBAT_DBG("[pbat] Runing \"%s\"\n", lpFileName);

    }

    if (!bQuiet) {

        pBat_SetConsoleTextColor(fOutput, PBAT_COLOR_DEFAULT);
        pBat_SetConsoleTitle(fOutput, lpTitle);

    }
}


void pBat_RunAutoBat(void)
{
    char system[FILENAME_MAX];

    PBAT_DBG("Running pBat_Auto.bat\n");

    snprintf(system, sizeof(system),"%s/pBat_Auto.bat", pBat_GetEnv(lpeEnv, "PBAT_PATH"));

    if (!pBat_FileExists(system))
        return;

    strcpy(ifIn.lpFileName, system);

    ifIn.iPos = 0;
    ifIn.bEof = 0;
    *(ifIn.batch.name) = '\0';

    /* Run the auto configuration file */
    pBat_RunBatch(&ifIn);

}
