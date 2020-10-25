/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include <libDos9.h>

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"
#include "Dos9_For.h"
#include "Dos9_Cd.h"

/*
    The for command

    * For each item in a list.
      Default token are ", " and carriage return.
      if any item is a regexp, the corresponding token are
      replace with the matching files.

        FOR %%A IN (ens) DO (

        )

    * For each integer value in a range of values

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

#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
#include <libcu8.h>
#endif /* libcu8 */

int Dos9_CmdFor(char* lpLine, PARSED_LINE** lpplLine)
{
	ESTR* lpParam=Dos9_EsInit();
	ESTR* lpDirectory=Dos9_EsInit();
	ESTR* lpInputBlock=Dos9_EsInit();
	PARSED_LINE* line=NULL;

	BLOCKINFO bkCode;

	FORINFO forInfo= {
		" ", /* default token delimiter */
		";", /* the default eol delimiter is ; */
		0, /* no skipped line */
		0, /* this is to be fullfiled later (the
                name letter of loop special var) */
		FALSE,
		1, /* the number of tokens we were given */
		{TOHIGH(1)|1} /* get the first token back */

	};

	char  cVarName;
	char* lpToken=lpLine+3;
	char* lpVarName;

	int iForType=FOR_LOOP_SIMPLE,
        status = DOS9_NO_ERROR;

	while ((lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

		if (!stricmp(Dos9_EsToChar(lpParam), "/F")) {

			iForType=FOR_LOOP_F;

		} else if (!stricmp(Dos9_EsToChar(lpParam), "/L")) {

			iForType=FOR_LOOP_L;

		} else if (!stricmp(Dos9_EsToChar(lpParam), "/R")) {

			iForType=FOR_LOOP_R;

		} else if (!stricmp(Dos9_EsToChar(lpParam), "/D")) {

			iForType=FOR_LOOP_D;

		} else if (!strcmp(Dos9_EsToChar(lpParam), "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_FOR);
			goto error;

		} else {

			break;

		}

	}

	while (TRUE) {


		if (lpToken == NULL) {

			/* if the line is not complete */
			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "FOR", FALSE);

            status = DOS9_EXPECTED_MORE;
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

			if (Dos9_TestLocalVarName(cVarName)) {

                Dos9_ShowErrorMessage(DOS9_SPECIAL_VAR_NON_ASCII,
                                        (void*)cVarName,
                                        FALSE);

                status = DOS9_SPECIAL_VAR_NON_ASCII;
                goto error;

			}

			/* just get out of that loop */
			break;

		} else if (iForType == FOR_LOOP_SIMPLE
                    || iForType == FOR_LOOP_L
                    || *Dos9_EsToChar(lpDirectory) != '\0') {

			/* It appears that the users does not specify a variable
			   but try rather to pass options, that are obviously inconsistent
			   in this case */
			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
                                        Dos9_EsToChar(lpParam),
                                        FALSE);
			status = DOS9_UNEXPECTED_ELEMENT;

			goto error;

		} else {

			switch(iForType) {

				case FOR_LOOP_D:
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
					if ((status = Dos9_ForMakeInfo(Dos9_EsToChar(lpParam),
					 												&forInfo)))
                        goto error;
			}

			lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

		}

	}

	/* if the for uses either /F or /R or even /D
	   store cVarName in the FORINFO structure
	*/

	if (FOR_LOOP_F == iForType
        || FOR_LOOP_R == iForType) {

		forInfo.cFirstVar=cVarName;

	}


	/* Now the next parameter should be IN */

	lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

	if (stricmp(Dos9_EsToChar(lpParam), "IN")
        || lpToken == NULL ) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "FOR", FALSE);
		status = DOS9_EXPECTED_MORE;
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
		status = DOS9_ARGUMENT_NOT_BLOCK;
		goto error;

	}

	/* So just get back the block */
	lpToken=Dos9_GetNextBlockEs(lpToken, lpInputBlock);

	/* Now we check if ``DO'' is used */

	if (!(lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "FOR", FALSE);
		status = DOS9_EXPECTED_MORE;
		goto error;

	}

	if ((stricmp(Dos9_EsToChar(lpParam), "DO"))) {

		/* if ``DO'' is not used */
		Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam),
																		FALSE);
		status = DOS9_UNEXPECTED_ELEMENT;
		goto error;

	}

    lpToken = Dos9_SkipBlanks(lpToken);

    if (*lpToken != '(') {
        lpToken = Dos9_GetBlockLine(lpToken, &bkCode);
    } else {
        lpToken = Dos9_GetNextBlock(lpToken, &bkCode);
    }

	if (*lpToken == ')')
        lpToken ++;

    lpToken = Dos9_SkipBlanks(lpToken);

    if (*lpToken && lpToken != NULL) {

            /* There is trailing characters after the block */
			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
			status = DOS9_UNEXPECTED_ELEMENT;
			goto error;

    }


    /* Beware after this line ! There is a quite high probability that lpLine
       will get somehow freed */
    line = Dos9_LookAHeadMakeParsedLine(&bkCode, lpplLine ? *lpplLine : NULL) ;

	switch(iForType) {

		case FOR_LOOP_SIMPLE:

			/* this handles simple for */
			status = Dos9_CmdForSimple(lpInputBlock, line,
											cVarName, " ,:;\n\t\"");

			break;

		case FOR_LOOP_R:
			status = Dos9_CmdForDeprecatedWrapper(lpInputBlock, lpDirectory,
													"/A:-D", line, cVarName);
			break;

		case FOR_LOOP_D:
			status = Dos9_CmdForDeprecatedWrapper(lpInputBlock, lpDirectory,
													"/A:D", line, cVarName);
			break;

		case FOR_LOOP_F:
			status = Dos9_CmdForF(lpInputBlock, line, &forInfo);
			break;

		case FOR_LOOP_L:
			status = Dos9_CmdForL(lpInputBlock, line, cVarName);
	}

error:
	Dos9_EsFree(lpParam);
	Dos9_EsFree(lpDirectory);
	Dos9_EsFree(lpInputBlock);

	/* we already swallowed everything */
    if (lpplLine)
        *lpplLine = NULL;

	if (!lpplLine && line)
        Dos9_FreeParsedStream(line);

	return status;
}

int Dos9_CmdForSimple(ESTR* lpInput, PARSED_LINE* lpplLine, char cVarName,
								char* lpDelimiters)
{

	char *lpToken=lpInput->str,
         *lpBegin,
         *tmp;

    size_t nSize;

    ESTR* lpesStr=Dos9_EsInit();
    FILELIST *files = NULL,
             *item = NULL;

	while ((files != NULL)
           || (lpToken != NULL && Dos9_GetParameterPointers(&lpBegin, &lpToken,
                                                    lpDelimiters, lpToken))) {

        if (files != NULL) {

            if (item == NULL) {

                Dos9_FreeFileList(files);
                files = NULL;
                continue;

            }

            Dos9_EsCpy(lpesStr, item->lpFileName + nSize);
            item = item->lpflNext;

        } else {

            if (lpToken == NULL) {

                Dos9_EsCpy(lpesStr, lpBegin);

            } else {

                Dos9_EsCpyN(lpesStr,
                            lpBegin,
                            (size_t)(lpToken-lpBegin)
                            );

            }

            if (strpbrk(lpesStr->str, "*?")) {

                /* Remove extra quotes */
                if (*lpBegin == '"') {
                    lpBegin ++;

                    if (lpToken == NULL) {

                        Dos9_EsCpy(lpesStr, lpBegin);

                    } else {

                        Dos9_EsCpyN(lpesStr,
                                    lpBegin,
                                    (size_t)(lpToken-lpBegin)
                                    );

                    }

                    if ((tmp = strrchr(lpesStr->str, '"'))
                        && (*(tmp + 1) == '\0')) {
                        *tmp = '\0';
                    }

                }

                if (!DOS9_TEST_ABSOLUTE_PATH(lpesStr->str))
                    nSize = strlen(lpCurrentDir) + 1;
                else
                    nSize = 0;

                /* We came across a reg expression, just launch a search */
                files = Dos9_GetMatchFileList(Dos9_EsToFullPath(lpesStr),
                                                DOS9_SEARCH_NO_PSEUDO_DIR
                                                | DOS9_SEARCH_NO_STAT);

                Dos9_AttributesSplitFileList(DOS9_ATTR_NO_DIR,
                                             files,
                                             &files,
                                             &item);

                if (item)
                    Dos9_FreeFileList(item);

                /* do not bother that much if the input expression does not
                   match anything */
                item = files;
                continue;

            }

        }

		/* assign the local variable */
		Dos9_SetLocalVar(lpvLocalVars, cVarName, Dos9_EsToChar(lpesStr));

		/* run the block */
		Dos9_RunParsedLine(lpplLine);

		/* if a goto as been executed while the for-loop
		   was ran */
		if (bAbortCommand)
			break;

	}

	/* delete the special var */
	Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);
    Dos9_EsFree(lpesStr);

	return 0;

}

#define XSTR(x) #x
#define STR(x) XSTR(x)

int Dos9_CmdForL(ESTR* lpInput, PARSED_LINE* lpplLine, char cVarName)
{
	int iLoopInfo[3]= {0,0,0}, /* loop information */
        i=0,
        status = DOS9_NO_ERROR;

	char lpValue[]=STR(INT_MIN),
         *lpToken=Dos9_EsToChar(lpInput),
         *lpNext;

    ESTR* lpesStr=Dos9_EsInit();

	while ((lpToken=Dos9_GetNextParameterEs(lpToken, lpesStr)) && (i < 3)) {
        /* Loop to get start, increment and end */
        /* works with both hexadecimal, octal and decimal numbers */

		iLoopInfo[i]=strtol(Dos9_EsToChar(lpesStr),
                            &lpNext,
                            0);

        if (*lpNext) {

            Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER,
                                    Dos9_EsToChar(lpInput),
                                    FALSE);

            status = DOS9_FOR_BAD_INPUT_SPECIFIER;

            goto error;

        }

		i++;

	}

	if (lpToken) {

		Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER,
                                    Dos9_EsToChar(lpInput),
                                    FALSE);

        status = DOS9_FOR_BAD_INPUT_SPECIFIER;

		goto error;

	}

	/* execute loop */
	for (i=iLoopInfo[DOS9_FORL_BEGIN];
	     i<=iLoopInfo[DOS9_FORL_END];
	     i+=iLoopInfo[DOS9_FORL_INC]) {

		snprintf(lpValue, sizeof(lpValue), "%d", i);

		Dos9_SetLocalVar(lpvLocalVars, cVarName, lpValue);

		/* execute the code */

		Dos9_RunParsedLine(lpplLine);


		/* if a goto as been executed while the for-loop
		   was ran */
		if (bAbortCommand)
			break;

	}

	Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);

error:
    Dos9_EsFree(lpesStr);
	return status;

}

int Dos9_CmdForF(ESTR* lpInput, PARSED_LINE* lpplLine, FORINFO* lpfrInfo)
{
	int iSkip=lpfrInfo->iSkip,
        status = DOS9_NO_ERROR;

	ESTR* lpInputToP=Dos9_EsInit();

	INPUTINFO inputInfo;

	if ((status = Dos9_ForVarCheckAssignment(lpfrInfo)))
		goto error;

	if ((status = Dos9_ForMakeInputInfo(lpInput, &inputInfo, lpfrInfo)))
		goto error;

	while (Dos9_ForGetInputLine(lpInputToP, &inputInfo)) {

		if (iSkip > 0) {

			iSkip--;
			/* skip the n-th first lines, as specifiied by
			   ``skip=n'' in the command line */

		} else {

            /* If the line starts with one of the EOL characters, just skip it */
            if (strchr(lpfrInfo->lpEol, *(lpInputToP->str)))
                continue;

			Dos9_ForSplitTokens(lpInputToP, lpfrInfo);
			/* split the block on subtokens */

			Dos9_RunParsedLine(lpplLine);
			/* split the input into tokens and then run the
			    block */
		}


		/* if a goto as been executed while the for-loop
		   was ran or a goto:eof or something like exit /b */

		if (bAbortCommand)
			break;

	}

	Dos9_ForCloseInputInfo(&inputInfo);
	Dos9_ForVarUnassign(lpfrInfo);

error:
	Dos9_EsFree(lpInputToP);

	return status;
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
	ESTR* param = Dos9_EsInit();
	char* token;
	int status = DOS9_NO_ERROR,
        type;

	while ((lpOptions = Dos9_ForGetSpecifier(lpOptions, param, &type))) {

        switch (type) {

        case DOS9_FOR_SPEC_INVALID:
            /* if no '=' was found, then the entries are just
			   wrong */
			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
                                   param->str,
                                    FALSE);

			status = DOS9_UNEXPECTED_ELEMENT;
			goto error;

        case DOS9_FOR_SPEC_USEBACKQ:
            /* if the script specifies "usebackq" it will change dos9 behaviour
               of the for /f loop */
			lpfiInfo->bUsebackq=TRUE;

			continue;

        case DOS9_FOR_SPEC_DELIMS:
            /* if the script specifies delims for the for /f loop */
            token = param->str + sizeof("delims");  /* get the part after the '=' */

            strncpy(lpfiInfo->lpDelims, token, sizeof(lpfiInfo->lpDelims));
			lpfiInfo->lpDelims[sizeof(lpfiInfo->lpDelims)-1] = '\0';

            /* The default end-of-line character is ';', thus if you specify ';' as
               a delimiter, It is wise to remove it of end-of-line character if those have
               not been redefined yet.

               This obviously does not cover the case where ';' is specified explicitely
               in both "delims" and "tokens" by the user. */

			if (strchr(lpfiInfo->lpDelims, ';') != NULL
                && !strcmp(lpfiInfo->lpEol, ";"))
                strcpy(lpfiInfo->lpEol, "");

			break;

        case DOS9_FOR_SPEC_SKIP:
            /* if skip is specified */
            token = param->str + sizeof("skip");  /* get the part after the '=' */
            lpfiInfo->iSkip=strtol(token, NULL, 0);

            break;

        case DOS9_FOR_SPEC_EOL:
            /* If EOL is specified */
            token = param->str + sizeof("eol");  /* get the part after the '=' */
            strncpy(lpfiInfo->lpEol, token, sizeof(lpfiInfo->lpEol));
			lpfiInfo->lpEol[sizeof(lpfiInfo->lpEol)-1] = '\0';

			break;

        case DOS9_FOR_SPEC_TOKENS:
            /* If tokens is specified*/
            token = param->str + sizeof("tokens");  /* get the part after the '=' */
            if ((status = Dos9_ForMakeTokens(token, lpfiInfo)))
				goto error;
        }
    }

error:
	Dos9_EsFree(param);

	return status;
}

int Dos9_ForIsSpecifier(const char* restrict p)
{
    if (!strnicmp(p, "tokens=", sizeof("tokens=") - 1))
        return DOS9_FOR_SPEC_TOKENS;

    if (!strnicmp(p, "eol=", sizeof("eol=") - 1))
        return DOS9_FOR_SPEC_EOL;

    if (!strnicmp(p, "delims=", sizeof("delims=") -1 ))
        return DOS9_FOR_SPEC_DELIMS;

    if (!strnicmp(p, "skip=", sizeof("skip=") - 1 ))
        return DOS9_FOR_SPEC_SKIP;

    if (!strncasecmp(p, "usebackq", sizeof("usebackq") - 1)
        && (*(p + sizeof("usebackq") - 1) == '\0' ||
            *(p + sizeof("usebackq") - 1) == ' '))
        return DOS9_FOR_SPEC_USEBACKQ;

    return 0;
}

char* Dos9_ForGetSpecifier(const char* restrict in, ESTR* restrict out, int* restrict type)
{
    char *next = NULL;

    *type = Dos9_ForIsSpecifier(in);

    if (*in == '\0')
        return NULL;

    /* find next space  or end of string */
    if (!(next = strchr(in, ' '))) {
        next = in + strlen(in);
    } else {
        /*skip unnecessary spaces */
        while (*(next + 1)== ' ')
            next++;
    }

    if (*type && *type != DOS9_FOR_SPEC_USEBACKQ) {

        /* this is an equal based spec, iterate through spaces
           to find the next valid specifier */

        while (*next && !Dos9_ForIsSpecifier(next+1)) {


            if (!(next = strchr(next + 1, ' '))) {

                /* We reached the end of the line */
                next = in + strlen(in);
                break;

            } else {

                /* skip unnecessary spaces */
                while (*(next + 1) == ' ')
                    next ++;

            }
        }
    }

    Dos9_EsCpyN(out, in, next - in);

    return *next ? next + 1 : next;
}

int  Dos9_ForMakeTokens (char* p,  FORINFO* infos)
{
    int start =-1, /* The start of the token chunk */ /* -² means empty */
        end =-1, /* The end of the token chunk */
        operator = -1, /* The operator */
        nb,
        ok = 1,
        last = 0, /* the last token */
        i = 0; /* the current token index in FORINFO */

    char* next, /* A pointer to the next item to process */
          previous='\0', /* the memory of the last character */
          *orig = p;

    while (ok) {

        /* skip unnecessary spaces and tabs */
        while (*p == ' ' || *p == '\t')
            p++;

        /* try to read an actual number from the specifier */
        nb = strtol(p, &next, 0); /* accept all formats (octal / hex / dec) */

        if (next == p) {

            /* Getting no number is unexpected unless p is `\0` or
              p is `*` or the previous item was `*` and it is followed as
              expected by `,` or the end of the string*/

            if (*p && *p != '*'
                && (previous == '*' &&  *p != ',' && *p != '\0'))
                goto bad_token;


        } else if (previous == '*') {

            /* There is some trailing numbers right after a `*`, this is
               unexpected */
            goto bad_token;

        } else if (operator != -1) {

            if (start == -1) {
                /* We have an operator but no start value, something
                    has gone wrong */
                goto bad_token;
            }

            /* This is the second member of a operator like - or . */
            end = nb;

        } else {

            /* This is the first member */
            start = nb;

        }

        p = next;

        /* skip unnecessary spaces and tabs */
        while (*p == ' ' || *p == '\t')
            p++;

        if (start == -1 && *p != '*') {
            /* That's unexpected, we have a line that has no first member,
               and the operator caracter is neither * */
            goto bad_token;
        }

        /* Save the current character for subsequent loops */
        previous = *p;

        switch (*p)  {

        case ',':
        case '\0':
            /* It is time to write to struct */


            if (operator == - 1)  {

                /* check there's still room for a new token chunk in the
                       infos structure */
                if (i >= TOKENINFO_NB_MAX )
                    goto infos_overflow;

                /* write a single token to infos */
                infos->lpToken[i]|=TOHIGH(start)+TOLOW(start);
                i++;

                last = start;


            } else if (end == -1) {

                /* We have an operator but no second member, that's
                   also unexpected .... */
                goto bad_token;

            } else {


                if (operator == DOS9_FOR_TOKEN_CAT) {

                    /* check there's still room for a new token chunk in the
                       infos structure */
                    if (i >= TOKENINFO_NB_MAX )
                        goto infos_overflow;

                    /* write a cat token to infos */
                    infos->lpToken[i]|=TOHIGH(start)+TOLOW(end);
                    i++;

                } else {

                    /* loop through tokens from start to end */
                    while (start <= end) {

                        /* check there's still room for a new token chunk in the
                            infos structure */
                        if (i >= TOKENINFO_NB_MAX )
                            goto infos_overflow;

                        infos->lpToken[i]|=TOHIGH(start)+TOLOW(start);
                        i++;

                        start ++;

                    }

                }

                last = end;

            }

            /* reset internals */
            operator = -1;
            start = -1;
            end = -1;



            /* interupt the loop if we reached the end of the string */
            if (!*p)
                ok = 0;

            p++;
            break;


        case '-':

            if (end != -1)
                goto bad_token;

            /* select list operator */
            operator = DOS9_FOR_TOKEN_LIST;

            p++;
            break;

        case '.':

            if (end != -1)
                goto bad_token;

            /* select catenation operator */
            operator = DOS9_FOR_TOKEN_CAT;

            p++;
            break;

        case '*':

            if (start == -1) {

                /* This is the basic * character */
                start = last + 1;
                end = ALL_TOKEN;
                operator = DOS9_FOR_TOKEN_CAT;

            } else if (operator == -1) {

                /* This select the token `start` plus another chunk
                   of tokens from `start + 1` to `ALL_TOKEN` */

                if (i >= TOKENINFO_NB_MAX )
                    goto infos_overflow;

                /* write a single token to infos */
                infos->lpToken[i]|=TOHIGH(start)+TOLOW(start);
                i++;

                start ++;
                operator = DOS9_FOR_TOKEN_CAT;
                end = ALL_TOKEN;

            } else if (operator == DOS9_FOR_TOKEN_LIST) {

                /* we certainly can't do 1-* */
                goto bad_token;

            } else if (operator == DOS9_FOR_TOKEN_CAT) {

                /* Select all tokens from begin up to the end */
                end = ALL_TOKEN;
            }

            p++;
            break;

        default:
            /* That's unexpected ... */
            goto bad_token;

        }
    }

    infos->iTokenNb = i;

    return 0;

infos_overflow:
    Dos9_ShowErrorMessage(DOS9_FOR_TOKEN_OVERFLOW,
                            (char*)TOKENINFO_NB_MAX, FALSE);

    return DOS9_FOR_TOKEN_OVERFLOW;

bad_token:
    Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER,
                                    orig, FALSE);
    return DOS9_FOR_BAD_TOKEN_SPECIFIER;
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

	for (i=0; i < lpfrInfo->iTokenNb; i++) {

		Dos9_ForGetToken(lpContent, lpfrInfo, i, lpVarContent);
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

	*Dos9_EsToChar(lpReturn) = '\0';

	/*  the type of line that arrives here is already truncated
	    at the first character specified in eol parameter */

	/* printf("Getting token nb. %d\n"
	       "\t* iTokenBegin = %d\n"
	       "\t* iTokenEnd   = %d\n"
	       "\t* lpDelims    = \"%s\"\n\n", iPos, iTokenBegin, iTokenEnd, lpDelims);

	*/

	while ((lpNextToken = strpbrk(lpToken, lpDelims))) {
		/* get next first occurence of a lpDelims character */

		if (lpNextToken == lpToken) {

			/* this is already a delimiter
			   just ignore it */

			lpToken=lpNextToken+1;

			continue;

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

	/* printf("Get data loop Ended\n");

	printf("lpBeginToken=\"%s\"\n", lpBeginToken);
	if (lpEndToken)
	    printf("lpEndToken=\"%s\"\n", lpEndToken); */

	if (lpBeginToken) {

		if (lpEndToken) {

			iLength = lpEndToken - lpBeginToken;

			Dos9_EsCpyN(lpReturn, lpBeginToken, iLength);


		} else {

			Dos9_EsCpy(lpReturn, lpBeginToken);

		}

	}

	/* printf("Returned from token"); */

}

void Dos9_ForVarUnassign(FORINFO* lpfrInfo)
{
	int i,
	    iMax=lpfrInfo->iTokenNb;
	char cVarName=lpfrInfo->cFirstVar;

	for (i=0; i<iMax; i++) {

		Dos9_SetLocalVar(lpvLocalVars, cVarName+i, NULL);

		if ((cVarName+i) & 0x80)
			break;
	}
}

int Dos9_ForVarCheckAssignment(FORINFO* lpfrInfo)
{
	int i,
	    iMax=lpfrInfo->iTokenNb;
	char cVarName=lpfrInfo->cFirstVar;

	for (i=0; i<iMax; i++) {

		if ((Dos9_GetLocalVarPointer(lpvLocalVars, cVarName))) {

			Dos9_ShowErrorMessage(DOS9_FOR_TRY_REASSIGN_VAR,
                                        (char*)(cVarName), FALSE);
			return DOS9_FOR_TRY_REASSIGN_VAR;

		}

		if (cVarName & 0x80)
			break;
	}

	return DOS9_NO_ERROR;

}

/* ******************************************************************
   INPUTINFO Functions
*/

int Dos9_ForMakeInputInfo(ESTR* lpInput, INPUTINFO* lpipInfo, FORINFO* lpfrInfo)
{
	int bUsebackq=lpfrInfo->bUsebackq,
	    iInputType,
	    iPipeFd[2];
    int status;

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

				Dos9_ShowErrorMessage(DOS9_FOR_USEBACKQ_VIOLATION,
                                        "`", FALSE);
				return DOS9_FOR_USEBACKQ_VIOLATION;

			}

			iInputType=INPUTINFO_TYPE_COMMAND;

			break;

		case '"':
			/* the given input is a string */
			if (!bUsebackq) {

                iInputType=INPUTINFO_TYPE_STRING;
                break;

            }


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

            if (Dos9_ForInputParseFileList(&(lpipInfo->Info.InputFile),
                        lpInput))
                        return -1;

            /* Open the first file on the array. If we fail, stop the loop
               at the beginning. Notice we do not check for *all* files
               existence before starting the loop, Although cmd.exe may do
               so (haven't checked yet) */

			if (!(lpipInfo->Info.InputFile.pFile=
                    fopen(lpipInfo->Info.InputFile.lpesFiles[0]->str
                        , "rb"))) {

				Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                                        lpToken,
                                        FALSE);
				return DOS9_FILE_ERROR;

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

			lpipInfo->cType=INPUTINFO_TYPE_COMMAND;
            lpipInfo->Info.InputFile.lpesFiles[0]=NULL;

			if (_Dos9_Pipe(iPipeFd, 1024, O_BINARY) == -1) {

				Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR ,
                                        __FILE__ "/Dos9_MakeInputInfo()",
                                        FALSE);

				return DOS9_CREATE_PIPE ;
			}

			/* Launch the actual command from which we get input on a separate
               Dos9 thread */
			if ((status = Dos9_ForInputProcess(lpInput, lpipInfo, iPipeFd)))
				return status;


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

		return DOS9_FOR_BAD_INPUT_SPECIFIER;

	}

	while (*lpToken && *lpToken!=cBeginToken)
		*(lpInput++)=*(lpToken++);

	*lpInput='\0';

	if (*lpToken==cBeginToken) {

		return 0;

	} else {

		Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER, lpBegin, FALSE);

		return DOS9_FOR_BAD_INPUT_SPECIFIER;

	}

}

void Dos9_ExecuteForSubCommand(struct pipe_launch_data_t* arg)
{
    BLOCKINFO bkBlock;

    lppsStreamStack = Dos9_OpenOutputD(lppsStreamStack, arg->fd, DOS9_STDOUT);
    close(arg->fd);

    bIgnoreExit = TRUE;

    bkBlock.lpBegin = Dos9_EsToChar(arg->str);
    bkBlock.lpEnd = bkBlock.lpBegin;

    while (*(bkBlock.lpEnd ++));

    Dos9_RunBlock(&bkBlock);

    Dos9_EsFree(arg->str);
    free(arg);

}

int Dos9_ForInputProcess(ESTR* lpInput, INPUTINFO* lpipInfo, int* iPipeFd)
{
    struct pipe_launch_data_t* param;
    FILE* pFile;

    Dos9_SetFdInheritance(iPipeFd[1], 0);
	Dos9_SetFdInheritance(iPipeFd[0], 0);

    if (!(pFile=fdopen(iPipeFd[0], "rb"))) {

        Dos9_ShowErrorMessage(DOS9_UNABLE_DUPLICATE_FD,
                              iPipeFd[0],
                              FALSE);

        close(iPipeFd[0]);
        close(iPipeFd[1]);

        return DOS9_UNABLE_DUPLICATE_FD;

    }

    if ((param = malloc(sizeof(struct pipe_launch_data_t))) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_ForInputProcess()", -1);


    param->fd = iPipeFd[1];
    param->str = Dos9_EsInit();

    Dos9_EsCpyE(param->str, lpInput);

    lpipInfo->Info.InputFile.handle
        = Dos9_CloneInstance((void (*)(void *))Dos9_ExecuteForSubCommand, param);

	lpipInfo->Info.InputFile.pFile=pFile;

	lpipInfo->Info.InputFile.lpesFiles[1]=NULL;
	lpipInfo->Info.InputFile.index=0;

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
	char name[FILENAME_MAX];

loop_begin:
    iReturn=0;

	switch (lpipInfo->cType) {

		case INPUTINFO_TYPE_COMMAND:
		case INPUTINFO_TYPE_STREAM:

retry:
			if ((Dos9_EsGet(lpReturn, lpipInfo->Info.InputFile.pFile))) {

                if (lpipInfo->Info.InputFile.lpesFiles[
                        ++ (lpipInfo->Info.InputFile.index)
                        ] == NULL) {

                    /* there is no more remaining files to be read */
                    break;

                }

                /* if the file list is not finished, then, open the next file */
                if (!(lpipInfo->Info.InputFile.pFile =
                        fopen(lpipInfo->Info.InputFile.lpesFiles[
                            		lpipInfo->Info.InputFile.index
                            	]->str, "rb"))) {

                    Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
                            name,
                            FALSE
                            );

                    break;

                }

                goto retry;

            }

            iReturn = 1;

			break;

		case INPUTINFO_TYPE_STRING:
			iReturn=!Dos9_ForGetStringInput(lpReturn, &(lpipInfo->Info.StringInfo));

	}

	if (bCmdlyCorrect && iReturn) {

		lpToken=Dos9_EsToChar(lpReturn);
		lpToken=Dos9_SkipBlanks(lpToken);

		if ((*lpToken=='\0'
		     || *lpToken=='\n')
		    && iReturn) {

			/* this is a blank line, and the CMDLYCORRECT
			   is enabled, that means that we *must* loop
			   back and get another line, since cmd.exe
			   strips blank lines from the input.
			 */


            goto loop_begin;

		}

	}

	if ((lpToken=strchr(Dos9_EsToChar(lpReturn), '\n')))
		*lpToken='\0';

	return iReturn;
}

int Dos9_ForInputParseFileList(FILE_LIST_T* lpList, ESTR* lpInput)
{
    int i=0;

    char *lpToken=Dos9_EsToChar(lpInput);

    ESTR *lpesStr=Dos9_EsInit();

    while ((lpToken=Dos9_GetNextParameterEs(lpToken, lpesStr))
            && (i < FILE_LIST_T_BUFSIZ)) {

        /* loop to get the appropriate elements */

        Dos9_EsToFullPath(lpesStr);

        lpList->lpesFiles[i++]=lpesStr;

        lpesStr=Dos9_EsInit();

    }

    lpList->lpesFiles[i] = NULL;
    lpList->index = 0;

    if (!i)
        return -1;


    Dos9_EsFree(lpesStr);

    return 0;
}

void Dos9_ForCloseInputInfo(INPUTINFO* lpipInfo)
{
    int i=0;
    void *p;
    THREAD *t = &(lpipInfo->Info.InputFile.handle);

	switch(lpipInfo->cType) {

        case INPUTINFO_TYPE_COMMAND:;

		    fclose(lpipInfo->Info.InputFile.pFile);
            Dos9_WaitForThread(t, &p);
            Dos9_CloseThread(t);
            break;

		case INPUTINFO_TYPE_STREAM:

            fclose(lpipInfo->Info.InputFile.pFile);

			while(lpipInfo->Info.InputFile.lpesFiles[i] != NULL) {

                Dos9_EsFree(lpipInfo->Info.InputFile.lpesFiles[i]);
                i++;

			}

			break;


		case INPUTINFO_TYPE_STRING:

			free(lpipInfo->Info.StringInfo.lpString);

	}

}

/* Wrapper for deprecated old FOR /R */
int  Dos9_CmdForDeprecatedWrapper(ESTR* lpMask, ESTR* lpDir, char* lpAttribute, PARSED_LINE* lpplLine, char cVarName)
{
	FORINFO forInfo= {
		" ", /* no tokens delimiters, since only one
                token is taken account */
		"", /* no end-of-line delimiters */
		0, /* no skipped line */
		cVarName, /* this is to be fulfiled later (the
                name letter of loop special var) */
		FALSE,
		1, /* the number of tokens we were given */
		{TOHIGH(1)|TOLOW(-1)} /* get all the token
                                    back */

	};

	ESTR* lpCommandLine=Dos9_EsInit();

	/* create equivalent for loop using for / */
	Dos9_EsCpy(lpCommandLine, "'DIR /b /s ");
	Dos9_EsCat(lpCommandLine, lpAttribute);
	Dos9_EsCat(lpCommandLine, " ");
	Dos9_Canonicalize(lpDir->str);
	Dos9_EsCatE(lpCommandLine, lpDir);
	Dos9_EsCat(lpCommandLine, "/");
	Dos9_EsCatE(lpCommandLine, lpMask);
	Dos9_EsCat(lpCommandLine, "'");

	if (Dos9_CmdForF(lpCommandLine, lpplLine, &forInfo))
		goto error;

	Dos9_EsFree(lpCommandLine);
	return 0;

error:

	Dos9_EsFree(lpCommandLine);
	return -1;

}
