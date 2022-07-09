/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2016 Romain GARBI
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
#include <ctype.h>

#include "pBat_Core.h"

#include "../errors/pBat_Errors.h"

//#define PBAT_DBG_MODE
#include "pBat_Debug.h"
void pBat_ExpandSpecialVar(ESTR* ptrCommandLine, ESTR** buf)
{

	ESTR *lpVarContent=buf[0],
         *lpExpanded=buf[1];

	char *lpToken=pBat_EsToChar(ptrCommandLine),
	      *lpNextToken,
	      *lpPreviousToken=lpToken,
	       *lpTokenBegin;

	while ((lpNextToken=pBat_SearchChar(lpToken, '%'))) {

		lpTokenBegin=lpNextToken+1;

		if ((lpTokenBegin=pBat_GetLocalVar(lpvLocalVars, lpTokenBegin, lpVarContent))) {

			/* If we have a defined variable */
			*lpNextToken='\0';

			pBat_EsCat(lpExpanded, lpPreviousToken);
			pBat_EsCatE(lpExpanded, lpVarContent);

			lpToken=lpTokenBegin;
			lpPreviousToken=lpTokenBegin;

		} else {

			/* If the variable is not defined */
			lpToken=lpNextToken+1;

		}


	}

	pBat_EsCat(lpExpanded, lpPreviousToken);
	pBat_EsCpyE(ptrCommandLine, lpExpanded);

	PBAT_DBG("[ExpandSpecialVar] : \"%s\".\n", pBat_EsToChar(ptrCommandLine));

}

void pBat_ExpandVar(ESTR* ptrCommandLine, char cDelimiter, ESTR** buf)
{
	ESTR *lpExpanded=buf[0], /* To store the expanded line */
         *lpVarContent=buf[1]; /* To store temporary var content */

	char *ptrToken=ptrCommandLine->str,
	      *ptrNextToken,
	      *ptrEndToken;

    char lpDelimiter[] = {cDelimiter, '\0'};

	while ((ptrNextToken=pBat_SearchChar(ptrToken, cDelimiter))) {

		*ptrNextToken='\0';
		ptrNextToken++;

        /* If we are doing delayed expansion, remove escape characters*/
        if ((bDelayedExpansion
                && cDelimiter == '!'))
                pBat_UnEscape(ptrToken);

		if (*ptrNextToken==cDelimiter) {

            /* a delimiter is escaped using double delimiter */
			pBat_EsCat(lpExpanded, ptrToken);
			pBat_EsCat(lpExpanded, lpDelimiter);
			ptrToken=ptrNextToken+1;

		} else if ((cDelimiter == '%' && (isdigit(*ptrNextToken)
                    || *ptrNextToken == '*'
                    || *ptrNextToken == '+'
                    || *ptrNextToken=='~'))
                    && (ptrEndToken=pBat_GetLocalVar(lpvArguments,
                                                      ptrNextToken, lpVarContent))) {

			/* We encountered a parameter variable that must be
			   developed right here because it owns priority over
			   existing variables (eg, something like '2cent') whose
			   name start with a number or an asterisk */
            pBat_EsCat(lpExpanded, ptrToken);
			pBat_EsCatE(lpExpanded, lpVarContent);
			ptrToken=ptrEndToken;


		} else if ((ptrEndToken=pBat_SearchChar(ptrNextToken, cDelimiter))) {

			*ptrEndToken='\0';

            pBat_EsCat(lpExpanded, ptrToken);

            /* add the content of the variable only if
                it is really defined */
			if ((pBat_GetVar(ptrNextToken, lpVarContent)))
                pBat_EsCatE(lpExpanded, lpVarContent);

			ptrToken=ptrEndToken+1;

			continue;

		} else {

			/* there is only a single %, that does not
			   match any following %, then, split it
			   from the input.
			 */

			pBat_EsCat(lpExpanded, ptrToken);
			ptrToken=ptrNextToken;

		}
	}

    if ((bDelayedExpansion
         && cDelimiter == '!'))
        pBat_UnEscape(ptrToken);

	pBat_EsCat(lpExpanded, ptrToken);
	pBat_EsCpy(ptrCommandLine, pBat_EsToChar(lpExpanded));

}

void pBat_ReplaceVars(ESTR* lpEsStr)
{
    ESTR* buf[2] = {pBat_EsInit_Cached(), pBat_EsInit_Cached()};

    pBat_ExpandVar(lpEsStr, '%', buf);

    pBat_EsFree_Cached(buf[0]);
    pBat_EsFree_Cached(buf[1]);
}


void pBat_DelayedExpand(ESTR* ptrCommandLine)
{
	ESTR* buf[2] = { pBat_EsInit_Cached(), pBat_EsInit_Cached() };
		
	pBat_ExpandSpecialVar(ptrCommandLine, buf);
	
	*pBat_EsToChar(buf[0]) = '\0';
	*pBat_EsToChar(buf[1]) = '\0';
	
	if (bDelayedExpansion)
		pBat_ExpandVar(ptrCommandLine, '!', buf);
	else
		pBat_UnEscape(ptrCommandLine->str);
	
	pBat_EsFree_Cached(buf[0]);
	pBat_EsFree_Cached(buf[1]);
}
