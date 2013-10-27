#include <stdio.h>
#include <stdlib.h>

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
         " \n,;", /* the default line-break characters
                   */
         "", /* no tokens delimiters, since only one
                token is taken account */
         "", /* no end-of-line delimiters */
         0, /* no skipped line */
         0, /* this is to be fullfiled later (the
                name letter of loop special var) */
         {TOKENNB_ALL_REMAINING} /* get all the token
                                    back */

     };

     char  cVarName;
     char* lpToken=lpLine+3;
     char* lpVarName;

     int iForType=FOR_LOOP_SIMPLE;
     int iUsebackq=FALSE;

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

            if (lpVarName=='%') lpVarName++;

            cVarName=*lpVarName;

            /* test conformance

            FIXME : This is a memory sailing away to sea */
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
                    switch (Dos9_ForMakeInfo(Dos9_EsToChar(lpParam), &forInfo)) {

                        case -1:
                            goto error;

                        case 1:
                            iUsebackq=TRUE;
                    }
            }

            lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

         }

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

     if ((lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

        Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);

     }

     switch(iForType){

        case FOR_LOOP_SIMPLE:

            /* this handles simple for */
            Dos9_CmdForSimple(lpInputBlock, &bkCode, cVarName);

        case FOR_LOOP_R:
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

        if (!(stricmp(Dos9_EsToChar(lpParam), "usebackq")))
            /* if the script specifies "usebackq" it will change dos9 behaviour
            of the for /f loop */
            iReturn=1;



        if ((lpToken = strchr(Dos9_EsToChar(lpParam), '='))) {

            *lpToken='\0';
            lpToken++;

        } else {
            /* if no '=' was found, then the entry are just
               wrong */
            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
            Dos9_EsFree(lpParam);
            return -1;
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

            /* if (Dos9_ForMakeTokens(lpToken, lpfiInfo)) {

            } */



        } else {

            Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
            Dos9_EsFree(lpParam);
            return -1;

        }

    }

    Dos9_EsFree(lpParam);

    return iReturn;

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

            Dos9_EsCpy(lpParam, lpOptions);

            if (*(lpOptions+2) == ' ' || *(lpOptions+2) == '\t') {

                lpTemp[0]=*lpOptions;

                Dos9_EsCpy(lpParam, lpOptions);

            }
       }
    }
}
