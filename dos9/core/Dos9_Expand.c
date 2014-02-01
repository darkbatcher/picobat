/* Dos9_Expand.c - Dos9 copyleft (c) DarkBatcher 2012 - Some rights reserved

   The following file contain definition of funtions that are used for expansion of vars
   (including delayed expansion) and some functions used for parsing these variable.

 */

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

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

    while ((lpNextToken=Dos9_StrToken(lpToken, '%'))) {

        lpTokenBegin=lpNextToken+1;

        if ((lpTokenBegin=Dos9_GetLocalVar(lpvLocalVars, lpTokenBegin, lpVarContent))) {

            /* si la variable est bien d�finie */
            *lpNextToken='\0';

            Dos9_EsCat(lpExpanded, lpPreviousToken);
            Dos9_EsCatE(lpExpanded, lpVarContent);

            lpToken=lpTokenBegin;
            lpPreviousToken=lpTokenBegin;

        } else {

            /* si la variable n'est pas d�finie */
            lpToken=lpNextToken+1;

        }


    }

    Dos9_EsCat(lpExpanded, lpPreviousToken);
    Dos9_EsCpyE(ptrCommandLine, lpExpanded);

    Dos9_EsFree(lpVarContent);
    Dos9_EsFree(lpExpanded);

}

void Dos9_ExpandVar(ESTR* ptrCommandLine, char cDelimiter)
{

    ESTR* lpExpanded=Dos9_EsInit();
    ESTR* lpVarContent=Dos9_EsInit();

    char *ptrToken=Dos9_EsToChar(ptrCommandLine),
         *ptrNextToken,
         *ptrEndToken;

    char lpDelimiter[2]={cDelimiter, 0};

    /* initialisation du buffer de sortie */
    Dos9_EsCpy(lpExpanded,"");

    while ((ptrNextToken=Dos9_StrToken(ptrToken, cDelimiter))) {

        DEBUG(ptrToken);
        *ptrNextToken='\0';
        ptrNextToken++; // on passe au caract�re suivant

        if (*ptrNextToken==cDelimiter) {

            // si un % est �chapp� via %%
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

            } else {

                *ptrEndToken=cDelimiter;
                Dos9_EsCat(lpExpanded, ptrToken);
                Dos9_EsCat(lpExpanded, lpDelimiter);
                ptrToken=ptrNextToken;

            }

            continue;

        } else {

            Dos9_EsCat(lpExpanded, ptrToken); // si il y'a un seul % qui ne respecte aucune r�gle
            Dos9_EsCat(lpExpanded, lpDelimiter);
            ptrToken=ptrNextToken;

        }
    }

    Dos9_EsCat(lpExpanded, ptrToken); // si pas de s�quence d�tect�e
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

    for (;*lpString!='\0';lpString++) {

            if (*lpString==cToken) return lpString;

    }

    return NULL;

}
