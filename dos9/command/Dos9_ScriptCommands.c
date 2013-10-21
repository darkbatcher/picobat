#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "LibDos9.h"

#include "Dos9_CmdLib.h"

#include "../core/Dos9_Core.h"



#include "../lang/Dos9_Lang.h"

#include "../errors/Dos9_Errors.h"

ESTR *lpesParameter, *lpesComplement;

void Dos9_ScriptCommandInit(void)
{
    lpesParameter=Dos9_EsInit();
    lpesComplement=Dos9_EsInit();
}

void Dos9_ScriptCommandEnd(void)
{
    Dos9_EsFree(lpesComplement);
    Dos9_EsFree(lpesParameter);
}

int Dos9_CmdEcho(char* lpLine)
{

    lpLine+=4;
    if (*lpLine==' ') {
         if (Dos9_GetNextParameterEs(lpLine, lpesParameter)) {
            if (!stricmp(Dos9_EsToChar(lpesParameter), "OFF")) {
                bEchoOn=FALSE;
                return 0;
            } else if (!stricmp(Dos9_EsToChar(lpesParameter) , "ON")) {
                bEchoOn=TRUE;
                return 0;
            } else if (!strcmp(Dos9_EsToChar(lpesParameter), "/?")) {
                puts(lpHlpDeprecated);
                return 0;
            }
         } else {
            /* si rien n'est entré on affiche l'état de la commannd echo */
            if (bEchoOn) puts(lpMsgEchoOn);
            else puts(lpMsgEchoOff);
            return 0;
         }
    }

    Dos9_GetEndOfLine(lpLine+1, lpesParameter);
    puts(Dos9_EsToChar(lpesParameter));

    return 0;
}

int Dos9_CmdExit(char* lpLine)
{
    char lpArg[]="-3000000000";
    char* lpNextToken;
    if ((lpNextToken=Dos9_GetNextParameter(lpLine+4, lpArg, 11))) {
        if (!stricmp(lpArg, "/?")) {
            puts(lpHlpDeprecated);
            return 0;
        } else if (!stricmp(lpArg, "/b")) {
            if ((lpNextToken=Dos9_GetNextParameter(lpNextToken, lpArg, 11))) {
                exit(atoi(lpArg));
            } else {
                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, "/b", FALSE);
                return 1;
            }
        } else {
            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpArg, FALSE);
            return 1;
        }
    }
    exit(0);
    return 0;
}

int Dos9_CmdPause(char* lpLine)
{
    if (!strcmp(lpLine+6, "/? ")) {
        puts(lpHlpDeprecated);
        return 0;
    }
    puts(lpMsgPause);
    getch();
    return 0;
}

int Dos9_CmdSet(char *lpLine)
{
    char lpArg[4], lpResult[20];
    EVALRES evResult;
    char* lpNextToken;
    char *lpVarName, *lpEqual;
    int i;
    if ((lpNextToken=Dos9_GetNextParameter(lpLine+3, lpArg, 4))) {
        if (!stricmp(lpArg,"/a")) {
            Dos9_GetNextParameterEs(lpNextToken, lpesParameter);
            if ((lpNextToken=strchr(Dos9_EsToChar(lpesParameter), '='))) {
                Dos9_EvalMath(lpNextToken+1, &evResult, getenv, bUseFloats);
                if (bUseFloats) {
                    sprintf(lpResult, "%g", evResult.dResult);
                } else {
                    sprintf(lpResult, "%d", evResult.iResult);
                }
                *(lpNextToken+1)='\0';
                Dos9_EsCat(lpesParameter, lpResult);
                Dos9_PutEnv(Dos9_EsToChar(lpesParameter));
            }
        } else if (!stricmp(lpArg, "/p")) {
            while (*lpNextToken==' ' || *lpNextToken=='\t') lpNextToken++;
            Dos9_EsCpy(lpesParameter, lpNextToken);

            lpVarName=Dos9_EsToChar(lpesParameter);
            if ((lpEqual=strchr(lpVarName, '='))) {

                lpEqual++;
                puts(lpEqual);
                Dos9_EsGet(lpesComplement, stdin);
                *lpEqual='\0';
                Dos9_EsCatE(lpesParameter, lpesComplement);
                Dos9_PutEnv(Dos9_EsToChar(lpesParameter));

            } else {
                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpVarName, FALSE);
            }

        } else if (!stricmp(lpArg, "/?")) {
            puts(lpHlpDeprecated);
        } else {
            lpLine=lpLine+3;
            while (*lpLine==' ' || *lpLine=='\t') lpLine++;
            if ((lpNextToken=strrchr(lpLine,' '))) *lpNextToken='\0';
            return Dos9_PutEnv(lpLine);
        }
    } else {
        for (i=0;environ[i];i++) puts(environ[i]);
        return 0;
    }
    return -1;
}

int Dos9_CmdSetLocal(char* lpLine)
{
    char lpName[FILENAME_MAX];
    char* lpNext=lpLine+8;

    while ((lpNext=Dos9_GetNextParameter(lpNext, lpName, FILENAME_MAX))) {
        if (!strcmp(lpName, "/?")) {
            puts(lpHlpDeprecated);
            return 0;
        } else if (!stricmp(lpName, "ENABLEDELAYEDEXPANSION")) {
            bDelayedExpansion=TRUE;
        } else if (!stricmp(lpName, "ENABLEFLOATS")) {
            bUseFloats=TRUE;
        } else if (!stricmp(lpName, "ENABLEDOS9MODE")) {
            bDos9Extension=TRUE;
        } else if (!stricmp(lpName, "DISABLEDOS9MODE")) {
            bDos9Extension=FALSE;
        } else if (!stricmp(lpName, "DISABLEFLOATS")) {
            bUseFloats=FALSE;
        } else if (!stricmp(lpName, "DISABLEDELAYEDEXPANSION")) {
            bDelayedExpansion=FALSE;
        } else if (!stricmp(lpName, "ENABLEEXTENSIONS") || !stricmp(lpName, "DISABLEEXTENSION")) {
            // provided for backward compatibility
        } else {
            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpName, FALSE);
            return 1;
        }
    }
    return 0;
}

int Dos9_CmdHelp(char* lpLine)
{
    puts(lpHlpDeprecated);
    return 0;
}

int Dos9_CmdRem(char* lpLine)
{
    return 0;
}


int Dos9_CmdCls(char* lpLine)
{
    char lpArg[4];
    if (Dos9_GetNextParameter(lpLine+3, lpArg, 4)) {
        if (!strcmp(lpArg, "/?")) {
            puts(lpHlpDeprecated);
            return 0;
        } else {
            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpArg, FALSE);
            return -1;
        }
    }
    Dos9_ClearConsoleScreen();
    return 0;
}

int Dos9_CmdColor(char* lpLine)
{
    char lpArg[4];
    if (Dos9_GetNextParameter(lpLine+5, lpArg, 4)) {
        if (!strcmp(lpArg, "/?")) {
            puts(lpHlpDeprecated);
        } else {
            colColor=strtol(lpArg, NULL, 16);
            Dos9_SetConsoleColor(colColor);
        }
        return 0;
    }
    colColor=DOS9_COLOR_DEFAULT;
    Dos9_SetConsoleColor(DOS9_COLOR_DEFAULT);
    return 0;
}

int Dos9_CmdTitle(char* lpLine)
{
    char lpArg[3];
    if (Dos9_GetNextParameter(lpLine+5, lpArg, 3)) {
        if (!strcmp(lpArg, "/?")) {
            puts(lpHlpDeprecated);
        } else {
            Dos9_SetConsoleTitle(lpLine+6);
        }
        return 0;
    }
    return -1;
}

int Dos9_CmdType(char* lpLine)
{
    char lpFileName[FILENAME_MAX];
    FILE* pFile;
    if (Dos9_GetNextParameter(lpLine+4, lpFileName, FILENAME_MAX)) {
        if (!strcmp(lpFileName, "/?")) {
            puts(lpHlpDeprecated);
            return 0;
        } else if ((pFile=fopen(lpFileName, "r"))) {
            while (fgets(lpFileName, FILENAME_MAX, pFile)) printf("%s", lpFileName);
            fclose(pFile);
            return 0;
        }
    }

    Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, FALSE);
    return -1;
}

int Dos9_CmdGoto(char* lpLine)
{
    char lpLabelName[FILENAME_MAX];
    char lpFileName[FILENAME_MAX];
    char* lpFile=NULL;
    lpLine+=4;
    *lpLine=':';
    if (*(lpLine+1)==':') lpLine++;
    if ((lpLine=Dos9_GetNextParameter(lpLine, lpLabelName, FILENAME_MAX))) {
        DEBUG("Found label Name");
        if (!strcmp(lpLabelName, ":/?")) {
            puts(lpHlpDeprecated);
            return 0;
        }
        if (Dos9_GetNextParameter(lpLine ,lpFileName, FILENAME_MAX)){
            if (bDos9Extension == FALSE) Dos9_ShowErrorMessage(DOS9_EXTENSION_DISABLED_ERROR, NULL, FALSE);
            else lpFile=lpFileName;
        }
        /* Now we have a valid label name, thus  we are about to find a label in the specified file */
        /* if we do have a valid file name, the search will be made in specified file */
        DEBUG("Jump to Label [(name) and (filename) nexts line]");
        DEBUG(lpLabelName);
        DEBUG(lpFile);
        if (Dos9_JumpToLabel(lpLabelName, lpFile)==-1) {
            Dos9_ShowErrorMessage(DOS9_LABEL_ERROR, lpLabelName, FALSE);
        }
        DEBUG("Jump made");
    }
    return 0;
}

int Dos9_CmdCd(char* lpLine)
{
    char* lpNext;
    char lpArg[4];
    if (!(lpLine=strchr(lpLine, ' '))) {
        Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, FALSE);
        return -1;
    }

    if ((lpNext=Dos9_GetNextParameter(lpLine, lpArg, 4))) {
        if (!strcmp(lpArg, "/?")) {
            puts(lpHlpDeprecated);
            return 0;
        } else if (!stricmp(lpArg, "/d")) {
            lpLine=lpNext;
        }

        while (*lpLine==' ' || *lpLine=='\t') lpLine++;

        if ((lpNext=strrchr(lpLine, ' '))) *lpNext='\0';
        /* FIX ME :
            A common bug occurs when specifying path with
            extra spaces at the end of the line. In windows,
            this does not changes the current path but do
            not returns errors though.

            This is not a real problem, but we need to compare
            with *nix behaviours about spaces left in paths, to
            find the right to handle this.
        */

        errno=0;
        chdir(lpLine);

        if (errno ==  0) {

            Dos9_UpdateCurrentDir();
            return 0;

        } else {

            /* do not perform errno checking
               as long as the most important reason for
               chdir to fail is obviously the non existence
               or the specified directory

               However, it appears that this is inconsistant
               using windows as it does not returns on failure
               every time a non-existing folder is passed to the
               function, tried with '.. ' on my system
            */

            Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, lpLine, FALSE);
            return -1;
        }
    }

    puts(Dos9_GetCurrentDir());
    return 0;
}
