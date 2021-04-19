/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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


#if defined WIN32
#include <conio.h>
#endif

#include <libDos9.h>

#include "../errors/Dos9_Errors.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

//#define DOS9_DBG_MODE

#include "../core/Dos9_Core.h"
#include "../core/Dos9_Debug.h"
#include "../command/Dos9_Commands.h"

#include "../../config.h"
#include <gettext.h>

void Dos9_AssignCommandLine(int c, char** argv)
{
    ESTR *lpEsStr=Dos9_EsInit(),
          *lpEsParam=Dos9_EsInit();

    /* int i; */

    char*  delims=" ,;=\t&|";

    while (*argv) {

        Dos9_EsCpy(lpEsParam, *argv);
        Dos9_EsReplace(lpEsParam, "\"", "^\""); /* escape parenthesis */

        if (strpbrk(*argv, delims)) {

            Dos9_EsCat(lpEsStr, "\"");
            Dos9_EsCatE(lpEsStr, lpEsParam);
            Dos9_EsCat(lpEsStr, "\" ");

        } else {

            Dos9_EsCatE(lpEsStr, lpEsParam);
            Dos9_EsCat(lpEsStr, " ");

        }

        argv ++;

    }

    Dos9_SetLocalVar(lpvArguments, c, Dos9_EsToChar(lpEsStr));

    Dos9_EsFree(lpEsStr);
    Dos9_EsFree(lpEsParam);
}

void Dos9_DuplicateStdStreams(void)
{
    int newInput, newOutput, newErr; /* descriptors to duplicate std streams */

    if ((newInput = dup(DOS9_STDIN)) == -1
        || (newOutput = dup(DOS9_STDOUT)) == -1
        || (newErr = dup(DOS9_STDERR)) == -1) {

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

void Dos9_Init(void)
{
    char lpSharePath[FILENAME_MAX];

    /* Initialize Dos9 specific environment */
    lpeEnv = Dos9_InitEnv(environ);

    /* Start libDos9 */
    if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);

    }

    /* Fixme : This is probably useless now */
    Dos9_SetNewLineMode(DOS9_NEWLINE_UNIX);

    /* Allocate locale blocks for special vars & arguments */
    lpvLocalVars=Dos9_GetLocalBlock();
    lpvArguments=Dos9_GetLocalBlock();

    /* Initialize the console features of Dos9 */
    Dos9_InitConsole();

    /* Initialize the completion features */
    Dos9_InitCompletion();

    /* Set the current directory of the current thread */
    getcwd(lpCurrentDir, FILENAME_MAX);

    /* Fetch path of the current directory */
    Dos9_GetExePath(lpDos9Path, FILENAME_MAX);
    Dos9_GetExeFilename(lpDos9Exec, FILENAME_MAX);

    /* Set the current locale for gettext to be able what language
       should be used */
    setlocale(LC_ALL, "");

    snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lpDos9Path);
    Dos9_LoadLocaleMessages(lpSharePath);

    /* Only use default LC_NUMERIC after loading messages
       not to mess with gettext messages loading

       Setting LC_NUMERIC to C enables to have strings numbers
       written like 1.1, regardless of the chosen language. For
       Example if LC_NUMERIC is french, then 1.1 becomes 1,1 */
    setlocale(LC_NUMERIC, "C");



    /* Set the value of %DOS9_PATH% */
    lpInitVar[4]="DOS9_PATH";
    lpInitVar[5]=lpDos9Path;

    /* Set the default Dos9 vars */
    Dos9_InitVar(lpInitVar);

    lppsStreamStack=Dos9_InitStreamStack();
}

int Dos9_InitSetModes(char* str)
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

            case 'F':
                /* enables floats */
                bUseFloats=TRUE;
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

#ifdef DOS9_USE_LIBCU8
#define LIBCU8_CONFIG "use-libcu8"
#else
#define LIBCU8_CONFIG "no-libcu8"
#endif // DOS9_USE_LIBCU8

#ifdef DOS9_USE_MODULES
#define MODULE_CONFIG "use-modules"
#else
#define MODULE_CONFIG "no-modules"
#endif // DOS9_USE_MODULES

#ifdef LIBDOS9_NO_CONSOLE
#define CONSOLE_CONFIG "no-console"
#else
#define CONSOLE_CONFIG "use-console"
#endif // LIBDOS9_NO_CONSOLE

#ifdef DOS9_NO_LINENOISE
#define LINENOISE_CONFIG "no-linenoise"
#else
#define LINENOISE_CONFIG "use-linenoise"
#endif // DOS9_NO_LINENOISE

#ifdef DOS9_STATIC_CMDLYCORRECT
#define CMDLYCORRECT_CONFIG "use-cmdlycorrect"
#else
#define CMDLYCORRECT_CONFIG "no-cmdlycorrect"
#endif // DOS9_STATIC_CMDLYCORRECT

void Dos9_InitHelp(void)
{
    /* Here it not mandatory to use the DOS9_NL macro to produce OS-independent
       line terminators, since the stdout file descriptor is text
       oriented when this portion of code is to be executed */

    fputs("DOS9 [" DOS9_VERSION "] (" DOS9_HOST ") - " DOS9_BUILDDATE "\n"
         "Copyright (c) 2010-" DOS9_BUILDYEAR " " DOS9_AUTHORS "\n", fError);
    fputs("config : " NLS_CONFIG " " MODULE_CONFIG " " LIBCU8_CONFIG " " LINENOISE_CONFIG " "
          CONSOLE_CONFIG " " CMDLYCORRECT_CONFIG "\n\n", fError);

    fputs(lpHlpMain, fError);

    exit(0);
}

char* Dos9_GetParameters(char** argv, char** lpFileName, int* bExitAfterCmd, int*
                            bQuiet)
{
    int i, c = '0', j, bGetSwitch = 1;
    //int iInputD, iOutputD;
    char* lpCmdCSwitch = NULL;

    ESTR* lpCmdC;

    if (!argv[0])
        Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, "Dos9", -1);

    /* get command line arguments */
    for (i=1; argv[i]; i++) {

        if (*argv[i]=='/' && bGetSwitch) {

            argv[i]++;

            switch(toupper(*argv[i])) {

                case 'A':
                    *bQuiet = Dos9_InitSetModes(argv[i]);
                    break;

                case 'K':
                    *bExitAfterCmd = FALSE;
                case 'C':
                    if (lpCmdCSwitch != NULL || argv[++i] == NULL)
                        Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, "", -1);

                    /* introduce new semantics with /C or /K command, if only
                       @ is passed as argument, then /C takes the command line
                       from the variable %__DOS9_COMMAND__% */
                    if (*(argv[i]) == '@' && *(argv[i]+1)=='\0') {

                        lpCmdCSwitch = getenv("__DOS9_COMMAND__");

                        lpCmdCSwitch = (lpCmdCSwitch != NULL)
                                                    ? lpCmdCSwitch : "";

                    } else {

                        /* allow a tiny memory leak, since i cannot see how
                           to handle it otherwise */
                        lpCmdC = Dos9_EsInit();

                        do {

                            if (strpbrk(argv[i], " \t\n")) {
                                Dos9_EsCat(lpCmdC, "\"");
                                Dos9_EsCat(lpCmdC,  argv[i]);
                                Dos9_EsCat(lpCmdC, "\"");
                            } else {
                                Dos9_EsCat(lpCmdC,  argv[i]);
                            }

                            Dos9_EsCat(lpCmdC, " ");

                        } while ((argv[i+1] != NULL) && i++);

                        lpCmdCSwitch = lpCmdC->str;
                    }

                    break;

                case '?':
                    Dos9_InitHelp();
                    return 0;

                case '/' :
                    /* there is no more switch on the command line.
                       '' */
                    bGetSwitch=FALSE;
                    break;

                default:
                    Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, -1);

            }

        } else {

            if (**lpFileName != '\0') {

                /* set parameters for the file currently ran */
                for (j=i ,  c='1'; argv[j] && c<='9'; i++, c++ , j++ ) {

                    Dos9_SetLocalVar(lpvArguments, c, argv[j]);

                }

                Dos9_AssignCommandLine('+', argv + j);

                break;
            }

            *lpFileName=argv[i];
            c='1';

            Dos9_AssignCommandLine('*', argv + i + 1);
            Dos9_SetLocalVar(lpvArguments, '+', "");

            bGetSwitch = FALSE;
        }

    }

    /* empty remaining special vars */
    for (; c<='9'; c++)
        Dos9_SetLocalVar(lpvArguments, c , "");

    return lpCmdCSwitch;
}


void Dos9_InitConsoleTitles(char *lpFileName, int bQuiet)
{
    char lpTitle[FILENAME_MAX]="Dos9 [" DOS9_VERSION "] - ";

    if (*lpFileName=='\0') {

        if (!bQuiet)
            Dos9_SetEnv(lpeEnv, "DOS9_IS_SCRIPT", "false");
        else
            Dos9_SetEnv(lpeEnv, "DOS9_IS_SCRIPT", "true");

        strcat(lpTitle, "Command prompt");
        bIsScript = 0;

    } else {

        strncat(lpTitle, lpFileName, sizeof(lpTitle)-sizeof("Dos9 [" DOS9_VERSION "] - "));
        Dos9_SetEnv(lpeEnv, "DOS9_IS_SCRIPT", "true");
        bIsScript = 1;

        DOS9_DBG("[dos9] Runing \"%s\"\n", lpFileName);

    }

    if (!bQuiet) {

        Dos9_SetConsoleTextColor(fOutput, DOS9_COLOR_DEFAULT);
        Dos9_SetConsoleTitle(fOutput, lpTitle);

    }
}


void Dos9_RunAutoBat(void)
{
    char system[FILENAME_MAX];

    DOS9_DBG("Running Dos9_Auto.bat\n");

    snprintf(system, sizeof(system),"%s/Dos9_Auto.bat", Dos9_GetEnv(lpeEnv, "DOS9_PATH"));

    if (!Dos9_FileExists(system))
        return;

    strcpy(ifIn.lpFileName, system);

    ifIn.iPos = 0;
    ifIn.bEof = 0;
    *(ifIn.batch.name) = '\0';

    /* Run the auto configuration file */
    Dos9_RunBatch(&ifIn);

}
