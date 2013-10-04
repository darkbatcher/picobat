/* Dos9_Expand.c - Dos9 copyleft (c) DarkBatcher 2012 - Some rights reserved

   The following file contain definition of funtions that are used for expansion of vars
   (including delayed expansion) and some functions used for parsing these variable.

 */

#include "Dos9_Core.h"

static ESTR* lpExpanded;
static ESTR* lpVarContent;

void Dos9_ExpandInit(void)
{
    lpExpanded=Dos9_EsInit();
    lpVarContent=Dos9_EsInit();
}

void Dos9_ExpandEnd(void)
{
    Dos9_EsFree(lpExpanded);
    Dos9_EsFree(lpVarContent);
}

/* function for replacing variable on commands line  */
void Dos9_ReplaceVars(ESTR* ptrCommandLine)
{
    char* ptrToken=Dos9_EsToChar(ptrCommandLine), *ptrNextToken, *ptrEndToken;
    /* initialisation du buffer de sortie */
    Dos9_EsCpy(lpExpanded,"");
    while ((ptrNextToken=Dos9_StrToken(ptrToken, '%'))) {
        DEBUG(ptrToken);
        *ptrNextToken='\0';
        ptrNextToken++; // on passe au caractère suivant
        if (*ptrNextToken=='%') { // si un % est échappé via %%
            Dos9_EsCat(lpExpanded, ptrToken);
            Dos9_EsCat(lpExpanded, "%");
            ptrToken=ptrNextToken+1;
            continue;
        } else if ((ptrEndToken=Dos9_StrToken(ptrNextToken, '%'))) {
            *ptrEndToken='\0';
            if ((Dos9_GetVar(ptrNextToken, lpVarContent))) {
                Dos9_EsCat(lpExpanded, ptrToken);
                Dos9_EsCatE(lpExpanded, lpVarContent);
                ptrToken=ptrEndToken+1;
            } else {
                *ptrEndToken='%';
                Dos9_EsCat(lpExpanded, ptrToken);
                Dos9_EsCat(lpExpanded, "%");
                ptrToken=ptrNextToken;
            }
            continue;
        } else {
            Dos9_EsCat(lpExpanded, ptrToken); // si il y'a un seul % qui ne respecte aucune règle
            Dos9_EsCat(lpExpanded, "%");
            ptrToken=ptrNextToken;
        }
    }
    Dos9_EsCat(lpExpanded, ptrToken); // si pas de séquence détectée
    DEBUG(Dos9_EsToChar(lpExpanded));
    Dos9_EsCpy(ptrCommandLine, Dos9_EsToChar(lpExpanded));
    DEBUG(Dos9_EsToChar(ptrCommandLine));
}


int Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion)
{
    ESTR* lpExpanded=Dos9_EsInit();
    ESTR* lpVarContent=Dos9_EsInit();
    char *lpLine=Dos9_EsToChar(ptrCommandLine), *lpNextToken, *lpLastToken;
    int cSkipChar=FALSE;
    int iParentheseNb=0;
    Dos9_EsCpy(lpExpanded, "");
    lpLastToken=lpLine;
    DEBUG(lpLastToken);
    for (;*lpLine;lpLine++) {
        if (cSkipChar) {
            cSkipChar=FALSE;
            continue;
        }
        if (*lpLine==')' && iParentheseNb>0) iParentheseNb--;
        if (iParentheseNb) continue;
        switch (*lpLine) {
            case '^':
            /* the next character musst be escaped */
                cSkipChar=TRUE;
                continue;
            case '(':
                iParentheseNb++;
                continue;
            case '%':
                if ((lpNextToken=Dos9_GetLocalVar(lpvLocalVars, lpLine+1, lpVarContent))) { /* si la variable est bien définie */
                    *lpLine='\0';
                    Dos9_EsCat(lpExpanded, lpLastToken);
                    Dos9_EsCatE(lpExpanded, lpVarContent);
                    lpLastToken=lpNextToken;
                    lpLine=lpLastToken-1;
                }

            case '!':
                if ((lpNextToken=Dos9_StrToken(lpLine+1, '!')) && cEnableDelayedExpansion) {
                    /* these lines should implement the epansion of delayed var
                       using '!' token */
                    *lpNextToken='\0';
                    if (Dos9_GetVar(lpLine+1, lpVarContent)) {
                        *lpLine='\0';
                        Dos9_EsCat(lpExpanded, lpLastToken);
                        Dos9_EsCatE(lpExpanded, lpVarContent);
                        lpLastToken=lpNextToken+1;
                        lpLine=lpLastToken-1;
                    } else {
                        *lpNextToken='!';
                    }
                }
        }
    }
    DEBUG("Copying that fucking line !");
    DEBUG(lpLastToken);
    Dos9_EsCat(lpExpanded, lpLastToken); /* note : the order of these functiond is quite important,
                                                    indeed lpLastToken comes from the buffer ptrCommandline
                                                    and thus, it can't be directly copyed on this buffer */
    Dos9_EsCpyE(ptrCommandLine, lpExpanded);
    DEBUG(Dos9_EsToChar(ptrCommandLine));
    return 0;
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
