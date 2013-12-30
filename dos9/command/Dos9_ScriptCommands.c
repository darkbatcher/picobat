#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matheval.h"
#include "inteval.h"

#include "LibDos9.h"

#include "Dos9_CmdLib.h"

#include "../core/Dos9_Core.h"

#include "Dos9_ScriptCommands.h"

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

double _Dos9_SetGetVarFloat(const char* lpName)
{
    char* lpContent;

    lpContent=getenv(lpName);

    if (lpContent) {

        return atof(lpContent);

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

    char *lpNextToken,
         *lpVarName,
         *lpEqual;

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
                goto error;

            }

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

int Dos9_CmdSetA(char* lpLine, int bFloats)
{

    ESTR* lpExpression=Dos9_EsInit();

    /* get the expression back */
    Dos9_GetNextParameterEs(lpLine, lpExpression);

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

    snprintf(lpResult, sizeof(lpResult), "=%.10g", dVal);

    Dos9_EsCat(lpExpression, lpResult);

    Dos9_PutEnv(Dos9_EsToChar(lpExpression));

    return 0;

    error:
        return -1;
}

int Dos9_CmdSetEvalInt(ESTR* lpExpression)
{
    return 0;
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
