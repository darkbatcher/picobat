/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2017 Romain GARBI
 *   Copyright (C) 2016 	 Teddy ASTIE
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
#include <math.h>
#include <string.h>

#include <libDos9.h>

#include "Dos9_If.h"
#include "Dos9_IfExp.h"
#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"
/*
        IF [/i] [NOT] test==valeur (...)
        IF [/i] test2 CMP test2 (...)
        IF [NOT] EXIST fichier (...)
        IF [NOT] DEFINED var (...)
        IF [NOT] ERRORLEVEL code (...) -> DEPRECATED !
*/

int Dos9_CmdIf(char* lpParam, PARSED_LINE** lpplLine)
{
	char lpArgument[FILENAME_MAX], *lpNext, *lpToken, *lpToken2 /*, *lpEnd */;
	int iFlag=0,
	    iResult;
    PARSED_LINE* line;

	ESTR *lpComparison, *lpOtherPart;
	LPFILELIST lpflFileList;

	BLOCKINFO bkInfo;

	lpParam+=2;

	if ((lpNext=Dos9_GetNextParameter(lpParam, lpArgument, 11))) {

		if (!strcmp(lpArgument, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_IF);
			return 0;

		}

		if (!stricmp(lpArgument, "/i")) {

			/* this is case insensitive */
			iFlag|=DOS9_IF_CASE_UNSENSITIVE;
			lpParam=lpNext;

			if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

				Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);

                if (lpplLine)
                    *lpplLine = NULL;

				return DOS9_EXPECTED_MORE;

			}

		}

		if (!stricmp(lpArgument, "NOT")) {

			/* this is negative */
			iFlag|=DOS9_IF_NEGATION;
			lpParam=lpNext;

			if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

				Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);

                if (lpplLine)
                    *lpplLine = NULL;

				return DOS9_EXPECTED_MORE;

			}

		}

		if (!stricmp(lpArgument, "EXIST")) {

			iFlag|=DOS9_IF_EXIST;

		} else if (!stricmp(lpArgument, "DEFINED")) {

			iFlag|=DOS9_IF_DEFINED;

		} else if (!stricmp(lpArgument, "ERRORLEVEL")) {

			iFlag|=DOS9_IF_ERRORLEVEL;

		}

		if ((iFlag & DOS9_IF_CASE_UNSENSITIVE)
		    && (iFlag & (DOS9_IF_ERRORLEVEL | DOS9_IF_EXIST | DOS9_IF_DEFINED))
		   ) {

			Dos9_ShowErrorMessage(DOS9_INCOMPATIBLE_ARGS,
                                     "'/i' (DEFINED, EXIST, ERRORLEVEL)",
                                     FALSE);
            if (lpplLine)
                    *lpplLine = NULL;

			return DOS9_INCOMPATIBLE_ARGS;

		}

		if (iFlag & (DOS9_IF_EXIST | DOS9_IF_DEFINED | DOS9_IF_ERRORLEVEL))
			lpParam=lpNext;

	}

	if (iFlag & DOS9_IF_ERRORLEVEL) {

		/* the script used 'ERRORLEVEL' Keyword */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);

            if (lpplLine)
                    *lpplLine = NULL;

			return DOS9_EXPECTED_MORE;

		}

        DOS9_IF_ERRORLEVEL_TEST(iResult, lpArgument, lpeEnv);

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;


	} else if (iFlag & DOS9_IF_EXIST) {

		/* the script used 'EXIST' keyWord */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);

            if (lpplLine)
                *lpplLine = NULL;

			return DOS9_EXPECTED_MORE;

		}

		DOS9_IF_EXIST_TEST(iResult, lpArgument, lpflFileList);

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;

	} else if (iFlag & DOS9_IF_DEFINED) {

		/* the script used 'DEFINED' */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);

            if (lpplLine)
                    *lpplLine = NULL;

			return DOS9_EXPECTED_MORE;

		}

		/* avoid errors from conversion from 64 bits to
		   32 bits */
        DOS9_IF_DEFINED_TEST(iResult, lpArgument, lpeEnv)

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;

	} else {

		/* the script uses normal comparisons */
		lpComparison=Dos9_EsInit();

        lpNext=Dos9_GetNextParameterEs(lpParam, lpComparison);

         if (!strcmp(lpComparison->str, "[")) {

            lpNext = Dos9_IfExp_Compute(lpParam, &iResult, iFlag);

            if (iResult == -1) {

                Dos9_EsFree(lpComparison);
                Dos9_ShowErrorMessage(DOS9_INVALID_IF_EXPRESSION, lpParam, FALSE);

                if (lpplLine)
                        *lpplLine = NULL;

                return DOS9_INVALID_IF_EXPRESSION;

            }

		 } else if (lpNext) {

			if ((lpToken=strstr(Dos9_EsToChar(lpComparison), "=="))) {
				/* if script uses old c-style comparison */
				*lpToken='\0';
				lpToken+=2;
				lpToken2 = lpComparison->str;

				/* Double quote hack

				   This is basically a hack designed to keep compatibility with cmd.exe
				   by supporting "foo"=="foo".

				   Todo: clean this a little bit

				*/
				char *q = (lpToken2 + strlen(lpToken2) - 1);

				if (*q == '"') {
					*q = '\0';
					lpToken++;
				}

                iResult = Dos9_PerformExtendedTest("==",
                                                   Dos9_EsToChar(lpComparison),
                                                    lpToken,
                                                    iFlag);

				if (iFlag & DOS9_IF_NEGATION)
                    iResult=!iResult;


			} else {
				/* if script uses new basic-like comparison (e.g. EQU, NEQ ...) */

				if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
					Dos9_EsFree(lpComparison);

                    if (lpplLine)
                        *lpplLine = NULL;

					return DOS9_EXPECTED_MORE;

				}

                lpOtherPart=Dos9_EsInit();

				if (!(lpNext=Dos9_GetNextParameterEs(lpNext, lpOtherPart))) {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
					Dos9_EsFree(lpOtherPart);
					Dos9_EsFree(lpComparison);

                    if (lpplLine)
                        *lpplLine = NULL;

					return DOS9_EXPECTED_MORE;

				}

                iResult = Dos9_PerformExtendedTest(lpArgument,
                                                    Dos9_EsToChar(lpComparison),
                                                    Dos9_EsToChar(lpOtherPart),
                                                    iFlag);

                Dos9_EsFree(lpOtherPart);

                if (iResult == -1) {

                    /* there is a syntax error, obviously */
                    return -1;

                }


			}
		} else {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
			Dos9_EsFree(lpComparison);

            if (lpplLine)
                *lpplLine = NULL;

			return DOS9_EXPECTED_MORE;

		}

		lpParam=lpNext;
		Dos9_EsFree(lpComparison);

	}

    lpNext = Dos9_SkipBlanks(lpParam);

    if (iResult) {

        if (*lpNext != '(') {
            /* if the command does not start with a '(' get
                the full line of blocks that follow the command */
            lpNext=Dos9_GetBlockLine(lpNext, &bkInfo);
        } else {
            /* else if the line is actually a block, process it
                as needed */
            lpNext = Dos9_GetNextBlock(lpNext, &bkInfo);
        }

        lpToken2 = bkInfo.lpEnd;
        if (*lpToken2 == ')')
            lpToken2 ++;

        lpToken2 = Dos9_SkipBlanks(lpToken2);

        /* refactor line */
        if (lpplLine && !*lpToken2)
            line = Dos9_LookAHeadMakeParsedLine(&bkInfo, *lpplLine);
        else
            line = Dos9_LookAHeadMakeParsedLine(&bkInfo, NULL);

        Dos9_RunParsedLine(line);

        if (!lpplLine)
            Dos9_FreeParsedLine(line);

    } else {

        if (*lpNext == '(') {

            lpNext=Dos9_GetNextBlock(lpNext, &bkInfo);

            if (*lpNext==')')
                lpNext ++;

            lpNext=Dos9_SkipBlanks(lpNext);

            if (!strnicmp(lpNext, "ELSE", 4)
                    && (Dos9_IsDelim(*(lpNext+4)) | (*(lpNext+4)=='\0'))) {

                lpNext=Dos9_SkipBlanks(lpNext+4);

                if (*lpNext != '(') {
                    /* if the command does not start with a '(' get
                        the full line of blocks that follow the command */
                    lpNext=Dos9_GetBlockLine(lpNext, &bkInfo);
                } else {
                    /* else if the line is actually a block, process it
                        as needed */
                    lpNext = Dos9_GetNextBlock(lpNext, &bkInfo);
                }

                if (*lpNext==')')
                    lpNext++;

                lpNext = Dos9_SkipBlanks(lpNext);

                if (*lpNext && *lpNext != '\n') {

                    /* there's something trailing here*/
                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpNext, FALSE);

                    if (lpplLine)
                        *lpplLine = NULL;

                    return DOS9_UNEXPECTED_ELEMENT;

                } else {

                    if (lpplLine)
                        line = Dos9_LookAHeadMakeParsedLine(&bkInfo, *lpplLine);
                    else
                        line = Dos9_LookAHeadMakeParsedLine(&bkInfo, NULL);

                    Dos9_RunParsedLine(line);

                    if (!lpplLine)
                        Dos9_FreeParsedLine(line);

                }

            } else if (*lpNext) {

                    /* there's something trailing that is not an else here*/
                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpNext, FALSE);

                    if (lpplLine)
                        *lpplLine = NULL;

                    return DOS9_UNEXPECTED_ELEMENT;

            }

        } else {

            /* If there is no else block and if the comparison is false,
               then cancel all the content of the line (ie. do not exec
               conditional operators following IF

			   Here We cancel the following operators but we do not try to
			   execute them before */

        }
    }

    if (lpplLine)
        *lpplLine = NULL;

	return 0;
}

/* Get the value from the chain pointed to by *number and return a value
   based on mode based on mode :
        1 -> double
        0 -> int */
int Dos9_GetCmpValue(const char* number, double* value)
{
    int val;
    char* end;
    int f=0;

    while (1) {

        if (f == 0)
            *value = (double)strtol(number, &end, 0);
        else /* Double mode chosen */
            *value = strtod(number, &end);

        if (*Dos9_SkipBlanks(end) != '\0'){
            if (f == 0)
                f = 1;
            else
                return  -1;
        } else
            return 0;

    }
}

/* Performs an extended test for the if command (ie. for 'EQU' and so on ...)

   It takes as argument a pointer to the comparant, two pointers to the
   parameters of comparant, and the flag.

   It returns either 0 if the test is false, 1 if the test is true; and -1 if
   the syntax is not valid.

   Note that C89 standard specify that logical operators must evaluate to 1 or
   0 only, so that we can not have confusion between errors.

 */
int Dos9_PerformExtendedTest(const char* lpCmp, const char* lpParam1,
											const char* lpParam2, int iFlag)
{

    int comp_type=0,
        string_comp=0;

    int iExp1,
        iExp2;

    /* We do everything using */
    double rhs,
            lhs,
            x1,
            x2;

    char* lpEnd;

    /* Get the value of the left and right hand side of the
       comparison */
    if (Dos9_GetCmpValue(lpParam1, &rhs)
        || Dos9_GetCmpValue(lpParam2, &lhs)) {

        /* If we get here, do something clever, that is,
           set left hand side to 0 and right hand side to
           strcmp(lpParam1, lpParam2) */

        lhs = 0.0;

        if (iFlag & DOS9_IF_CASE_UNSENSITIVE)
            rhs = (double)stricmp(lpParam1, lpParam2);
        else
            rhs = (double)strcmp(lpParam1, lpParam2);

    }

    if (!stricmp(lpCmp, "EQU")
        || !stricmp(lpCmp, "==")) {

        if (comp_type & CMP_FLOAT_COMP) {
            x1 = frexp(rhs, &iExp1);
            x2 = frexp(lhs, &iExp2);

            /* majorate the error */
            return (fabs(x1-x2*pow(2, iExp2-iExp1))
                                <= DOS9_FLOAT_EQUAL_PRECISION);
        }

        return (rhs == lhs);

    } else if (!stricmp(lpCmp, "NEQ")) {

        if (comp_type & CMP_FLOAT_COMP) {
            x1 = frexp(rhs, &iExp1);
            x2 = frexp(lhs, &iExp2);

            /* majorate the error */
            return (fabs(x1-x2*pow(2, iExp2-iExp1))
                                >= DOS9_FLOAT_EQUAL_PRECISION);
        }

        return (rhs != lhs);

    } else if (!stricmp(lpCmp, "GEQ")) {

        return (rhs >= lhs);

    } else if (!stricmp(lpCmp, "GTR")) {

        return (rhs > lhs);

    } else if (!stricmp(lpCmp, "LEQ")) {

        return (rhs <= lhs);

    } else if (!stricmp(lpCmp, "LSS")) {

        return (rhs < lhs);

    } else {

        Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpCmp, FALSE);
        return -1;

    }

}
