/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2013 DarkBatcher
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
#include <time.h>

#include "matheval.h"
#include "inteval.h"

#include "libDos9.h"

#include "Dos9_CmdLib.h"

#include "../core/Dos9_Core.h"

#include "Dos9_ScriptCommands.h"

#include "../lang/Dos9_Lang.h"

#include "../errors/Dos9_Errors.h"

int Dos9_CmdEcho(char* lpLine)
{

    ESTR* lpEsParameter=Dos9_EsInit();

    lpLine+=4;

    if (*lpLine==' ') {

         if (Dos9_GetNextParameterEs(lpLine, lpEsParameter)) {

            if (!stricmp(Dos9_EsToChar(lpEsParameter), "OFF")) {

                bEchoOn=FALSE;

            } else if (!stricmp(Dos9_EsToChar(lpEsParameter) , "ON")) {

                bEchoOn=TRUE;

            } else if (!strcmp(Dos9_EsToChar(lpEsParameter), "/?")) {

                puts(lpHlpDeprecated);

            } else {

                goto print;

            }

         } else {

            /* si rien n'est entré on affiche l'état de la commannd echo */
            if (bEchoOn) puts(lpMsgEchoOn);
            else puts(lpMsgEchoOff);

         }

    } else {

        print:

        Dos9_GetEndOfLine(lpLine+1, lpEsParameter);
        puts(Dos9_EsToChar(lpEsParameter));

    }

    Dos9_EsFree(lpEsParameter);

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

double _Dos9_SetGetVarFloat(const char* lpName)
{
    char* lpContent;

    lpContent=getenv(lpName);

    if (lpContent) {

        if (!strcmp(lpContent, "nan")){

            return NAN;

        } else if (!strcmp(lpContent, "inf")) {

            return INFINITY;

        } else {

            return atof(lpContent);
        }

    } else {

        return 0.0;

    }

}

int _Dos9_SetGetVarInt(const char* lpName)
{
    char* lpContent;

    lpContent=getenv(lpName);

    if (lpContent) {

        return atoi(lpContent);

    } else {

        return 0;

    }

}

int Dos9_CmdSet(char *lpLine)
{
    char lpArgBuf[5],
         *lpArg=lpArgBuf;

    char *lpNextToken;

    int i,
        bFloats;

    if ((lpNextToken=Dos9_GetNextParameter(lpLine+3, lpArgBuf, sizeof(lpArgBuf)))) {

        if (!stricmp(lpArg, "/?")) {

            /* help is deprecated */
            puts(lpHlpDeprecated);
            goto error;

        } else if (!strnicmp(lpArg,"/a", 2)) {

            lpArg+=2;

            bFloats=bUseFloats;
            /* use mode set through setlocal */

            if (*lpArg==':') lpArg++;

            switch (toupper(*lpArg)) {

                case 'F' :
                    bFloats=TRUE;
                    break;

                case 'I' :
                    bFloats=FALSE;
            }

            /* get the floats */
            if (!(Dos9_CmdSetA(lpNextToken, bFloats)))
                goto error;

        } else if (!stricmp(lpArg, "/p")) {

            if ((Dos9_CmdSetP(lpNextToken)))
                goto error;

        } else {

            /* simple set */

            lpLine=lpLine+3;
            while (*lpLine==' ' || *lpLine=='\t') lpLine++;
            if ((lpNextToken=strrchr(lpLine,' '))) *lpNextToken='\0';
            Dos9_PutEnv(lpLine);

        }

    } else {

        /* in default cases, print environment */
        for (i=0;environ[i];i++) puts(environ[i]);

    }

    return 0;

    error:
        return -1;
}

int Dos9_CmdSetP(char* lpLine)
{

    ESTR* lpEsVar=Dos9_EsInit();
    ESTR* lpEsInput=Dos9_EsInit();
    char* lpEqual;

    while (*lpLine==' ' || *lpLine=='\t') lpLine++;

    Dos9_GetEndOfLine(lpLine, lpEsVar);

    if ((lpEqual=strchr(Dos9_EsToChar(lpEsVar), '='))) {

        lpEqual='\0';
        lpEqual++;

        puts(lpEqual);

        Dos9_EsGet(lpEsInput, stdin);

        Dos9_EsCat(lpEsVar, "=");
        Dos9_EsCatE(lpEsVar, lpEsInput);

        Dos9_PutEnv(Dos9_EsToChar(lpEsVar));

    } else {

        Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpEsVar), FALSE);
        goto error;

    }

    Dos9_EsFree(lpEsVar);
    Dos9_EsFree(lpEsInput);
    return 0;

    error:
        Dos9_EsFree(lpEsVar);
        Dos9_EsFree(lpEsInput);
        return -1;
}

int Dos9_CmdSetA(char* lpLine, int bFloats)
{

    ESTR* lpExpression=Dos9_EsInit();

    /* get the expression back */
    Dos9_GetEndOfLine(lpLine, lpExpression);

    switch(bFloats) {

        case TRUE:
            /* evaluate floating expression */
            if ((Dos9_CmdSetEvalFloat(lpExpression)))
                goto error;

            break;

        case FALSE:
            /* evaluate integer expression */
            if ((Dos9_CmdSetEvalInt(lpExpression)))
                goto error;

            break;

    }


    Dos9_EsFree(lpExpression);
    return 0;

    error:
        Dos9_EsFree(lpExpression);
        return -1;
}

int Dos9_CmdSetEvalFloat(ESTR* lpExpression)
{
    void* evaluator; /* an evaluator for libmatheval-Dos9 */
    char *lpVarName,
         *lpEqual,
          lpResult[30];
    char  cLeftAssign=0;
    double dResult,
           dVal;

    lpVarName=Dos9_EsToChar(lpExpression);

    while (*lpVarName==' ' || *lpVarName=='\t') lpVarName++;

    /* if we don't have expression, end-up with an error */
    if (!*lpVarName) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "SET", FALSE);
        goto error;

    }

    /* seek an '=' sign */
    if (!(lpEqual=strchr(lpVarName, '='))) {

        Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpVarName, FALSE);
        goto error;

    }

    *lpEqual='\0';

    /* seek a sign like '+=', however, '' might be a valid environment
       variable name depending on platform */
    if (lpEqual != lpVarName) {

        cLeftAssign=*(lpEqual-1);

    }

    /* create evaluator */
    if (!(evaluator=evaluator_create(lpEqual+1))) {

        Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
        goto error;

    }

    dResult=evaluator_evaluate2(evaluator, _Dos9_SetGetVarFloat);

    evaluator_destroy(evaluator);

    /* clear if operator is recognised */

    switch (cLeftAssign) {

        case '*':
        case '/':
        case '+':
        case '-':
            *(lpEqual-1)='\0';
            /* get the value of the variable */
            dVal=_Dos9_SetGetVarFloat(lpVarName);

            switch(cLeftAssign) {

                case '*':
                    dVal*=dResult;
                    break;

                case '/':
                    dVal/=dResult;
                    break;

                case '+':
                    dVal+=dResult;
                    break;

                case '-':
                    dVal-=dResult;

            }

            break;

        default:
            dVal=dResult;

    }

    snprintf(lpResult, sizeof(lpResult), "=%.16g", dVal);

    Dos9_EsCat(lpExpression, lpResult);

    Dos9_PutEnv(Dos9_EsToChar(lpExpression));

    return 0;

    error:
        return -1;
}

/* evaluate an interger expression */
int Dos9_CmdSetEvalInt(ESTR* lpExpression)
{
    char *lpVarName,
         *lpEqual,
          lpResult[30];
    char  cLeftAssign=0;
    int   iResult,
          iVal,
          bDouble=FALSE;

    Dos9_EsCat(lpExpression, "\n");

    lpVarName=Dos9_EsToChar(lpExpression);

    while (*lpVarName==' ' || *lpVarName=='\t') lpVarName++;

    /* if we don't have expression, end-up with an error */
    if (!*lpVarName) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "SET", FALSE);
        goto error;

    }

    /* seek an '=' sign */
    if (!(lpEqual=strchr(lpVarName, '='))) {

        Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpVarName, FALSE);
        goto error;

    }

    *lpEqual='\0';

    if (lpEqual != lpVarName) {

        cLeftAssign=*(lpEqual-1);

    }

    iResult=IntEval_Eval(lpEqual+1);

    if (IntEval_Error != INTEVAL_NOERROR) {

        Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
        goto error;

    }

    switch(cLeftAssign) {

        case '*':
        case '/':
        case '+':
        case '-':
        case '^':

            *(lpEqual-1)='\0';
            iVal=_Dos9_SetGetVarInt(lpVarName);

            switch(cLeftAssign) {

                case '*':
                    iVal*=iResult;
                    break;

                case '/':
                    iVal/=iResult;
                    break;

                case '+':
                    iVal+=iResult;
                    break;

                case '-':
                    iVal/=iResult;
                    break;

                case '^':
                    iVal^=iResult;
                    break;

            }

            break;

        case '&':
        case '|':

            /* more complicated, it need to be
               resolved */

            *(lpEqual-1)='\0';
            iVal=_Dos9_SetGetVarInt(lpVarName);

            if (lpVarName != (lpEqual-1)) {

                if (*(lpEqual-2) == cLeftAssign) {

                    bDouble=TRUE;
                    *(lpEqual-2)='\0';

                }

            }

            switch (cLeftAssign) {

                case '|':
                    if (bDouble){

                        iVal=iVal || iResult;

                    } else {

                        iVal|=iResult;

                    }

                    break;

                case '&':
                    if (bDouble){

                        iVal=iVal && iResult;

                    } else {

                        iVal&=iResult;

                    }

            }

            break;

        default:
            iVal=iResult;

    }

    snprintf(lpResult, sizeof(lpResult), "=%d", iVal);

    Dos9_EsCat(lpExpression, lpResult);
    Dos9_PutEnv(Dos9_EsToChar(lpExpression));

    return 0;

    error:
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
    ESTR* lpEsTitle=Dos9_EsInit();

    if (Dos9_GetNextParameter(lpLine+5, lpArg, 3)) {

        if (!strcmp(lpArg, "/?")) {

            puts(lpHlpDeprecated);

        } else {

            Dos9_GetEndOfLine(lpLine, lpEsTitle);
            Dos9_SetConsoleTitle(Dos9_EsToChar(lpEsTitle));

        }

        Dos9_EsFree(lpEsTitle);

        return 0;
    }

    Dos9_EsFree(lpEsTitle);

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
    int bEchoError=TRUE;

    lpLine+=4;
    *lpLine=':';

    if (*(lpLine+1)==':') lpLine++;
    if ((lpLine=Dos9_GetNextParameter(lpLine, lpLabelName, FILENAME_MAX))) {
        DEBUG("Found label Name");

        if (!strcmp(lpLabelName, ":/?")) {

            puts(lpHlpDeprecated);
            return 0;

        }

        if ((lpLine=Dos9_GetNextParameter(lpLine ,lpFileName, FILENAME_MAX))) {

            if (!stricmp(lpFileName, "/Q")) {
                /* on a choisi de rendre l'erreux muette */
                bEchoError=FALSE;

                if (!(Dos9_GetNextParameter(lpLine, lpFileName, FILENAME_MAX)))
                    goto next;
            }

            if (bDos9Extension == FALSE) Dos9_ShowErrorMessage(DOS9_EXTENSION_DISABLED_ERROR, NULL, FALSE);
            else lpFile=lpFileName;
        }

        next:

        /* Now we have a valid label name, thus  we are about to find a label in the specified file */
        /* if we do have a valid file name, the search will be made in specified file */
        DEBUG("Jump to Label [(name) and (filename) nexts line]");
        DEBUG(lpLabelName);
        DEBUG(lpFile);

        if (Dos9_JumpToLabel(lpLabelName, lpFile)==-1 && bEchoError) {
            Dos9_ShowErrorMessage(DOS9_LABEL_ERROR, lpLabelName, FALSE);
        }

        DEBUG("Jump made");
    }
    return 0;
}

int Dos9_CmdCd(char* lpLine)
{
    char* lpNext;
    ESTR* lpEsDir=Dos9_EsInit();

    if (!(lpLine=strchr(lpLine, ' '))) {
        Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, FALSE);
        goto error;
    }

    if ((lpNext=Dos9_GetNextParameterEs(lpLine, lpEsDir))) {

        if (!strcmp(Dos9_EsToChar(lpEsDir), "/?")) {

            puts(lpHlpDeprecated);
            goto error;

        } else if (!stricmp(Dos9_EsToChar(lpEsDir), "/d")) {

            lpLine=lpNext;

        }

        while (*lpLine==' ' || *lpLine=='\t') lpLine++;

        Dos9_GetEndOfLine(lpLine, lpEsDir);

        lpLine=Dos9_EsToChar(lpEsDir);

        lpNext=NULL;

        while (*lpLine) {

            switch(*lpLine) {
                case '\t':
                case ' ':

                    if (!lpNext) lpNext=lpLine;
                    break;

                default:
                    lpNext=NULL;
            }

            lpLine++;

        }

        if (lpNext) *lpNext='\0';

        errno=0;

        lpLine=Dos9_EsToChar(lpEsDir);

        chdir(lpLine);

        if (errno ==  0) {

            Dos9_UpdateCurrentDir();

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
            goto error;

        }

    } else {

        puts(Dos9_GetCurrentDir());

    }

    Dos9_EsFree(lpEsDir);
    return 0;

    error:

        Dos9_EsFree(lpEsDir);
        return -1;
}

int Dos9_CmdBlock(char* lpLine)
{
    BLOCKINFO bkCode;
    char* lpToken,
          *lpEnv;

    lpEnv=GetEnvironmentStrings();

    if (lpEnv) {

        FreeEnvironmentStrings(lpEnv);

    }

    lpToken=Dos9_GetNextBlock(lpLine, &bkCode);

    if (*lpToken!=')') {

        Dos9_ShowErrorMessage(DOS9_INVALID_TOP_BLOCK, lpLine, FALSE);
        return -1;

    }

    lpToken++;

    while (*lpToken==' ' || *lpToken=='\t') lpToken++;

    if (*lpToken) {

        Dos9_ShowErrorMessage(DOS9_INVALID_TOP_BLOCK, lpLine, FALSE);
        return -1;

    }

    Dos9_RunBlock(&bkCode);

    return iErrorLevel;
}
