/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Set.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"
double _Dos9_SetGetVarFloat(const char* lpName)
{
	char* lpContent;

	lpContent=getenv(lpName);

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

			Dos9_ShowInternalHelp(DOS9_HELP_SET);
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
			if (Dos9_CmdSetA(lpNextToken, bFloats))
				goto error;

		} else if (!stricmp(lpArg, "/p")) {

			if ((Dos9_CmdSetP(lpNextToken)))
				goto error;

		} else {

			/* simple set */

			if ((Dos9_CmdSetS(lpLine+3)))
				goto error;

		}

	} else {

		/* in default cases, print environment */
		for (i=0; environ[i]; i++) puts(environ[i]);

	}

	return 0;

error:
	return -1;
}

/* simple set */
int Dos9_CmdSetS(char* lpLine)
{
	ESTR* lpEsVar=Dos9_EsInit();

	char* lpCh;

	lpLine=Dos9_SkipBlanks(lpLine);

	Dos9_GetEndOfLine(lpLine, lpEsVar);

	/* fixme : this is a hack that will not work fine on
	   Other oses than MS-WINDOWS */

	if (!(lpCh=strchr(Dos9_EsToChar(lpEsVar), '='))) {

		Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
								Dos9_EsToChar(lpEsVar),
								FALSE);

		goto error;

	}

	*lpCh='\0';
	lpCh++;

	if (Dos9_setenv(Dos9_EsToChar(lpEsVar), lpCh)) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
		                      Dos9_EsToChar(lpEsVar),
		                      FALSE);
		goto error;

	}

	Dos9_EsFree(lpEsVar);
	return 0;

error:
	Dos9_EsFree(lpEsVar);
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

		*lpEqual='\0';
		lpEqual++;

		puts(lpEqual);

		Dos9_EsGet(lpEsInput, stdin);

		if ((lpEqual=strchr(Dos9_EsToChar(lpEsInput), '\n')))
			*lpEqual='\0';

		if (Dos9_setenv(Dos9_EsToChar(lpEsVar), Dos9_EsToChar(lpEsInput))) {

			Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
			                      Dos9_EsToChar(lpEsVar),
			                      FALSE);

			goto error;

		}

	} else {

		Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
		                      Dos9_EsToChar(lpEsVar),
		                      FALSE);

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

	lpLine=Dos9_SkipBlanks(lpLine);

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

	lpVarName=Dos9_SkipBlanks(lpVarName);

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

	snprintf(lpResult, sizeof(lpResult), "%.16g", dVal);

	if (Dos9_setenv(Dos9_EsToChar(lpExpression), lpResult)) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
		                      Dos9_EsToChar(lpExpression),
		                      FALSE);

		goto error;

	}

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
					if (bDouble) {

						iVal=iVal || iResult;

					} else {

						iVal|=iResult;

					}

					break;

				case '&':
					if (bDouble) {

						iVal=iVal && iResult;

					} else {

						iVal&=iResult;

					}

			}

			break;

		default:
			iVal=iResult;

	}

	snprintf(lpResult, sizeof(lpResult), "%d", iVal);

	if (Dos9_setenv(Dos9_EsToChar(lpExpression), lpResult)) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
		                      Dos9_EsToChar(lpExpression),
		                      FALSE);

		goto error;

	}

	return 0;

error:
	return -1;
}

