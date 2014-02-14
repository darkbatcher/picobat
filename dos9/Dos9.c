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



#if defined WIN32
    #include <conio.h>
#else
    /* definitions for GNU/Linux compatibility */
    #define getch(a)
#endif

#include <libDos9.h>

#include "errors/Dos9_Errors.h"

#include "lang/Dos9_Lang.h"

#include "core/Dos9_Core.h"

/* those are codes of internal commands */

#include "command/Dos9_ScriptCommands.h"
#include "command/Dos9_Conditions.h"
#include "command/Dos9_For.h"
#include "command/Dos9_CommandInfo.h"


int main(int argc, char *argv[])
{
    /*  a function which initializes Dos9's engine,
        parses the command line argumments,
        And display starting message

    */

    char *lpFileName=NULL,
          lpFileAbs[FILENAME_MAX];
    char lpTitle[FILENAME_MAX+10]="Dos9 [" DOS9_VERSION "] - ";
    int i;
    int j;
    int c;
    int bQuiet=FALSE;

    if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);
    }

    /* Set new line Mode to UNIX */
    Dos9_SetNewLineMode(DOS9_NEWLINE_UNIX);

    lpvLocalVars=Dos9_GetLocalBlock();
    Dos9_InitConsole();

    #ifdef WIN32

        SetThreadLocale(LOCALE_USER_DEFAULT);

    #elif defined _POSIX_C_SOURCE

        setlocale(LC_ALL, "");

    #endif // WINDOWS

    /* Load Messages (including errors) */
    Dos9_LoadStrings();
    Dos9_LoadErrors();

    /* **********************************
       *   getting Dos9's parameters    *
       ********************************** */

    for (i=1;argv[i];i++) {
        if (*argv[i]=='/') {
            argv[i]++;
            switch(toupper(*argv[i])) {

                case 'V': // enables expansion
                    bDelayedExpansion=TRUE;
                    break;

                case 'F': // enables floats
                    bUseFloats=TRUE;
                    break;

                case 'E':
                    bEchoOn=FALSE;
                    break;

                case 'N': // enables new commands
                    bDos9Extension=TRUE;
                    break;

                case 'Q':
                    bQuiet=TRUE; // run silently
                    break;

                case 'I':
                    if (!argv[++i]) {

                        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "Dos9", -1);

                    }

                    iInputD=atoi(argv[i]); // select input descriptor
                    break;

                case 'O':
                    if (!argv[++i]) {

                        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "Dos9", -1);

                    }

                    iOutputD=atoi(argv[i]); // select input descriptor
                    break;

                case '?':
                    puts("DOS9 [" DOS9_VERSION "] - Build " DOS9_BUILDDATE "\n"
                         "Copyright (c) 2010-" DOS9_BUILDYEAR " " DOS9_AUTHORS "\n\n"
                         "This is free software, you can modify and/or redistribute it under "
                         "the terms of the GNU Genaral Public License.\n");

                    puts(lpHlpMain);
                    return 0;

                default:
                    Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, -1);
            }
        } else {
            if (lpFileName) {
                /* set parameters for the file currently runned */
                for (j=i ,  c='1';argv[j] && c<='9';i++, c++ , j++ ) {
                    Dos9_SetLocalVar(lpvLocalVars, c, argv[j]);
                }
                break;
            }
            lpFileName=argv[i];
            Dos9_SetLocalVar(lpvLocalVars, '0', lpFileName);
        }

    }

    /* initialisation du système de génération aléatoire */

    srand(time(NULL));

    colColor=DOS9_COLOR_DEFAULT;
    /* messages affichés */

    Dos9_InitConsole();


    if (!lpFileName) {

        if (!bQuiet)
            Dos9_PrintIntroduction();

        strcat(lpTitle, "Command prompt");
        Dos9_PutEnv("DOS9_IS_SCRIPT=false");

    } else {

        if (Dos9_GetFilePath(lpFileAbs, lpFileName, sizeof(lpFileAbs))==-1)
            Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, -1);

        lpFileName=lpFileAbs;

        strncat(lpTitle, lpFileName, sizeof(lpTitle)-sizeof("Dos9 [" DOS9_VERSION "] - "));
        Dos9_PutEnv("DOS9_IS_SCRIPT=true");

    }

    if (!bQuiet) {

        Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
        Dos9_SetConsoleTitle(lpTitle);

    }

    strcpy(lpTitle, "DOS9_PATH=");

    Dos9_GetExePath(lpTitle+10, FILENAME_MAX);

    lpInitVar[2]=lpTitle;
    Dos9_InitVar(lpInitVar);

      /**********************************************
       *         Initialization of Modules          *
       **********************************************/

    putenv("ERRORLEVEL=0");

    lpclCommands=Dos9_MapCommandInfo(lpCmdInfo, sizeof(lpCmdInfo)/sizeof(COMMANDINFO));

    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_INIT, NULL, NULL);
    Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_INIT, NULL, NULL);

    lppsStreamStack=Dos9_InitStreamStack();

    /* getting input intialised (if they are specified) */

    if (iInputD) {

        Dos9_OpenOutputD(lppsStreamStack, iInputD, DOS9_STDIN);

    }

    if (iOutputD) {

        Dos9_OpenOutputD(lppsStreamStack, iOutputD, DOS9_STDOUT);

    }

    pErrorHandler=Dos9_Exit;



    /* running auto batch initialisation */
    Dos9_UpdateCurrentDir();

    strcat(lpTitle, "/Dos9_Auto.bat");
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETFILE, lpTitle+10, NULL);
    Dos9_RunBatch(TRUE);

    Dos9_LoadErrors();
    Dos9_LoadStrings();

    /* todo : Fix the bug caused by cd in batch scripts :
       sometimes, the filename given is relative and not absolute
       so that any use of cd will make the script to have
       undefined behaviour */

    /* then run batch mode */
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETFILE, lpFileName, NULL);
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETPOS, NULL, NULL);

    Dos9_RunBatch((int)lpFileName); // if we are actually running a script lpFileName is non-zero

    Dos9_Exit();

    return 0;

}
