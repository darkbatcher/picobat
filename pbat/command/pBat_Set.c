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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Set.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

/* internal function for libmatheval to fetch variables values */
double _pBat_SetGetVar(const char* lpName)
{
	char* lpContent;

	lpContent=pBat_GetEnv(lpeEnv, lpName);

	if (lpContent) {

		if (!strcmp(lpContent, "nan")) {

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


/* internal function for libmatheval to set variables values */
double _pBat_SetSetVar(char* name, double f)
{
	char value[FILENAME_MAX];

	sprintf(value, "%.16g", f);

	pBat_SetEnv(lpeEnv, name, value);

    return f;
}

/*

	SET [/A] [/P] var=exp

	Set an environment variable.

	- /A : select the mathematic mode ('f' for floating-point and 'i' for
	integers)

	- /P : prompts the user with exp1


	Cmd.exe' set usually behaves strangely toward quotes:

		* If the expression is quoted, just like `set var="exp1" exp2',
	what is assigned is `var="exp1" exp2'.

		* If the name and expression are quoted, just like
	`set "var=exp1" exp2', cmd.exe just look for the last quote on the line
	and strip the following characters. Thus what we get will be `var=exp1'.
	This behaviour introduces concerns about how the line is actually parsed
	because remaining characters are ignored.

	However, for set /a, the behaviour is a little bit different; Various
	variables may be defined through the use of `,' and quotes. This syntax
	is really fragile because equivalent signs may not work when replaced.
	Basically, `set "var1=1","var2=2"' may assign various variables, while
	using a space may not.

	Things are even worse in case of inline assignments using set /a.
	Problems are introduced by the lack of standard precedence rules for
	assignments. As an example, `set /a var=1+(var3=2)' evaluates to 3
	whereas `set /a var=1+var3=2' evaluates to 2 (That does not make sense
	anyway, can't figure how to get 2 out of this expression).

	IMPLEMENTATION:
	===============

	pBat actually supports the following behaviours on simple set (without
	any switches) :

    The set simple mode and behaves *exactly* the same way as cmd does.

	pBat behaves exactly like cmd.exe when using the '/p' switch.

	pBat behaves mostly like cmd.exe when using the '/a' switch, except
	that inline assignment are supported but is not guaranteed to be 100%
	compatible as inline assignment are a bit complicated to understand
	under cmd. (As stated above, it may hard to parse)

*/
int pBat_CmdSet(char *lpLine)
{
	char lpArgBuf[5],
	     *lpArg=lpArgBuf;

	char *lpNextToken;

	int i,
        nok = 0,
	    status = PBAT_NO_ERROR;

	if ((lpNextToken=pBat_GetNextParameter(lpLine+3, lpArgBuf,
            sizeof(lpArgBuf))))  {

		if (!stricmp(lpArg, "/?")) {

			pBat_ShowInternalHelp(PBAT_HELP_SET);
			goto error;

		} else if (!stricmp(lpArg, "/a")) {

			if (status = pBat_CmdSetA(lpNextToken))
				goto error;

		} else if (!stricmp(lpArg, "/p")) {

			if (status = pBat_CmdSetP(lpNextToken))
				goto error;

		} else {

			/* simple set */

			nok = pBat_CmdSetS(lpLine+3);

		}

	} else {

        nok = 1;

	}

	if (nok == 1) {

		lpLine = pBat_SkipBlanks(lpLine+3);

		/* if a variable have been found,
		   this variable vary usually
		   when using a prefix
		*/
		char found = 0;

		/* in default cases, print environment
		   which match with the prefix (if defined)
		*/
		for (i=0; i < lpeEnv->index; i++) {
			if (*lpLine == '\0'
                || ((lpeEnv->envbuf[i]->name != NULL)
                    && !strnicmp(lpeEnv->envbuf[i]->name, lpLine, strlen(lpLine)))) {

				found = TRUE;

				/* variable valid */
	            fprintf(fOutput, "%s=%s" PBAT_NL, lpeEnv->envbuf[i]->name,
	                                    lpeEnv->envbuf[i]->content);
			}
		}

		if (!found) {

			pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, lpLine, FALSE);
			status = PBAT_UNEXPECTED_ELEMENT;

			goto error;

		}
	}

error:
	return status;
}

/* simple set */
int pBat_CmdSetS(char* lpLine)
{
	ESTR* lpEsVar=pBat_EsInit();

	char *lpCh,
		*lpBegin,
		*lpEnd;

	while (lpLine) {

		lpLine=pBat_SkipBlanks(lpLine);

		if (*lpLine=='\0'){

			break;

		} else {

			/* use the good old method of cmd.exe' set
			   truncate the line at the last quote
			*/

			pBat_GetEndOfLine(lpLine, lpEsVar);

            /* Strip any pair of '"' if encountered */
			if (*(lpEsVar->str) == '"'
                && (lpEnd = strrchr(lpEsVar->str + 1, '"'))) {

                *lpEnd = '\0';
                lpBegin = lpEsVar->str + 1;

            } else {

                lpBegin = lpEsVar->str;

            }

			lpLine=NULL;


		}

		if (!(lpCh=strchr(lpBegin, '='))) {

			/* Well, apparently this line is not well constructed,
			   however, we do not display error, and backtrack to
			   the original function  */

			pBat_EsFree(lpEsVar);
			return 1;

		}

		*lpCh='\0';
		lpCh++;

		pBat_SetEnv(lpeEnv, lpBegin, lpCh);

	}

	pBat_EsFree(lpEsVar);
	return 0;
}

/* Function for set /p */
int pBat_CmdSetP(char* lpLine)
{

	ESTR* lpEsVar=pBat_EsInit();
	ESTR* lpEsInput=pBat_EsInit();
	char* lpEqual;
	int status = PBAT_NO_ERROR;

	lpLine = pBat_SkipBlanks(lpLine);

    /* If the expression starts with an ", use get parameter
       to process the token in a clean way */
	if (*lpLine == '"') {

        pBat_GetNextParameterEs(lpLine, lpEsVar);

    } else {

        pBat_GetEndOfLine(lpLine, lpEsVar);

    }


	if ((lpEqual=strchr(pBat_EsToChar(lpEsVar), '='))) {

		*lpEqual='\0';
		lpEqual++;

		fprintf(fOutput, "%s", lpEqual);


#if !defined(WIN32) && !defined(PBAT_NO_LINENOISE)
        /* If fInput is a tty, use Linenoise ! */
		if (isatty(fileno(fInput)))
            pBat_LineNoise(lpEsInput, fInput);
        else /* This may look to be a bug, but it's intentional */
#endif
            pBat_EsGet(lpEsInput, fInput);

		if ((lpEqual=strchr(pBat_EsToChar(lpEsInput), '\n')))
			*lpEqual='\0';

		pBat_SetEnv(lpeEnv, pBat_EsToChar(lpEsVar),
				pBat_EsToChar(lpEsInput));

	} else {

		pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT,
		                      pBat_EsToChar(lpEsVar),
		                      FALSE);

        status = PBAT_UNEXPECTED_ELEMENT;

		goto error;

	}

error:
	pBat_EsFree(lpEsVar);
	pBat_EsFree(lpEsInput);
	return status;
}

int pBat_CmdSetA(char* lpLine)
{

	ESTR* lpExpression=pBat_EsInit();
	int status = PBAT_NO_ERROR;

	lpLine=pBat_SkipBlanks(lpLine);

	while ((lpLine=pBat_GetNextParameterEsD(lpLine, lpExpression , "\","))
            && !(status = pBat_CmdSetEval(lpExpression)))
                /* loop through expressions */;

error:
	pBat_EsFree(lpExpression);
	return status;
}

int pBat_CmdSetEval(ESTR* lpExpression)
{
	void* evaluator; /* an evaluator for libmatheval-pBat */
	char *lpVarName,
	     *lpEqual,
	     lpResult[50];
	char  cLeftAssign=0;
	double dResult,
	       dVal;
    int status = PBAT_NO_ERROR,
        bDouble = 0,
        fmode;

	lpVarName=pBat_EsToChar(lpExpression);

	lpVarName=pBat_SkipBlanks(lpVarName);

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "SET", FALSE);
		return PBAT_EXPECTED_MORE;

	}

	/* seek an '=' sign */
	if (!(lpEqual=strchr(lpVarName, '='))) {

		pBat_ShowErrorMessage(PBAT_INVALID_EXPRESSION, lpVarName, FALSE);
		return PBAT_INVALID_EXPRESSION;

	}

	*lpEqual='\0';

	/* seek a sign like '+=', however, '' might be a valid environment
	   variable name depending on platform */
	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

    if (pBat_LockMutex(&mSetLock))
        pBat_ShowErrorMessage(PBAT_LOCK_MUTEX_ERROR,
                              __FILE__ "/pBat_CmdSetEvalFloat()" , -1);

	/* create evaluator */
	if (!(evaluator=evaluator_create(lpEqual+1, &fmode, lpstSymbols))) {

		pBat_ShowErrorMessage(PBAT_INVALID_EXPRESSION, lpEqual+1, FALSE);
		status = PBAT_INVALID_EXPRESSION;

		goto error;

	}

    evaluator_set_functions(_pBat_SetGetVar, _pBat_SetSetVar);
	dResult=evaluator_evaluate(evaluator);

	evaluator_destroy(evaluator);

	/* clear if operator is recognized */

#define OPERATE_INT(op1, op, op2) \
    (double)(((int)op1) op ((int)(op2)))

#define OPERATE(op1, op, op2, mode) \
    ((!mode) ? \
        (OPERATE_INT(op1, op, op2)) \
        : \
        (op1 op op2))


	switch (cLeftAssign) {

		case '*':
		case '/':
		case '+':
		case '-':
        case '%':
        case '^':
			*(lpEqual-1)='\0';
			/* get the value of the variable */
			pBat_AdjustVarName(lpVarName);
			dVal=_pBat_SetGetVar(lpVarName);

			switch(cLeftAssign) {

				case '*':
					dVal*=dResult;
					break;

				case '/':
					dVal = OPERATE(dVal, /,
                                    dResult, fmode);
					break;

                case '%':
                    dVal = fmod(dVal, dResult);
                    break;

				case '+':
					dVal+=dResult;
					break;

				case '-':
					dVal-=dResult;
					break;

                case '^':
                    dVal = OPERATE_INT(dVal, ^, dResult);
                    break;


			}

			break;

		case '&':
		case '|':

			/* more complicated, it need to be
			   resolved */

			*(lpEqual-1)='\0';

			pBat_AdjustVarName(lpVarName);
			dVal=_pBat_SetGetVar(lpVarName);

			if (lpVarName != (lpEqual-1)) {

				if (*(lpEqual-2) == cLeftAssign) {

					bDouble=1;
					*(lpEqual-2)='\0';

				}

			}

			switch (cLeftAssign) {

				case '|':
					if (bDouble)
                        dVal = OPERATE_INT(dVal, ||, dResult);
                    else
                        dVal = OPERATE_INT(dVal, |, dResult);

					break;

				case '&':
					if (bDouble)
                        dVal = OPERATE_INT(dVal, &&, dResult);
                    else
                        dVal = OPERATE_INT(dVal, &, dResult);
			}

			break;


		default:
			dVal=dResult;

	}

	snprintf(lpResult, sizeof(lpResult), "%.14g", dVal);

    if (!bIsScript)
        fprintf(fOutput, "%.14g", dVal);

	pBat_SetEnv(lpeEnv, pBat_EsToChar(lpExpression), lpResult);

error:
    if (pBat_ReleaseMutex(&mSetLock))
        pBat_ShowErrorMessage(PBAT_RELEASE_MUTEX_ERROR,
                              __FILE__ "/pBat_CmdSetEval()" , -1);

	return status;
}
