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
#include <errno.h>

#include "LibDos9.h"
#include "Dos9_For.h"

#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"

#include "Dos9_CmdLib.h"

/*
    The for command

    * For each item in a list.
      Default token are ", " and carriot return.

        FOR %%A IN (ens) DO (

        )

    * For each interger value in a range of values

        FOR /L %%A IN (begin, increment, end) DO (

        )

    * For each file of a directory :

        FOR /R "basedir" %%A IN (selector) DO (

        )

      DEPRECATED : such kind of old fashionned loop
      with unflexible option are processed like the
      following line :

        FOR /F "tokens=*" %%A IN ('dir /B basedir/selector') DO (

        )

    * Process text from a file :

        FOR /F "options" %%A IN (file) DO command
        FOR /F "options" %%A IN ("string") DO command
        FOR /F "options" %%A IN (`string`) DO command

 */

int Dos9_CmdFor(char* lpLine)
{
     ESTR* lpParam=Dos9_EsInit();
     ESTR* lpDirectory=Dos9_EsInit();
     ESTR* lpInputBlock=Dos9_EsInit();

     BLOCKINFO bkCode;

     FORINFO forInfo={
         " ", /* no tokens delimiters, since only one
                token is taken account */
         "", /* no end-of-line delimiters */
         0, /* no skipped line */
         0, /* this is to be fullfiled later (the
                name letter of loop special var) */
         FALSE,
         1, /* the number of tokens we were given */
         {TOHIGH(1)|1} /* get all the token
                                    back */

     };

     char  cVarName;
     char* lpToken=lpLine+3;
     char* lpVarName;

     int iForType=FOR_LOOP_SIMPLE;

     while ((lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

        if (!stricmp(Dos9_EsToChar(lpParam), "/F")) {

            iForType=FOR_LOOP_F;

        } else if (!stricmp(Dos9_EsToChar(lpParam), "/L")) {

            iForType=FOR_LOOP_L;

        } else if (!stricmp(Dos9_EsToChar(lpParam), "/R")) {

            iForType=FOR_LOOP_R;

        } else {

            break;

        }

     }

     while (TRUE) {


         if (lpToken == NULL) {

            /* if the line is not complete */
            Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "FOR", FALSE);

            goto error;

         }

         if ( *Dos9_EsToChar(lpParam) == '%' ) {

            /*
                The user did not specified options, that's not
                problematic.
            */

            /* get back the var name */
            lpVarName=Dos9_EsToChar(lpParam)+1;

            if (*lpVarName=='%') lpVarName++;

            cVarName=*lpVarName;

            /* test conformance

            FIXME : This is memory sailing away to sea */
            // DOS9_TEST_VARNAME(cVarName);

            /* just get out of that loop */
            break;

         } else if (iForType == FOR_LOOP_SIMPLE || iForType == FOR_LOOP_L || *Dos9_EsToChar(lpDirectory) != '\0') {

            /* It appears that the users does not specify a variable
               but try rather to path options, that are obviously inconsistent
               in this case */
            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
            goto error;

         } else {

            switch(iForType) {

                case FOR_LOOP_R:
                    /* In that case, this is must be the
                       directory parameter */
                    Dos9_EsCpyE(lpDirectory, lpParam);
                    break;

                case FOR_LOOP_F:
                    /* in that case, this must be one of
                       the tokens that contains for-loop parameter.
                       This parameter can be dispatched in many different
                       parameter, as long are they're in a row */
                    if ((Dos9_ForMakeInfo(Dos9_EsToChar(lpParam), &forInfo)))  {

                        return -1;

                    }
            }

            lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

         }

     }

     /* if the for uses either /F or /R or even /D
        store cVarName in the FORINFO structure
     */

     if (FOR_LOOP_F == iForType || FOR_LOOP_R == iForType) {

        forInfo.cFirstVar=cVarName;

     }


     /* Now the next parameter should be IN */

     lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

     if (stricmp(Dos9_EsToChar(lpParam), "IN") || lpToken == NULL ) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "FOR", FALSE);
        goto error;

     }


     /* Now, the program expect to be given the list of argument
        of the FOR loop, i.e. the part that should be enclosed
        on parenthesis */

     while (*lpToken == ' ' || *lpToken == '\t') lpToken++;
        /* it is not obvious that the lpToken pointer
           points precisely to a non space character */

     if (*lpToken != '(') {

        /* if no parenthesis-enclosed argument have been found
           then, we return an error */

       Dos9_ShowErrorMessage(DOS9_ARGUMENT_NOT_BLOCK, lpToken, FALSE);
       goto error;

     }

     /* So just get back the block */
     lpToken=Dos9_GetNextBlockEs(lpToken, lpInputBlock);

     /* Now we check if ``DO'' is used */

     if (!(lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "FOR", FALSE);
        goto error;

     }

     if ((stricmp(Dos9_EsToChar(lpParam), "DO"))) {

        /* if ``DO'' is not used */
        Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
        goto error;

     }

     lpToken = Dos9_GetNextBlock(lpToken, &bkCode);

     if (*lpToken) {

        lpToken++;

        while (*lpToken==' ' || *lpToken=='\t')
            lpToken++;

        if (*lpToken) {

            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
            goto error;

        }

     }

     switch(iForType){

        case FOR_LOOP_SIMPLE:

            /* this handles simple for */
            Dos9_CmdForSimple(lpInputBlock, &bkCode, cVarName);

            break;

        case FOR_LOOP_R:
            break;

        case FOR_LOOP_F:
            Dos9_CmdForF(lpInputBlock, &bkCode, &forInfo);
            break;

        case FOR_LOOP_L:
            break;

     }

     Dos9_EsFree(lpParam);
     Dos9_EsFree(lpDirectory);
     Dos9_EsFree(lpInputBlock);

     return 0;

     error:
        Dos9_EsFree(lpParam);
        Dos9_EsFree(lpDirectory);
        Dos9_EsFree(lpInputBlock);

        return -1;
}

int Dos9_CmdForSimple(ESTR* lpInput, BLOCKINFO* lpbkCommand, char cVarName)
{
    char  lpDelimiters[]=" ,:;\n\t";
    char* lpToken=Dos9_EsToChar(lpInput);
    char* lpNextToken;

    char  cContinue=TRUE;

    while (*lpToken && cContinue) {

        while ((strchr(lpDelimiters, *lpToken))) lpToken++;
        /* skip in raw delimiters */

        if (*lpToken=='\''  || *lpToken=='"') {

            /* the tokens begins with either a "'" or a '"'
               then we try to find the closing delimiter*/

            /* no need to increment lpToken since quotes are
               preserved, as stated by the documentation */
            lpNextToken=lpToken+1;

            cContinue=FALSE;

            while (*lpNextToken) {


                if (*lpNextToken==*lpToken) {

                    if ((strchr(lpDelimiters, *(lpNextToken+1)))) {

                        cContinue=TRUE;
                        lpNextToken++;
                        *lpNextToken='\0';

                        break;

                    }

                }

                lpNextToken++;

            }

        } else {

            /* if we do not found quotes, then
               look for the following delimiter */

            lpNextToken=lpToken;

            while (*lpNextToken && !(strchr(lpDelimiters, *lpNextToken))) {

                lpNextToken++;

            }

            if (*lpNextToken=='\0') cContinue=FALSE;

            *lpNextToken='\0';

        }

        /* assign the local variable */
        if (!Dos9_SetLocalVar(lpvLocalVars, cVarName, lpToken)) {


            return -1;

        }

        /* run the block */
        Dos9_RunBlock(lpbkCommand);

        lpToken=lpNextToken+1;

    }

    /* delete the special var */
    Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);


    return 0;

}

int Dos9_CmdForF(ESTR* lpInput, BLOCKINFO* lpbkInfo, FORINFO* lpfrInfo)
{
    int iSkip=lpfrInfo->iSkip;

    ESTR* lpInputToP=Dos9_EsInit();

    INPUTINFO inputInfo;

    //printf("Starting for LOOP \n");

    if ((Dos9_ForVarCheckAssignment(lpfrInfo)))
        goto error;

    if ((Dos9_ForMakeInputInfo(lpInput, &inputInfo, lpfrInfo) == -1))
        goto error;

    while (Dos9_ForGetInputLine(lpInputToP, &inputInfo)) {

        //printf("Processing :\n \"%s\"\n", Dos9_EsToChar(lpInputToP));

        if (iSkip > 0) {

            iSkip--;
            /* skip the n-th first lines, as specifiied by
               ``skip=n'' in the command line */

        } else {

            //printf("Splitting Token !\n");

            Dos9_ForSplitTokens(lpInputToP, lpfrInfo);
            /* split the block on subtokens */

            //printf("Running Block !\n");
            Dos9_RunBlock(lpbkInfo);
            /* split the input into tokens and then run the
                block */
        }

    }

    Dos9_ForCloseInputInfo(&inputInfo);
    Dos9_ForVarUnassign(lpfrInfo);

    Dos9_EsFree(lpInputToP);

    return 0;

    error:
        Dos9_EsFree(lpInputToP);

        return -1;

}

/* ************************************************************
    FORINFO functions
*/

int Dos9_ForMakeInfo(char* lpOptions, FORINFO* lpfiInfo)
/*
    Fill the FORINFO structure with the appropriate
    informations
*/
{
    ESTR* lpParam=Dos9_EsInit();
    char* lpToken;
    int iReturn=0;

    while ((lpOptions = Dos9_GetNextParameterEs(lpOptions, lpParam)))
    {

        if (!(stricmp(Dos9_EsToChar(lpParam), "usebackq"))) {
            /* if the script specifies "usebackq" it will change dos9 behaviour
            of the for /f loop */
            lpfiInfo->bUsebackq=TRUE;

            continue;

        }



        if ((lpToken = strchr(Dos9_EsToChar(lpParam), '='))) {

            *lpToken='\0';
            lpToken++;

        } else {
            /* if no '=' was found, then the entry are just
               wrong */
            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
            goto error;
        }

        /* switching to choice the appropriate parameter */

        if (!(stricmp(Dos9_EsToChar(lpParam), "delims"))) {

            Dos9_ForAdjustParameter(lpOptions, lpParam);

            strncpy(lpfiInfo->lpDelims, lpToken, sizeof(lpfiInfo->lpDelims));

            lpfiInfo->lpDelims[sizeof(lpfiInfo->lpDelims)-1] = '\0';
                /* see C89 or later standard */


        } else if (!(stricmp(Dos9_EsToChar(lpParam), "skip"))) {

            lpfiInfo->iSkip=atoi(lpToken);


        } else if (!(stricmp(Dos9_EsToChar(lpParam), "eol"))) {

            Dos9_ForAdjustParameter(lpOptions, lpParam);
            strncpy(lpfiInfo->lpEol, lpToken, sizeof(lpfiInfo->lpEol));

            lpfiInfo->lpEol[sizeof(lpfiInfo->lpEol)-1] = '\0';
                /* see C89 or later standard */

        } else if (!(stricmp(Dos9_EsToChar(lpParam), "tokens"))) {

            /* make the options */
            if (Dos9_ForMakeTokens(lpToken, lpfiInfo)==-1) {

                goto error;

            }



        } else {

            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
            goto error;

        }

    }


    Dos9_EsFree(lpParam);

    return iReturn;

    error:
        Dos9_EsFree(lpParam);
        return -1;

}

int  Dos9_ForMakeTokens(char* lpToken, FORINFO* lpfrInfo)
{
    /* this is used to determine the useffull tokens for
       the parsing */

    char* const lpOriginTok=lpToken;
    char* lpNextToken;
    int iTokenNb;
    int isCat=FALSE;
    int iPrevTok;
    int i=0;

    lpfrInfo->lpToken[0]=0;

    while (lpToken) {

        iTokenNb=strtol(lpToken, &lpNextToken, 10);

        switch (*lpNextToken) {

            case ',':
                /* just save the token to high low order word */

                if (!isCat) {

                    lpfrInfo->lpToken[i]|=TOHIGH(iTokenNb)+TOLOW(iTokenNb);

                } else {

                    lpfrInfo->lpToken[i]|=TOLOW(iTokenNb);
                    isCat=FALSE;
                    break;
                }

                if (i >= TOKENINFO_NB_MAX ) {

                    Dos9_ShowErrorMessage(DOS9_FOR_TOKEN_OVERFLOW, (char*)TOKENINFO_NB_MAX, FALSE);
                    return -1;

                }



                lpfrInfo->lpToken[++i]=0;

                lpNextToken++;

                break;


            case '-':

                if (isCat) {

                    Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER, lpOriginTok, FALSE);
                    return -1;

                }


                isCat=TRUE;

                lpfrInfo->lpToken[i]|=TOHIGH(iTokenNb);

                lpNextToken++;

                break;

            case '\0':

                if (iTokenNb) {

                    if (!isCat) {

                        lpfrInfo->lpToken[i]=TOHIGH(iTokenNb)+TOLOW(iTokenNb);

                    } else {

                        lpfrInfo->lpToken[i]|=TOLOW(iTokenNb);

                    }

                } else {

                    if (isCat) {

                        Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER, lpOriginTok, FALSE);
                        return -1;

                    }

                }


                lpfrInfo->lpToken[++i]=0;

                lpNextToken=NULL;
                break;

            case '*':

                /* some unlawful syntax as allowed

                    4-* -> in this case isCat is TRUE
                    4* -> in this case isCat is set to false and iTokenNb=4
                    3,* ->

                */

                if (!isCat) {

                    if (iTokenNb != 0) {

                        lpfrInfo->lpToken[i]|=TOHIGH(iTokenNb);


                    } else if (i > 0) {

                        iPrevTok=LOWWORD(lpfrInfo->lpToken[i-1]);

                        if (iPrevTok == -1) iPrevTok=0;


                        lpfrInfo->lpToken[i]|=TOHIGH(iPrevTok+1);

                    } else {

                        lpfrInfo->lpToken[i]|=TOHIGH(1);

                    }

                }

                lpfrInfo->lpToken[i]|=TOLOW(ALL_TOKEN);

                lpfrInfo->lpToken[++i]=0;

                lpNextToken++;

                if (!*lpNextToken) break;

                if (*lpNextToken!=',') {

                    Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER, lpNextToken, FALSE);
                    return -1;

                }

                lpNextToken++;

                break;

            default:

                Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
                return -1;

        }

        lpToken=lpNextToken;

    }


    lpfrInfo->iTokenNb=i;


    return 0;

}

void Dos9_ForAdjustParameter(char* lpOptions, ESTR* lpParam)
{
    char lpTemp[2]={0,0};


    /* this is old-style options specifications ie,
           specifying every options on the same argument
           like

            "tokens=1,2 delims=,; eol=,#"

     */


    if (*lpOptions != '\0') {

       if (*(lpOptions+1) == ' ' || *(lpOptions+1) == '\t') {

            lpTemp[0]=*lpOptions;

            Dos9_EsCpy(lpParam, lpTemp);

            if (*(lpOptions+2) == ' ' || *(lpOptions+2) == '\t') {

                lpTemp[0]=*lpOptions;

                Dos9_EsCpy(lpParam, lpOptions);

            }
       }
    }
}


/* ************************************************************
    tokenization function
*/


void Dos9_ForSplitTokens(ESTR* lpContent, FORINFO* lpfrInfo)
{

    ESTR* lpVarContent=Dos9_EsInit();

    int i;
    char *lpToken,
         *lpEol=lpfrInfo->lpEol;

    char cVarName=lpfrInfo->cFirstVar;


    if ((lpToken=strpbrk(Dos9_EsToChar(lpContent), lpEol))) {

        *lpToken='\0';

    }
    /* Truncate line at the first apparition of a end-of-line
       charcaters

       As specified by ``eol=;'' for example

       */

    for (i=0; i<lpfrInfo->iTokenNb; i++) {


         //printf("Getting token n. %d \n", i);
         Dos9_ForGetToken(lpContent, lpfrInfo, i, lpVarContent);
         // printf("Returned : \"%s\"\n", Dos9_EsToChar(lpVarContent));
            /* get the token descibed by the token info number i */

         Dos9_SetLocalVar(lpvLocalVars, cVarName, Dos9_EsToChar(lpVarContent));
            /* set the variable */

         cVarName++;

         if (cVarName & 0x80) {

            cVarName=0x21;

         }

    }

    Dos9_EsFree(lpVarContent);

}

void Dos9_ForGetToken(ESTR* lpContent, FORINFO* lpfrInfo, int iPos, ESTR* lpReturn)
{
    char  *lpToken=Dos9_EsToChar(lpContent),
          *lpDelims=lpfrInfo->lpDelims,
          *lpNextToken,
          *lpBeginToken=NULL,
          *lpEndToken=NULL;

    int   iLength,
          iTokenPos=1,
          iTokenBegin=HIGHWORD(lpfrInfo->lpToken[iPos]),
          iTokenEnd=LOWWORD(lpfrInfo->lpToken[iPos]);

    Dos9_EsCpy(lpReturn, "");

    /*  the type of line that arrives here is already truncated
        at the first character specified in eof parameter */

    /* printf("Getting token nb. %d\n"
           "\t* iTokenBegin = %d\n"
           "\t* iTokenEnd   = %d\n"
           "\t* lpDelims    = \"%s\"\n\n", iPos, iTokenBegin, iTokenEnd, lpDelims);
    */

    while ((lpNextToken = strpbrk(lpToken, lpDelims)))
    {
        /* get next first occurence of a lpDelims character */

        if (lpNextToken == lpToken) {

            /* this is already a delimiter
               just ignore it */

               lpToken=lpNextToken+1;

        }

        /* we skipped all delimiters we are now in
           processing */

        if (iTokenPos == iTokenBegin) {

            lpBeginToken=lpToken;

        }

        if (iTokenPos == iTokenEnd) {

            lpEndToken=lpNextToken;
            break;

        }

        iTokenPos++;
        lpToken=lpNextToken+1;

    }

    if ((iTokenPos==iTokenBegin) && !lpBeginToken) {

        lpBeginToken=lpToken;
    }


    if (lpBeginToken) {

        if (lpEndToken) {

            iLength = lpEndToken - lpBeginToken;

            Dos9_EsCpyN(lpReturn, lpBeginToken, iLength);


        } else {

            Dos9_EsCpy(lpReturn, lpBeginToken);

        }

    }


}

void Dos9_ForVarUnassign(FORINFO* lpfrInfo)
{
    int i,
        iMax=lpfrInfo->iTokenNb;
    char cVarName=lpfrInfo->cFirstVar;

    for (i=0;i<iMax;i++) {

        Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);

        if (cVarName & 0x80)
            break;
    }
}

int Dos9_ForVarCheckAssignment(FORINFO* lpfrInfo)
{
    int i,
        iMax=lpfrInfo->iTokenNb;
    char cVarName=lpfrInfo->cFirstVar;

    for (i=0;i<iMax;i++) {

        if ((Dos9_GetLocalVarPointer(lpvLocalVars, cVarName))) {

            Dos9_ShowErrorMessage(DOS9_FOR_TRY_REASSIGN_VAR, (char*)((int)cVarName), FALSE);
            return -1;

        }

        if (cVarName & 0x80)
            break;
    }

    return 0;

}

/* ******************************************************************
   INPUTINFO Functions
*/

int Dos9_ForMakeInputInfo(ESTR* lpInput, INPUTINFO* lpipInfo, FORINFO* lpfrInfo)
{
    int bUsebackq=lpfrInfo->bUsebackq,
        iInputType,
        iPipeFd[2];

    char *lpToken=Dos9_EsToChar(lpInput);


    switch (*lpToken) {

        case '\'':
            /* the given input is a command line to be executed */

            if (bUsebackq) {

                iInputType=INPUTINFO_TYPE_STRING;

            } else {

                iInputType=INPUTINFO_TYPE_COMMAND;

            }

            break;

        case '`':
            /* the given input is a command line using Usebackq */
            if (!bUsebackq) {

                Dos9_ShowErrorMessage(DOS9_FOR_USEBACKQ_VIOLATION, "`", FALSE);
                return -1;

            }

            iInputType=INPUTINFO_TYPE_COMMAND;

            break;

        case '"':
            /* the given input is a string */
            if (bUsebackq) {

                Dos9_ShowErrorMessage(DOS9_FOR_USEBACKQ_VIOLATION, "\"", FALSE);
                return -1;

            }

            iInputType=INPUTINFO_TYPE_STRING;

            break;

        default:
            /* the input given is a filename */
            iInputType=INPUTINFO_TYPE_STREAM;
    }

    if (iInputType!=INPUTINFO_TYPE_STREAM) {

        if (Dos9_ForAdjustInput(lpToken))
            return -1;

    }

    switch (iInputType) {

        case INPUTINFO_TYPE_STREAM:

            lpipInfo->cType=INPUTINFO_TYPE_STREAM;

            if (!(lpipInfo->Info.pInputFile=fopen(lpToken, "r"))) {

                Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR, lpToken, FALSE);
                return -1;

            }

            break;

        case INPUTINFO_TYPE_STRING:

            lpipInfo->cType=INPUTINFO_TYPE_STRING;

            if (!(lpipInfo->Info.StringInfo.lpString=strdup(lpToken))) {

                Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                      __FILE__ "/Dos9_MakeInputInfo()",
                                      TRUE);

            }

            lpipInfo->Info.StringInfo.lpToken=lpipInfo->Info.StringInfo.lpString;

            break;

        case INPUTINFO_TYPE_COMMAND:
            /* this is a litte bit more difficult (indeed, platform dependent)
               the program should open a pipe, print the script and then get the output

               The command is

                    Dos9 /Q /N /E /V /D n

               The /Q avoid the presentation to be printed and the title to be change

            */

            lpipInfo->cType=INPUTINFO_TYPE_COMMAND;


            if (_Dos9_Pipe(iPipeFd, 1024, O_TEXT) == -1)
            {
                Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR , __FILE__ "/Dos9_MakeInputInfo()", FALSE);
                return -1;
            }

            if (Dos9_ForInputProcess(lpInput, lpipInfo, iPipeFd)==-1) {

                close(iPipeFd[0]);
                close(iPipeFd[2]);
                return -1;

            }

    }

    return 0;

}

int Dos9_ForAdjustInput(char* lpInput)
{
    const char* lpBegin=lpInput;
    char* lpToken=lpInput+1;
    char cBeginToken=*lpInput;

    if (cBeginToken!='"' && cBeginToken!='\'' && cBeginToken!='`') {

        Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER, lpBegin, FALSE);

        return -1;

    }

    while (*lpToken && *lpToken!=cBeginToken)
        *(lpInput++)=*(lpToken++);

    *lpInput='\0';

    if (*lpToken==cBeginToken) {

        return 0;

    } else {

        Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER, lpBegin, FALSE);

        return -1;

    }

}

int Dos9_ForInputProcess(ESTR* lpInput, INPUTINFO* lpipInfo, int* iPipeFd)
{
    char* lpArgs[8];
    char lpLastArg[16];
    FILE* pFile;

    int i=0;

    lpArgs[i++]="Dos9";
    lpArgs[i++]="tst.bat";
    lpArgs[i++]="/Q";

    if (!bEchoOn)
        lpArgs[i++]="/E";

    if (bDelayedExpansion)
        lpArgs[i++]="/V";

    if (bDos9Extension)
        lpArgs[i++]="/N";

    //snprintf(lpLastArg, sizeof(lpLastArg),  "/D %d", iPipeFd[0]);
        /* give the input descriptor to the child*/

    lpArgs[i]=NULL;

    #ifdef WIN32

        spawnvp(_P_NOWAIT, lpArgs[0], (DOS9_SPAWN_CAST)lpArgs);

        if (errno == ENOENT)
        {

            Dos9_ShowErrorMessage(DOS9_FOR_LAUNCH_ERROR | DOS9_PRINT_C_ERROR, Dos9_EsToChar(lpInput), FALSE);

            return -1;
        }

    #elif defined _POSIX_C_SOURCE

        iPid=fork();
        if (iPid == 0 ) {
          /* if we are in the son */

          if ( execvp(lpArguments[0], lpArguments) == -1) {

            Dos9_ShowErrorMessage(DOS9_FOR_LAUNCH_ERROR | DOS9_PRINT_C_ERROR, Dos9_EsToChar(lpInput), FALSE);
            return -1;

          }

          exit(0);

        } else {
          /* if we are in the father */

          if (iPid == (pid_t)-1) {

            /* the execution failed */
            Dos9_ShowErrorMessage(DOS9_FOR_LAUNCH_ERROR | DOS9_PRINT_C_ERROR, Dos9_EsToChar(lpInput), FALSE);
            return -1;

          }

        }

    #endif // WIN32

    if (!(pFile=fdopen(iPipeFd[1], "r")))
    {

        return -1;

    }

    fputs(Dos9_EsToChar(lpInput), pFile);
        /* write the command given on the for command */

    lpipInfo->Info.StreamInfo.pInputStream=pFile;
    lpipInfo->Info.StreamInfo.iPipeFds[1]=iPipeFd[1];
    lpipInfo->Info.StreamInfo.iPipeFds[0]=iPipeFd[0];

    return 0;
}

int Dos9_ForGetStringInput(ESTR* lpReturn, STRINGINFO* lpsiInfo)
{
    char *lpToken=lpsiInfo->lpToken,
         *lpBeginToken=lpToken;
    size_t iLength;

    if (!*lpToken)
        return 1;

    while (*lpToken && *lpToken!='\n') lpToken++;

    iLength = lpToken - lpBeginToken;

    Dos9_EsCpyN(lpReturn, lpBeginToken, iLength);

    if (*lpToken)
        lpToken++;

    lpsiInfo->lpToken=lpToken;

    return 0;

}

int Dos9_ForGetInputLine(ESTR* lpReturn, INPUTINFO* lpipInfo)
{

    int iReturn=0;
    char* lpToken;

    switch (lpipInfo->cType) {

        case INPUTINFO_TYPE_COMMAND:
            /*
                iReturn=!Dos9_EsGet(lpReturn, lpipInfo->Info.StreamInfo.pInputStream);
            */

            iReturn=0;
            break;

        case INPUTINFO_TYPE_STREAM:
            iReturn=!Dos9_EsGet(lpReturn, lpipInfo->Info.pInputFile);
            break;

        case INPUTINFO_TYPE_STRING:
            iReturn=!Dos9_ForGetStringInput(lpReturn, &(lpipInfo->Info.StringInfo));

    }

    if ((lpToken=strchr(Dos9_EsToChar(lpReturn), '\n')))
        *lpToken='\0';

    return iReturn;
}

void Dos9_ForCloseInputInfo(INPUTINFO* lpipInfo)
{

    switch(lpipInfo->cType) {

        case INPUTINFO_TYPE_STREAM:

            fclose(lpipInfo->Info.pInputFile);
            break;


        case INPUTINFO_TYPE_STRING:

            free(lpipInfo->Info.StringInfo.lpString);
            break;

        case INPUTINFO_TYPE_COMMAND:
            fclose(lpipInfo->Info.StreamInfo.pInputStream);
            close(lpipInfo->Info.StreamInfo.iPipeFds[0]);
            close(lpipInfo->Info.StreamInfo.iPipeFds[1]);

    }

}
