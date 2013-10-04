/*  Dos9 - copyleft (c) DarkBatcher

    This source is a part of Darkbatchr's Dos9 interpreter

    Permission granted to modify and distribute copy of this software, whether modified or not, provided that the
    following condition are respected

        * Any copy, either of an modified version or not, must retain the above copyright and this license
        * Modified versions of this software must be redistributed under the same conditions as this executable,
          without any license change, or term add

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

#include "LibDos9.h"

#include "errors/Dos9_Errors.h"

#include "lang/Dos9_Lang.h"

#include "core/Dos9_Core.h"

/* those are codes of internal commands */

#include "command/Dos9_ScriptCommands.h"
#include "command/Dos9_Conditions.h"
#include "command/Dos9_CommandInfo.h"


/*
#include "commands/Dos9_FolderCommands.h" // CHDIR, DIR, MKDIR, RMDIR
#include "commands/Dos9_FileCommands.h" // MOVE, DEL, COPY, REN, TYPE
#include "commands/Dos9_ScriptCommands.h" // ECHO, SETLOCAL, SET, GOTO, PAUSE, CLS
#include "commands/Dos9_Conditions.h" // IF, FOR
*/


int main(int argc, char *argv[])
{
    /*  a function which initializes Dos9's engine,
        parses the command line argumments,
        And display starting message
    */

    char* lpFileName=NULL;
    char lpTitle[FILENAME_MAX+10]="Dos9 [version 0.7] - ";
    int i;
    int j;
    int c;
    int bSave;
    lpvLocalVars=Dos9_GetLocalBlock();
    Dos9_InitConsole();
    Dos9_ExpandInit();
    Dos9_ScriptCommandInit();

    #ifdef WINDOWS

        SetThreadLocale(LOCALE_USER_DEFAULT);

    #elif defined __linux

        setlocale(LC_ALL, "");

    #endif // WINDOWS

    Dos9_LoadStrings();
    Dos9_LoadErrors();

    Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);

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
                case '?':
                    printf("DOS9 [version 0.7] - Released %s\nCopyleft (c) DarkBatcher 2010-2012 - some rights reserved\n\n", __DATE__);
                    puts(lpHlpMain);
                    return 0;
                default:
                    Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, -1);
            }
        } else {
            if (lpFileName) {
                /* set parameters for the file currently runned */
                for (j=i ,  c='1';argv[j] && c<='9';i++, c++) {
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

    if (!lpFileName){
        Dos9_PrintIntroduction(); //on affiche le message d'accueil
        strcat(lpTitle, "Invite de commande");
        Dos9_PutEnv("DOS9_IS_SCRIPT=false");
    } else if (!Dos9_FileExists(lpFileName)) {
        Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, -1); // ou le message d'erreur si le fichier n'existe pas
    } else {
        strncat(lpTitle, lpFileName, sizeof(lpTitle)-sizeof("Dos9 [version 0.7] - "));
         Dos9_PutEnv("DOS9_IS_SCRIPT=true");
    }
    Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    Dos9_SetConsoleTitle(lpTitle);
    strcpy(lpTitle, "DOS9_PATH=");

    Dos9_GetExePath(lpTitle+10, FILENAME_MAX);

    lpInitVar[2]=lpTitle;
    Dos9_InitVar(lpInitVar);
      /**********************************************
       *         Initialization of Modules          *
       **********************************************/
    putenv("ERRORLEVEL=0");
    lpclCommands=Dos9_MapCommandInfo(lpCmdInfo, sizeof(lpCmdInfo)/sizeof(COMMANDINFO));
    //printf("CommandList=%d\n",lpclCommands);
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_INIT, NULL, NULL);
    Dos9_SendMessage(DOS9_PARSE_MODULE, MODULE_PARSE_INIT, NULL, NULL);
    //Dos9_SendMessage(DOS9_VAR_MODULE, MODULE_VAR_INIT, NULL, NULL);
    DEBUG("Initializing stream environment pointer");
    lppsStreamStack=Dos9_InitStreamStack();
    DEBUG("Done");
    //Dos9_SendMessage(DOS9_CALL_MODULE, MODULE_CALL_INIT, NULL,NULL);

    /* running auto batch initialisation */
    Dos9_UpdateCurrentDir();
    bSave=bEchoOn;

    bEchoOn=FALSE;

    strcat(lpTitle+10, "/Dos9_Auto.bat");
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETFILE, lpTitle+10, NULL);
    Dos9_RunBatch(TRUE);

    bEchoOn=bSave;
    /* then run batch mode */
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETFILE, lpFileName, NULL);
    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_SETPOS, NULL, NULL);

    Dos9_RunBatch((int)lpFileName); // if we are actually running a script lpFileName is non-zero

    Dos9_FreeCommandList(lpclCommands);
    Dos9_FreeStreamStack(lppsStreamStack);
    Dos9_FreeLocalBlock(lpvLocalVars);
    Dos9_ExpandEnd();
    Dos9_ScriptCommandEnd();

    return 0;

}
