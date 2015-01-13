/*
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

#include "errors/Dos9_Errors.h"

#include "lang/Dos9_Lang.h"
#include "lang/Dos9_ShowHelp.h"

#include "core/Dos9_Core.h"

/* those are codes of internal commands */

#include "command/Dos9_Commands.h"
#include "command/Dos9_CommandInfo.h"

//#define DOS9_DBG_MODE
#include "core/Dos9_Debug.h"

#include "../config.h"

void Dos9_SigHandler(int c)
{
	/* this is to prevent ctrl-C or SIGINT to close the
	   the command prompt. */
}


void Dos9_AssignCommandLine(char** argv)
{
    ESTR *lpEsStr=Dos9_EsInit(),
          *lpEsParam=Dos9_EsInit();

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

    Dos9_SetLocalVar(lpvLocalVars, '*', Dos9_EsToChar(lpEsStr));

    Dos9_EsFree(lpEsStr);
    Dos9_EsFree(lpEsParam);
}



int main(int argc, char *argv[])
{
	/*  a function which initializes Dos9's core,
	    parses the command line argumments,
	    And display starting message
	*/

	DOS9_DBG("Initializing Dos9's custom environ");

	char *lpFileName="",
	      lpFileAbs[FILENAME_MAX],
	      lpTmp[FILENAME_MAX],
	      lpExePath[FILENAME_MAX],
	      lpTitle[FILENAME_MAX]="Dos9 [" DOS9_VERSION "] - ",
		 *lpCmdCSwitch=NULL;


	int i,
	    j,
	    c='0',
	    bQuiet=FALSE,
	    bGetSwitch=TRUE,
	    bExitAfterCmd=TRUE;

	ESTR* lpesCmd;

    DOS9_DBG("Initializing Dos9's custom environ");
    lpeEnv = Dos9_InitEnv(environ);

	DOS9_DBG("Initializing signal handler...\n");
	signal(SIGINT, Dos9_SigHandler);

	DOS9_DBG("Initializing libDos9 ...\n");
	if (Dos9_LibInit() == -1) {

		puts("Error : Unable to load LibDos9. Exiting ...");
		exit(-1);
	}

	DOS9_DBG("Setting UNIX newlines ...\n");
	Dos9_SetNewLineMode(DOS9_NEWLINE_UNIX);

	DOS9_DBG("Allocating local variable block ... \n");
	lpvLocalVars=Dos9_GetLocalBlock();

	DOS9_DBG("Initializing console ...\n");
	Dos9_InitConsole();

	DOS9_DBG("Setting locale ...\n");
#ifdef WIN32

	SetThreadLocale(LOCALE_USER_DEFAULT);

#elif defined _POSIX_C_SOURCE

	setlocale(LC_ALL, "");

#endif // WINDOWS

	DOS9_DBG("Loading GETTEXT messages... \n");
	Dos9_LoadStrings();
	Dos9_LoadErrors();
	Dos9_LoadInternalHelp();

	DOS9_DBG("Loading current directory...\n");
	Dos9_UpdateCurrentDir();

	/* **********************************
	   *   getting Dos9's parameters    *
	   ********************************** */

	if (!argv[0])
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE,
		                      "Dos9",
		                      -1
		                     );



    DOS9_DBG("Getting current executable name ...\n");
	Dos9_GetExePath(lpExePath, FILENAME_MAX);

	DOS9_DBG("\tGot \"%s\" as name ...\n", lpTitle);
	lpInitVar[4]="DOS9_PATH";
	lpInitVar[5]=lpExePath;

	DOS9_DBG("Initializing variables ...\n");
	Dos9_InitVar(lpInitVar);
	Dos9_SetEnv(lpeEnv, "ERRORLEVEL","0");

	DOS9_DBG("Mapping commands ... \n");
	lpclCommands=Dos9_MapCommandInfo(lpCmdInfo, sizeof(lpCmdInfo)/sizeof(COMMANDINFO));

	DOS9_DBG("Initializing streams ... \n");
	lppsStreamStack=Dos9_InitStreamStack();

	pErrorHandler=Dos9_Exit;

    DOS9_DBG("Getting command line arguments ... \n");
	/* get command line arguments */
	for (i=1; argv[i]; i++) {

		DOS9_DBG("* Got \"%s\" as argument...\n", argv[i]);

		if (*argv[i]=='/' && bGetSwitch) {
			argv[i]++;
			switch(toupper(*argv[i])) {

				case 'A':
				if (*(argv[i]+1) == ':')
					argv[i]++;

				while (*(argv[i]++)) {
					switch (toupper(*argv[i])) {

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
							#if !defined(DOS9_STATIC_CMDLYCORRECT)
							bCmdlyCorrect=TRUE;
							#else
							Dos9_ShowErrorMessage(DOS9_UNABLE_SET_OPTION,
													"CMDLYCORRECT",
													FALSE);
							#endif
							break;

						case 'Q':
							bQuiet=TRUE; // run silently
							break;
					}

				}
				break;

				case 'I':
					if (!argv[++i]) {

						Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "Dos9", -1);

					}

					iInputD=atoi(argv[i]); // select input descriptor
					Dos9_OpenOutputD(lppsStreamStack, iInputD, DOS9_STDIN);
					break;

				case 'O':
					if (!argv[++i]) {

						Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "Dos9", -1);

					}

					iOutputD=atoi(argv[i]); // select input descriptor
					Dos9_OpenOutputD(lppsStreamStack, iOutputD, DOS9_STDOUT);
					break;

				case 'K':
					bExitAfterCmd = FALSE;
				case 'C':
					if (lpCmdCSwitch != NULL || argv[++i] == NULL)
						Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, -1);

					lpCmdCSwitch = argv[i];
					break;

				case '?':
					puts("DOS9 [" DOS9_VERSION "] (" DOS9_HOST ") - " DOS9_BUILDDATE "\n"
					     "Copyright (c) 2010-" DOS9_BUILDYEAR " " DOS9_AUTHORS "\n\n"
					     "This is free software, you can modify and/or redistribute it under "
					     "the terms of the GNU Genaral Public License v3 (or any later version).\n");

					puts(lpHlpMain);

					puts("Feel free to report bugs and submit suggestions at : <" DOS9_BUGREPORT ">\n"
						 "For more informations see : <" DOS9_URL ">");
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

			if (*lpFileName!='\0') {

				/* set parameters for the file currently runned */
				for (j=i ,  c='1'; argv[j] && c<='9'; i++, c++ , j++ ) {

					Dos9_SetLocalVar(lpvLocalVars, c, argv[j]);

				}

				break;
			}

			lpFileName=argv[i];
			Dos9_SetLocalVar(lpvLocalVars, '0', lpFileName);
			c='1';

			Dos9_AssignCommandLine(argv + i);

			bGetSwitch = FALSE;
		}

	}

	/* empty remaining special vars */
	for (; c<='9'; c++)
		Dos9_SetLocalVar(lpvLocalVars, c , "");

	srand(time(NULL));

	colColor=DOS9_COLOR_DEFAULT;
	/* messages affichés */


	DOS9_DBG("Setting introduction and DOS9_IS_SCRIPT ...\n");

	if (*lpFileName=='\0') {

		if (!bQuiet)
			Dos9_PrintIntroduction();

		strcat(lpTitle, "Command prompt");
		Dos9_SetEnv(lpeEnv, "DOS9_IS_SCRIPT", "false");

	} else {

		strncat(lpTitle, lpFileName, sizeof(lpTitle)-sizeof("Dos9 [" DOS9_VERSION "] - "));
		Dos9_SetEnv(lpeEnv, "DOS9_IS_SCRIPT","true");

		DOS9_DBG("[dos9] Runing \"%s\"\n", lpFileName);

	}

	if (!bQuiet) {

		Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
		Dos9_SetConsoleTitle(lpTitle);

	}

	/* running auto batch initialisation */

	strcat(lpExePath, "/Dos9_Auto.bat");

	strcpy(ifIn.lpFileName, lpExePath);
	ifIn.iPos=0;
	ifIn.bEof=FALSE;

	DOS9_DBG("Running file \"%s\"\n", ifIn.lpFileName);

	Dos9_RunBatch(&ifIn);

	DOS9_DBG("\tRan\n");

	if (lpCmdCSwitch != NULL) {

		lpesCmd=Dos9_EsInit();

		Dos9_EsCpy(lpesCmd, lpCmdCSwitch);
		Dos9_RunLine(lpesCmd);

		if (bExitAfterCmd == TRUE)
			goto skip;
	}

	if (*lpFileName!='\0') {

		/* generates real path if the path is uncomplete */

		if (Dos9_GetFilePath(lpFileAbs, lpFileName, sizeof(lpFileAbs))==-1)
			Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, -1);

	}

	/* run the batch */
	strcpy(ifIn.lpFileName, lpFileName);
	ifIn.iPos=0;
	ifIn.bEof=FALSE;

	DOS9_DBG("Running file \"%s\" ...\n", ifIn.lpFileName);

	Dos9_RunBatch(&ifIn);

	DOS9_DBG("\t Ran\nExiting...\n");

skip:

	Dos9_Exit();

	Dos9_LibClose();

	return 0;

}
