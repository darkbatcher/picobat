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

int Dos9_CmdIf(char* lpParam)
{
	char lpArgument[FILENAME_MAX], *lpNext, *lpToken, *lpToken2, *lpEnd;
	int iFlag=0,
	    iResult;

	ESTR *lpComparison, *lpOtherPart;
	LPFILELIST lpflFileList;
	/* syntaxe possible
	        IF [/i] [NOT] test==valeur (...)
	        IF [/i] test2 CMP test2 (...)
	        IF [NOT] EXIST fichier (...)
	        IF [NOT] DEFINED var (...)
	        IF [NOT] ERRORLEVEL code (...) -> DEPRECATED !
	*/

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
				return 0;

			}

		}

		if (!stricmp(lpArgument, "NOT")) {

			/* this is negative */
			iFlag|=DOS9_IF_NEGATION;
			lpParam=lpNext;

			if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

				Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
				return 0;

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

			Dos9_ShowErrorMessage(DOS9_INCOMPATIBLE_ARGS, "'/i' (DEFINED, EXIST, ERRORLEVEL)", FALSE);
			return 0;

		}

		if (iFlag & (DOS9_IF_EXIST | DOS9_IF_DEFINED | DOS9_IF_ERRORLEVEL))
			lpParam=lpNext;

	}

	if (iFlag & DOS9_IF_ERRORLEVEL) {

		/* the script used 'ERRORLEVEL' Keyword */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
			return -1;

		}

        DOS9_IF_ERRORLEVEL_TEST(iResult, lpArgument, lpeEnv);

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;


	} else if (iFlag & DOS9_IF_EXIST) {

		/* the script used 'EXIST' keyWord */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
			return -1;

		}

		DOS9_IF_EXIST_TEST(iResult, lpArgument, lpflFileList);

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;

	} else if (iFlag & DOS9_IF_DEFINED) {

		/* the script used 'DEFINED' */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
			return -1;

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
                return -1;

            }

		 } else if (lpNext) {

			if ((lpToken=strstr(Dos9_EsToChar(lpComparison), "=="))) {
				/* if script uses old c-style comparison */
				*lpToken='\0';
				lpToken+=2;

				lpToken2 = Dos9_EsToChar(lpComparison);

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

				if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {
                    iResult=!stricmp(Dos9_EsToChar(lpComparison), lpToken);
				} else {
				    iResult=!strcmp(Dos9_EsToChar(lpComparison), lpToken);
				}

				if (iFlag & DOS9_IF_NEGATION) iResult=!iResult;


			} else {
				/* if script uses new basic-like comparison (e.g. EQU, NEQ ...) */

				if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
					Dos9_EsFree(lpComparison);
					return -1;

				}

                lpOtherPart=Dos9_EsInit();

				if (!(lpNext=Dos9_GetNextParameterEs(lpNext, lpOtherPart))) {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "IF", FALSE);
					Dos9_EsFree(lpOtherPart);
					Dos9_EsFree(lpComparison);
					return -1;

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
			return -1;

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

        Dos9_RunBlock(&bkInfo);

    } else {

        if (*lpNext == '(') {

            lpNext=Dos9_GetNextBlock(lpNext, &bkInfo);

            if (*lpNext==')')
                lpNext ++;

            lpNext=Dos9_SkipBlanks(lpNext);

            if (!strnicmp(lpNext, "ELSE", 4)
                    && (Dos9_IsDelim(*(lpNext+4)) | *(lpNext+4)=='\0')) {

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

                Dos9_RunBlock(&bkInfo);

                if (*lpNext==')')
                    lpNext++;

                lpNext = Dos9_SkipBlanks(lpNext);

                if (*lpNext && *lpNext != '\n') {

                    /* there's something trailing that is not an else here*/
                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpNext, FALSE);
                    return -1;

                }

            } else if (*lpNext) {

                    /* there's something trailing that is not an else here*/
                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpNext, FALSE);
                    return -1;

            }

        }
    }

	return 0;
}


/* Performs an extended test for the if command (ie. for 'EQU' and so on ...)

   It takes as argument a pointer to the comparant, two pointers to the
   parameters of comparant, and the flag.

   It returns either 0 if the test is false, 1 if the test is true; and -1 if
   the syntax is not valid.

   Note that C89 standard specify that logical operators must evaluate to 1 or
   0 only, so that we can not have confusion between errors.

 */
int Dos9_PerformExtendedTest(const char* lpCmp, const char* lpParam1, const char* lpParam2, int iFlag)
{

    int comp_type=0,
        irhs, /* right end side */
        ilhs, /* left end side */
        invalid=0;

    int iExp1,
        iExp2;

    double drhs,
            dlhs,
            x1,
            x2;

    char* lpEnd;

    if (*lpCmp=='F' || *lpCmp=='f') {

        /* use the float versions of comparisons */
        lpCmp++;
        comp_type=CMP_FLOAT_COMP;

        drhs = strtod(lpParam1, &lpEnd);
        if (*lpEnd != '\0') {
            invalid=1;
            goto next;
        }

        dlhs = strtod(lpParam2, &lpEnd);
        if (*lpEnd != '\0') {
            invalid=1;
            goto next;
        }

    } else {

        irhs = strtol(lpParam1, &lpEnd, 0);
        if (*lpEnd != '\0') {
            invalid=1;
            goto next;
        }

        ilhs = strtol(lpParam2, &lpEnd, 0);
        if (*lpEnd != '\0') {
            invalid=1;
            goto next;
        }

    }

next:

    if (!stricmp(lpCmp, "EQU")
        || !stricmp(lpCmp, "==")) {

        comp_type|=CMP_EQUAL;

    } else if (!stricmp(lpCmp, "NEQ")) {

        comp_type|=CMP_DIFFERENT;

    } else if (!stricmp(lpCmp, "GEQ")) {

        comp_type|=CMP_GREATER_OR_EQUAL;

    } else if (!stricmp(lpCmp, "GTR")) {

        comp_type|=CMP_GREATER;

    } else if (!stricmp(lpCmp, "LEQ")) {

        comp_type|=CMP_LESSER_OR_EQUAL;

    } else if (!stricmp(lpCmp, "LSS")) {

        comp_type|=CMP_LESSER;

    } else {

        Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpCmp, FALSE);
        return -1;

    }

    if (invalid && (comp_type != CMP_EQUAL)
            && (comp_type != CMP_DIFFERENT)) {

        Dos9_ShowErrorMessage(DOS9_COMPARISON_FORBIDS_STRING,
                                lpCmp,
                                FALSE
                                );

        return -1;

    }

    switch (comp_type) {

        case CMP_EQUAL:
            if (invalid) {

                /* if there is no valid numbers */

                if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

                    return !stricmp(lpParam1, lpParam2);

                } else {

                    return !strcmp(lpParam1, lpParam2);

                }

            } else {

                return (irhs == ilhs);

            }

        case CMP_DIFFERENT:
            if (invalid) {

                    /* then, we force use of logical expressions in order
                       to get something between 1 and 0 */

                if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

                    return stricmp(lpParam1, lpParam2) != 0;

                } else {

                    return strcmp(lpParam1, lpParam2) != 0;

                }

            } else {

                return (irhs != ilhs);

            }

        case CMP_GREATER:
            return (irhs > ilhs);

        case CMP_GREATER_OR_EQUAL:
            return (irhs >= ilhs);

        case CMP_LESSER:
            return (irhs < ilhs);

        case CMP_LESSER_OR_EQUAL:
            return (irhs <= ilhs);

        case CMP_EQUAL | CMP_FLOAT_COMP:
            if (invalid) {

                if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

                    return !stricmp(lpParam1, lpParam2);

                } else {

                    return !strcmp(lpParam1, lpParam2);

                }

            } else {

                x1 = frexp(drhs, &iExp1);
                x2 = frexp(dlhs, &iExp2);

                /* majorate the error */
                return (fabs(x1-x2*pow(2, iExp2-iExp1)) <= DOS9_FLOAT_EQUAL_PRECISION);

            }

        case CMP_DIFFERENT | CMP_FLOAT_COMP:
            if (invalid) {

                if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

                    return stricmp(lpParam1, lpParam2) != 0;

                } else {

                    return strcmp(lpParam1, lpParam2) != 0;

                }

            } else {

                x1 = frexp(drhs, &iExp1);
                x2 = frexp(dlhs, &iExp2);

                /* majorate the error */
                return !(fabs(x1-x2*pow(2, iExp2-iExp1)) <= DOS9_FLOAT_EQUAL_PRECISION);

            }

        case CMP_GREATER | CMP_FLOAT_COMP:
            return (drhs > dlhs);

        case CMP_GREATER_OR_EQUAL | CMP_FLOAT_COMP:
            return (drhs >= dlhs);

        case CMP_LESSER | CMP_FLOAT_COMP:
            return (drhs < dlhs);

        case CMP_LESSER_OR_EQUAL | CMP_FLOAT_COMP:
            return (drhs <= dlhs);


    }

    return -1;
}
