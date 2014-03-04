/* Dos9_Expand.c - Dos9 copyleft (c) DarkBatcher 2012 - Some rights reserved

   The following file contain definition of funtions that are used for expansion of vars
   (including delayed expansion) and some functions used for parsing these variable.

 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

// #define DOS9_DBG_MODE
#include "Dos9_Debug.h"

/* function for replacing variable on commands line  */
void Dos9_ReplaceVars(ESTR* ptrCommandLine)
{
    Dos9_ExpandVar(ptrCommandLine, '%');
}

void Dos9_ExpandSpecialVar(ESTR* ptrCommandLine)
{

    ESTR* lpVarContent=Dos9_EsInit();
    ESTR* lpExpanded=Dos9_EsInit();

    char *lpToken=Dos9_EsToChar(ptrCommandLine),
         *lpNextToken,
         *lpPreviousToken=lpToken,
         *lpTokenBegin;

    DOS9_DBG("[+]Dos9_ExpandSpecialVar: \"%s\".\n", Dos9_EsToChar(ptrCommandLine));

    while ((lpNextToken=Dos9_StrToken(lpToken, '%'))) {

        lpTokenBegin=lpNextToken+1;

        if ((lpTokenBegin=Dos9_GetLocalVar(lpvLocalVars, lpTokenBegin, lpVarContent))) {

            /* si la variable est bien définie */
            *lpNextToken='\0';

            Dos9_EsCat(lpExpanded, lpPreviousToken);
            Dos9_EsCatE(lpExpanded, lpVarContent);

            lpToken=lpTokenBegin;
            lpPreviousToken=lpTokenBegin;

        } else {

            /* si la variable n'est pas définie */
            lpToken=lpNextToken+1;

        }


    }

    Dos9_EsCat(lpExpanded, lpPreviousToken);
    Dos9_EsCpyE(ptrCommandLine, lpExpanded);

    Dos9_EsFree(lpVarContent);
    Dos9_EsFree(lpExpanded);

    DOS9_DBG("[-]Dos9_ExpandSpecialVar: \"%s\".\n", Dos9_EsToChar(ptrCommandLine));

}

void Dos9_ExpandVar(ESTR* ptrCommandLine, char cDelimiter)
{

    ESTR* lpExpanded=Dos9_EsInit();
    ESTR* lpVarContent=Dos9_EsInit();

    char *ptrToken=Dos9_EsToChar(ptrCommandLine),
         *ptrNextToken,
         *ptrEndToken;

    char lpDelimiter[3]={cDelimiter, 0, 0};

    /* initialisation du buffer de sortie */
    Dos9_EsCpy(lpExpanded,"");

    while ((ptrNextToken=Dos9_StrToken(ptrToken, cDelimiter))) {

        DEBUG(ptrToken);
        *ptrNextToken='\0';
        ptrNextToken++; // on passe au caractère suivant

        if ((*ptrNextToken==cDelimiter)) {

            // si un % est échappé via %%

            /* on supprime le caractère qui peut éventuellement
               trainer dans ce buffer */
            lpDelimiter[1]='\0';

            Dos9_EsCat(lpExpanded, ptrToken);
            Dos9_EsCat(lpExpanded, lpDelimiter);
            ptrToken=ptrNextToken+1;
            continue;

        } else if (isdigit(*ptrNextToken)) {

            /* if the variable is one of the parameter variables,
               then skip, for compatibility purpose
             */
            lpDelimiter[1]=*ptrNextToken;
            Dos9_EsCat(lpExpanded, ptrToken);
            Dos9_EsCat(lpExpanded, lpDelimiter);
            ptrToken=ptrNextToken+1;
            continue;


        } else if ((ptrEndToken=Dos9_StrToken(ptrNextToken, cDelimiter))) {

            *ptrEndToken='\0';

            if ((Dos9_GetVar(ptrNextToken, lpVarContent))) {

                Dos9_EsCat(lpExpanded, ptrToken);
                Dos9_EsCatE(lpExpanded, lpVarContent);
                ptrToken=ptrEndToken+1;

            }

            continue;

        } else {

            /* there is only a single %, that does not
               match any following %, then, split it
               from the input.
             */

            Dos9_EsCat(lpExpanded, ptrToken);
            ptrToken=ptrNextToken;

        }
    }

    Dos9_EsCat(lpExpanded, ptrToken); // si pas de séquence détectée
    DEBUG(Dos9_EsToChar(lpExpanded));
    Dos9_EsCpy(ptrCommandLine, Dos9_EsToChar(lpExpanded));
    DEBUG(Dos9_EsToChar(ptrCommandLine));

    Dos9_EsFree(lpExpanded);
    Dos9_EsFree(lpVarContent);

}

void Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion)
{
    Dos9_ExpandSpecialVar(ptrCommandLine);

    if (cEnableDelayedExpansion) {

        Dos9_ExpandVar(ptrCommandLine, '!');

    }
}

void Dos9_RemoveEscapeChar(char* lpLine)
{
    /* this function is designed to remove the escape characters
       (e.g. char '^') from the command line */
    char  lastEsc=FALSE;
    char* lpPosition=lpLine;
    for (;*lpLine;lpLine++,lpPosition++)
    {
        if (*lpLine=='^' && lastEsc!=TRUE) {
            lpPosition--;
            lastEsc=TRUE;
            continue;
        }
        lastEsc=FALSE;
        if (lpPosition != lpLine) *lpPosition=*lpLine;
    }
    *lpPosition='\0';
}

char* Dos9_StrToken(char* lpString, char cToken)
{
    if (lpString == NULL)
        return NULL;

    for (;*lpString!='\0';lpString++) {

            if (*lpString==cToken) return lpString;

    }

    return NULL;

}
